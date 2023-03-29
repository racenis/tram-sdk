// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <framework/stats.h>
#include <framework/file.h>

#include <render/model.h>
#include <render/renderer.h>
#include <render/vertices.h>

#include <framework/async.h>

#include <fstream>
#include <cstring>

#include <templates/hashmap.h>

using namespace tram;
using namespace tram::Render;
using namespace tram::Render;

Model* ERROR_MODEL = nullptr;

Hashmap<Model*> MODEL_LIST("model name list", 500);
template <> Pool<Model> PoolProxy<Model>::pool("model pool", 500);

void Model::LoadErrorModel() {
    ERROR_MODEL = Model::Find(UID("errorstatic"));
    ERROR_MODEL->Load();
}

Model* Model::Find(name_t name){
    Model* model = MODEL_LIST.Find(name);
    
    if (!model) {
        model = PoolProxy<Model>::New(name);
        MODEL_LIST.Insert(UID(name), model);
    }
    
    return model;
}

void Model::LoadFromMemory(){
    assert(status == LOADED);
    
    if (vertex_format == Model::STATIC_VERTEX){
        StaticModelData* data = (StaticModelData*) model_data;

        CreateIndexedVertexArray(
            VERTEX_DEFINITION<StaticModelVertex>,
            vertex_buffer_handle,
            element_buffer_handle,
            vertex_array_handle, 
            data->vertices.size() * sizeof(StaticModelVertex),
            &data->vertices[0],
            data->indices.size() * sizeof(Triangle),
            &data->indices[0]
        );
        
        size_t approx_memory = (data->indices.size() * sizeof(Triangle)) + (data->vertices.size() * sizeof(StaticModelVertex));
        approx_vram_usage += approx_memory;
        Stats::Add(Stats::RESOURCE_VRAM, approx_memory);

        delete model_data;
        model_data = nullptr;

        status = READY;

        return;
    } else if (vertex_format == Model::DYNAMIC_VERTEX){
        DynamicModelData* data = (DynamicModelData*) model_data;

        CreateIndexedVertexArray(
            VERTEX_DEFINITION<DynamicModelVertex>,
            vertex_buffer_handle,
            element_buffer_handle,
            vertex_array_handle, 
            data->vertices.size() * sizeof(DynamicModelVertex),
            &data->vertices[0],
            data->indices.size() * sizeof(Triangle),
            &data->indices[0]
        );

        size_t approx_memory = (data->indices.size() * sizeof(Triangle)) + (data->vertices.size() * sizeof(DynamicModelVertex));
        approx_vram_usage += approx_memory;
        //RESOURCE_VRAM_USAGE += approx_memory;
        Stats::Add(Stats::RESOURCE_VRAM, approx_memory);

        delete model_data;
        model_data = nullptr;

        status = READY;

        return;
    }
}


bool Model::Unload(){
    // TODO: add the code that deletes models
    // like the gldeletebuffers()
    return false;
}



// TODO: make bucketing system better
// right now you can only have one bucket per material
// you should be able to have multiple buckets of the same material,
// so that you can have more than 15 materials of the same type in a model

// also you should have the same logic for both model types, instead of those huge branches

