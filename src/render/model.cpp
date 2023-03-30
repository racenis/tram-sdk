// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <framework/stats.h>
#include <framework/file.h>
#include <framework/logging.h>

#include <render/model.h>
#include <render/renderer.h>
#include <render/vertices.h>
#include <render/error.h>

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
    
    if (vertex_format == VERTEX_STATIC){
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
    } else if (vertex_format == VERTEX_DYNAMIC){
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


void Model::LoadFromDisk(){
    assert(status == UNLOADED);
    std::ifstream file;
    char path[200];

    struct TriangleBucket {
        materialtype_t material_type;       // material type for this bucket
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
        vertex_format = VERTEX_STATIC;
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
            materialtype_t material_type = materials[material_index]->GetType();
            
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

    if (File file (path, MODE_READ); file.is_open()) {
        vertex_format = VERTEX_DYNAMIC;
        DynamicModelData* data = new DynamicModelData;
        model_data = data;

        std::cout << "Loading: " << path << std::endl;

        assert(data);
        
        name_t file_version = file.read_name();
        
        if (file_version != UID("DYMDLv1")) {
            std::cout << "Model " << path << " is not using right DYMDLv1 version!" << std::endl;
            std::cout << "Add \"DYMDLv1\" to file and also add bone roll to the bone definitions (0.0 to the end of lines), or reexport." << std::endl;
        }
        
        
        uint32_t vcount = file.read_uint32();   // number of vertices
        uint32_t tcount = file.read_uint32();   // number of triangles
        uint32_t mcount = file.read_uint32();   // number of materials
        uint32_t bcount = file.read_uint32();   // number of bones
        uint32_t gcount = file.read_uint32();   // number of vertex groups


        for (uint32_t i = 0; i < mcount; i++) {
            materials.push_back(Material::Find(file.read_name()));
        }
        
        for (uint32_t i = 0; i < vcount; i++) {
            DynamicModelVertex vertex {
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
                
                // this looks stupid, but its because I accidentally made the
                // model format stupid. maybe will be fixed in DYMDLv2
                .bone = {0, 0, 0, 0},
                
                .boneweight = {0.0f, 0.0f, 0.0f, 0.0f},
    
                .texture = 0 // will be filled in later
            };
            
            vertex.bone.x = file.read_uint32();
            vertex.boneweight.x = file.read_float32();
            vertex.bone.y = file.read_uint32();
            vertex.boneweight.y = file.read_float32();
            vertex.bone.z = file.read_uint32();
            vertex.boneweight.z = file.read_float32();
            vertex.bone.w = file.read_uint32();
            vertex.boneweight.w = file.read_float32();

            data->vertices.push_back(vertex);
            
            if (i % 100 == 0) std::cout << i << std::endl;
        }
        
        // this is basically a repetion of the same code as for static model
        // so, it could be possible to make this a function.
        for (uint32_t i = 0; i < tcount; i++) {
            Triangle index {
                .indices = {
                    file.read_uint32(),
                    file.read_uint32(),
                    file.read_uint32()
                }
            };
            
            uint32_t material_index = file.read_uint32();
            materialtype_t material_type = materials[material_index]->GetType();
            
            size_t bucket = 0;
            
            // try to find the bucket that contains this triangle's material
            for (; bucket < triangle_buckets.size(); bucket++) {
                if (triangle_buckets[bucket].material_type != material_type) continue;
                
                // check if material is in the bucket
                for (size_t i = 0; i < triangle_buckets[bucket].materials.size(); i++) {
                    if (triangle_buckets[bucket].materials[i] == material_index) {
                        goto found_bucket2;
                    }
                }
                
                // otherwise try to add it to the bucket
                if (triangle_buckets[bucket].materials.size() < 15) {
                    triangle_buckets[bucket].materials.push_back(material_index);
                    goto found_bucket2;
                }
            }            
            
            found_bucket2:

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
        
        for (uint32_t i = 0; i < bcount; i++) {
            armature.push_back(Bone {
                .name = file.read_name(),
                .parentIndex = file.read_uint32(),
                
                .head = {
                    file.read_float32(),
                    file.read_float32(),
                    file.read_float32()
                },
                
                .tail = {
                    file.read_float32(),
                    file.read_float32(),
                    file.read_float32()
                },
                
                .roll = file.read_float32()
            });
        }
        
        for (uint32_t i = 0; i < gcount; i++) {
            name_t group = file.read_name();
            
            if (armature[i].name != group) {
                std::cout << "Model " << name << " group " << group << " is not matching bone " << armature[i].name << "!" << std::endl;
            }
            
            data->groups.push_back(group);
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
    
    //vertex_array_handle = ERROR_MODEL->vertex_array_handle;
    //vertex_buffer_handle = ERROR_MODEL->vertex_buffer_handle;
    //element_buffer_handle = ERROR_MODEL->element_buffer_handle;
    
    //element_ranges = ERROR_MODEL->element_ranges;

    //materials = ERROR_MODEL->materials;
    //armature = ERROR_MODEL->armature;

    //status = READY;
    
    vertex_format = VERTEX_STATIC;

    auto data = MakeNewErrorModel();
    model_data = data;

    materials.push_back(Material::Find("defaulttexture"));

    element_ranges.push_back(ElementRange {
        .element_offset = 0,
        .element_length = (uint32_t) data->indices.size(),
        .material_count = 1,
        .material_type = MATERIAL_TEXTURE,
        .materials = {0}
    });
    
    armature.push_back(Bone {
        .name = UID("Root"),
        .parentIndex = (uint32_t) -1,
        .head = {0.0f, 0.0f, 0.0f},
        .tail = {0.0f, 1.0f, 0.0f},
        .roll = 0.0f
    });

    status = LOADED;
    
    load_fail = true;
}
