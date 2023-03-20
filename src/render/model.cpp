// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <framework/stats.h>

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
        StaticModelData* data = (StaticModelData*)mData;

        CreateIndexedVertexArray(
            VERTEX_DEFINITION<StaticModelVertex>,
            vertex_buffer_handle,
            element_buffer_handle,
            vertex_array_handle, 
            data->vertices.size() * sizeof(StaticModelVertex),
            &data->vertices[0],
            data->indices.size() * sizeof(ModelIndex),
            &data->indices[0]
        );
        
        size_t approx_memory = (data->indices.size() * sizeof(ModelIndex)) + (data->vertices.size() * sizeof(StaticModelVertex));
        approx_vram_usage += approx_memory;
        //RESOURCE_VRAM_USAGE += approx_memory;
        Stats::Add(Stats::RESOURCE_VRAM, approx_memory);

        delete mData;
        mData = nullptr;

        status = READY;

        return;
    } else if (vertex_format == Model::DYNAMIC_VERTEX){
        DynamicModelData* data = (DynamicModelData*)mData;

        CreateIndexedVertexArray(
            VERTEX_DEFINITION<DynamicModelVertex>,
            vertex_buffer_handle,
            element_buffer_handle,
            vertex_array_handle, 
            data->vertices.size() * sizeof(DynamicModelVertex),
            &data->vertices[0],
            data->indices.size() * sizeof(ModelIndex),
            &data->indices[0]
        );

        size_t approx_memory = (data->indices.size() * sizeof(ModelIndex)) + (data->vertices.size() * sizeof(DynamicModelVertex));
        approx_vram_usage += approx_memory;
        //RESOURCE_VRAM_USAGE += approx_memory;
        Stats::Add(Stats::RESOURCE_VRAM, approx_memory);

        delete mData;
        mData = nullptr;

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

    struct VertexBucket {
        std::vector<ModelIndex> indices;
    };
    
    // index into this vector = material::type enum
    // increase VertexBucket() count if material::type count gets increased
    std::vector<VertexBucket> vertex_buckets {VertexBucket(), VertexBucket(), VertexBucket(), VertexBucket(), VertexBucket(), VertexBucket()};
    
    
    strcpy(path, "data/models/");
    strcat(path, name);
    strcat(path, ".stmdl");


    file.open(path);
    if (file.is_open()) {
        vertex_format = STATIC_VERTEX;
        StaticModelData* data = new StaticModelData;
        mData = data;

        std::cout << "Loading: " << path << std::endl;

        assert(data);

        uint32_t vcount = 0;    //number of vertices
        uint32_t tcount = 0;    //number of triangles
        uint32_t mcount = 0;    //number of materials
        file >> vcount;
        file >> tcount;
        file >> mcount;
        
        

        if(mcount > 15){
            std::cout << "Too many materials in model: " << name << std::endl;
            mcount = 15;
        }

        for(uint32_t i = 0; i < mcount; i++){
            Material* mat;
            std::string matName;
            file >> matName;

            mat = Material::Find(UID(matName));

            materials.push_back(mat);
        }
        
        for(uint32_t i = 0; i < vcount; i++){
            StaticModelVertex v;
            file >> v.co.x;
            file >> v.co.y;
            file >> v.co.z;
            file >> v.normal.x;
            file >> v.normal.y;
            file >> v.normal.z;
            file >> v.tex.x;
            file >> v.tex.y;
            file >> v.lighttex.x;
            file >> v.lighttex.y;
            v.texture = 0;
            data->vertices.push_back(v);
        }
        
        for(uint32_t i = 0; i < tcount; i++){
            ModelIndex t;
            Material::Type matType;
            uint32_t matIndex;
            file >> t.tri.x;
            file >> t.tri.y;
            file >> t.tri.z;
            file >> matIndex;

            matType = materials[matIndex]->GetType();

            if(matIndex > 15 - 1) matIndex = 0;
            vertex_buckets[matType].indices.push_back(t);

            data->vertices[t.tri.x].texture = matIndex;
            data->vertices[t.tri.y].texture = matIndex;
            data->vertices[t.tri.z].texture = matIndex;
        }
        
        file.close();

        data->indices.reserve(tcount);

        for (size_t i = 0; i < 6; i++) {
            if (vertex_buckets[i].indices.size() > 0) {
                ElementRange range;
                range.material_type = (Material::Type)i;
                range.material_count = materials.size();
                for (uint32_t k = 0; k < 16; k++) range.materials[k] = k;
                range.element_length = vertex_buckets[i].indices.size();
                range.element_offset = data->indices.size();
                
                element_ranges.push_back(range);
                data->indices.insert(data->indices.end(), vertex_buckets[i].indices.begin(), vertex_buckets[i].indices.end());
            }
        }

        Bone rootbone;
        rootbone.parentIndex = -1;
        rootbone.name = UID("Root");
        armature.push_back(rootbone);

        status = LOADED;
        //std::cout << "MODEL just got loaded with status" << status << std::endl;
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
        mData = data;

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

        if(mcount > 15){
            std::cout << "Too many materials in model: " << name << std::endl;
            mcount = 15;
        }

        for(uint32_t i = 0; i < mcount; i++){
            Material* mat;
            std::string matName;
            file >> matName;

            mat = Material::Find(UID(matName));

            materials.push_back(mat);

        }
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
        for(uint32_t i = 0; i < tcount; i++){
            ModelIndex t;
            uint32_t matIndex;
            Material::Type matType;
            file >> t.tri.x;
            file >> t.tri.y;
            file >> t.tri.z;
            file >> matIndex;

            matType = materials[matIndex]->GetType();

            if(matIndex > 15 - 1) matIndex = 0;
            vertex_buckets[matType].indices.push_back(t);

            data->vertices[t.tri.x].texture = matIndex;
            data->vertices[t.tri.y].texture = matIndex;
            data->vertices[t.tri.z].texture = matIndex;
        }
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
        for(uint32_t i = 0; i < gcount; i++){
            std::string g;
            file >> g;
            data->groups.push_back(g);
        }
        file.close();


        data->indices.reserve(tcount);

        for (size_t i = 0; i < 6; i++) {
            if (vertex_buckets[i].indices.size() > 0) {
                ElementRange range;
                range.material_type = (Material::Type)i;
                range.material_count = materials.size();
                for (uint32_t k = 0; k < 16; k++) range.materials[k] = k;
                range.element_length = vertex_buckets[i].indices.size();
                range.element_offset = data->indices.size();
                
                element_ranges.push_back(range);
                data->indices.insert(data->indices.end(), vertex_buckets[i].indices.begin(), vertex_buckets[i].indices.end());
            }
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