// and iostream is kinda slow, so a better file reading thing should be used
void Model::LoadFromDisk(){
    assert(status == UNLOADED);
    std::ifstream file;
    char path[200];

    struct TriangleBucket {
        Material::Type material_type;       // material type for this bucket
        std::vector<uint32_t> materials;    // which materials have already been added to the bucket
        std::vector<Triangle> triangles;    // triangle indices in the bucket
    };
    
    // TODO: make this autoexpand, not predefined
    std::vector<TriangleBucket> triangle_buckets {TriangleBucket(), TriangleBucket(), TriangleBucket(), TriangleBucket(), TriangleBucket(), TriangleBucket()};
    

    // trying to load model as a static model, text mode
    strcpy(path, "data/models/");
    strcat(path, name);
    strcat(path, ".stmdl");

    if (File file (path, MODE_READ); file.is_open()) {
        vertex_format = STATIC_VERTEX;
        StaticModelData* data = new StaticModelData;
        model_data = data;

        std::cout << "Loading: " << path << std::endl;

        assert(data);

        uint32_t vcount = file.read_uint32();   // number of vertices
        uint32_t tcount = file.read_uint32();   // number of triangles
        uint32_t mcount = file.read_uint32();   // number of materials

        for (uint32_t i = 0; i < mcount; i++) {
            materials.push_back(Material::Find(file.read_name()));
        }
        
        for (uint32_t i = 0; i < vcount; i++) {
            data->vertices.push_back(StaticModelVertex {
                .co = {
                    file.read_float32(),
                    file.read_float32(),
                    file.read_float32()
                },
                
                .normal = {
                    file.read_float32(),
                    file.read_float32(),
                    file.read_float32()
                },
                
                .tex = {
                    file.read_float32(),
                    file.read_float32()
                },
                
                .lighttex = {
                    file.read_float32(),
                    file.read_float32()
                },
    
                .texture = 0 // will be filled in later
            });
        }
        
        for (uint32_t i = 0; i < tcount; i++) {
            Triangle index {
                .indices = {
                    file.read_uint32(),
                    file.read_uint32(),
                    file.read_uint32()
                }
            };
            
            uint32_t material_index = file.read_uint32();
            Material::Type material_type = materials[material_index]->GetType();
            
            size_t bucket = 0;
            
            // try to find the bucket that contains this triangle's material
            for (; bucket < triangle_buckets.size(); bucket++) {
                if (triangle_buckets[bucket].material_type != material_type) continue;
                
                // check if material is in the bucket
                for (size_t i = 0; i < triangle_buckets[bucket].materials.size(); i++) {
                    if (triangle_buckets[bucket].materials[i] == material_index) {
                        goto found_bucket;
                    }
                }
                
                // otherwise try to add it to the bucket
                if (triangle_buckets[bucket].materials.size() < 15) {
                    triangle_buckets[bucket].materials.push_back(material_index);
                    goto found_bucket;
                }
            }            
            
            found_bucket:

            if (bucket < triangle_buckets.size()) {
                triangle_buckets[bucket].triangles.push_back(index);
            } else {
                assert(triangle_buckets.size() < 3);
                
                triangle_buckets.push_back(TriangleBucket {
                    .material_type = material_type,
                    .materials = {material_index},
                    .triangles = {index}
                });
            }

            data->vertices[index.indices.x].texture = material_index;
            data->vertices[index.indices.y].texture = material_index;
            data->vertices[index.indices.z].texture = material_index;
        }

        for (auto& bucket : triangle_buckets) {
            ElementRange range {
                .element_offset = (uint32_t) data->indices.size(),
                .element_length = (uint32_t) bucket.triangles.size(),
                .material_count = (uint32_t) bucket.materials.size(),
                .material_type = bucket.material_type,
            };
            
            for (size_t i = 0; i < bucket.materials.size(); i++) {
                range.materials[i] = bucket.materials[i];
            }
            
            element_ranges.push_back(range);
            data->indices.insert(data->indices.end(), bucket.triangles.begin(), bucket.triangles.end());
        }

        Bone rootbone {
            .name = UID("Root"),
            .parentIndex = (uint32_t) -1,
            .head = {0.0f, 0.0f, 0.0f},
            .tail = {0.0f, 1.0f, 0.0f},
            .roll = 0.0f
        };

        armature.push_back(rootbone);

        status = LOADED;

        for (size_t i = 0; i < materials.size(); i++){
            materials[i]->AddRef();
            Async::ForceLoadResource(materials[i]);
        }

        return;
    }



    // ok, the model isn't static
    // try opening it as a dynamic model

    strcpy(path, "data/models/");
    strcat(path, name);
    strcat(path, ".dymdl");

    file.open(path);
    if (file.is_open()) {
        vertex_format = DYNAMIC_VERTEX;
        DynamicModelData* data = new DynamicModelData;
        model_data = data;

        std::cout << "Loading: " << path << std::endl;

        assert(data);
        
        uint32_t f_version = 0;
        char ver_name[20];
        file >> ver_name;
        if (strcmp(ver_name, "DYMDLv1") == 0) f_version = 1;

        uint32_t vcount = 0;     // number of vertices
        uint32_t tcount = 0;     // number of triangles
        uint32_t mcount = 0;     // number of materials
        uint32_t bcount = 0;     // number of bones
        uint32_t gcount = 0;     // number of vertex groups
        
        if (f_version == 1) file >> vcount;
        else vcount = atoi(ver_name);
        file >> tcount;
        file >> mcount;
        file >> bcount;
        file >> gcount;
        
        

        std::cout << "f1" << std::endl;

        if(mcount > 15){
            std::cout << "Too many materials in model: " << name << std::endl;
            mcount = 15;
        }

        std::cout << "f2" << std::endl;

        for(uint32_t i = 0; i < mcount; i++){
            Material* mat;
            std::string matName;
            file >> matName;

            mat = Material::Find(UID(matName));

            materials.push_back(mat);

        }
        
        std::cout << "f3 " << vcount << " aa " << sizeof(DynamicModelVertex) * vcount << std::endl;
        
        for(uint32_t i = 0; i < vcount; i++){
            DynamicModelVertex v;
            file >> v.co.x;
            file >> v.co.y;
            file >> v.co.z;
            file >> v.normal.x;
            file >> v.normal.y;
            file >> v.normal.z;
            file >> v.tex.x;
            file >> v.tex.y;
            file >> v.bone.x;
            file >> v.boneweight.x;
            file >> v.bone.y;
            file >> v.boneweight.y;
            file >> v.bone.z;
            file >> v.boneweight.z;
            file >> v.bone.w;
            file >> v.boneweight.w;
            v.texture = 0;
            data->vertices.push_back(v);
        }
        
        std::cout << "f4" << std::endl;
        
        for(uint32_t i = 0; i < tcount; i++){
            Triangle t;
            uint32_t matIndex;
            Material::Type matType;
            file >> t.indices.x;
            file >> t.indices.y;
            file >> t.indices.z;
            file >> matIndex;

            matType = materials[matIndex]->GetType();

            if(matIndex > 15 - 1) matIndex = 0;
            triangle_buckets[matType].triangles.push_back(t);

            data->vertices[t.indices.x].texture = matIndex;
            data->vertices[t.indices.y].texture = matIndex;
            data->vertices[t.indices.z].texture = matIndex;
        }
        
        std::cout << "f5" << std::endl;
        
        for(uint32_t i = 0; i < bcount; i++){
            Bone b;
            std::string bonename;

            file >> bonename;
            file >> b.parentIndex;
            file >> b.head.x;
            file >> b.head.y;
            file >> b.head.z;
            file >> b.tail.x;
            file >> b.tail.y;
            file >> b.tail.z;
            if (f_version == 1) file >> b.roll;

            b.name = UID(bonename);
            armature.push_back(b);
        }
        
        std::cout << "f6" << std::endl;
        
        for(uint32_t i = 0; i < gcount; i++){
            std::string g;
            file >> g;
            data->groups.push_back(g);
        }
        file.close();
        std::cout << "f7" << std::endl;

        //data->indices.reserve(tcount);

        for (size_t i = 0; i < 6; i++) {
            if (triangle_buckets[i].triangles.size() > 0) {
                ElementRange range;
                range.material_type = (Material::Type)i;
                range.material_count = materials.size();
                for (uint32_t k = 0; k < 15; k++) range.materials[k] = k;
                range.element_length = triangle_buckets[i].triangles.size();
                range.element_offset = data->indices.size();
                
                element_ranges.push_back(range);
                data->indices.insert(data->indices.end(), triangle_buckets[i].triangles.begin(), triangle_buckets[i].triangles.end());
            }
        }

std::cout << "f" << std::endl;

        // push references into material
        status = LOADED;
        for (size_t i = 0; i < materials.size(); i++){
            materials[i]->AddRef();
            Async::ForceLoadResource(materials[i]);
        }

        return;
    }

    // ok, so the model isn't static or dynamic
    // we have no other model types, so it means that there actually isn't any usable model

    //status = LOAD_FAIL;

        // if we get this far, then it means that the model wasn't loaded from disk

    std::cout << "Model file for " << name << " couldn't be accessed!" << std::endl;

    // copy the error model stuff
    //vao = error_model->vao;
    //vbo = error_model->vbo;
    //ebo = error_model->ebo;

    //for (size_t i = 0; i < 6; i++){
    //    eboLen[i] = error_model->eboLen[i];
    //    eboOff[i] = error_model->eboOff[i];
    //    eboMat[i] = error_model->eboMat[i];
    //}
    
    vertex_array_handle = ERROR_MODEL->vertex_array_handle;
    vertex_buffer_handle = ERROR_MODEL->vertex_buffer_handle;
    element_buffer_handle = ERROR_MODEL->element_buffer_handle;
    
    element_ranges = ERROR_MODEL->element_ranges;

    materials = ERROR_MODEL->materials;
    armature = ERROR_MODEL->armature;

    status = READY;
    load_fail = true;
    //AddToModelReminderQueue(this);

    return;
}
