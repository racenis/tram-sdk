// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// MODEL.CPP
// Methods for the 3D model resource.

#include <glad.h>

#include <core.h>
#include <render.h>

#include <async.h>

#include <fstream>
#include <cstring>

using namespace Core;
using namespace Core::Render;

Model* Model::error_model = nullptr;
std::unordered_map<uint64_t, Model*> Model::List;

Model* Model::Find(name_t name){
    std::unordered_map<uint64_t, Model*>::iterator ff = List.find(name);
    Model* model;

    if(ff == List.end()){
        model = new Model(name);
        List[name] = model;
    } else {
        model = ff->second;
    }

    return model;
}

void Model::LoadFromMemory(){
//std::cout << "Loading from memory THE MODEL" << std::endl;
    if (vertForm == Model::STATIC_VERTEX && status == LOADED){
        StaticModelData* data = (StaticModelData*)mData;

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data->vertices.size() * sizeof(StaticModelVertex), &data->vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->indices.size() * sizeof(ModelIndex), &data->indices[0], GL_STATIC_DRAW);



        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StaticModelVertex), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(StaticModelVertex), (void*)offsetof(StaticModelVertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(StaticModelVertex), (void*)offsetof(StaticModelVertex, tex));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(StaticModelVertex), (void*)offsetof(StaticModelVertex, lighttex));
        glEnableVertexAttribArray(3);

        glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(StaticModelVertex), (void*)offsetof(StaticModelVertex, texture));
        glEnableVertexAttribArray(4);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        delete mData;
        mData = nullptr;

        status = READY;
        //AddToModelReminderQueue(this);

        return;
    } else if (vertForm == Model::DYNAMIC_VERTEX && status == LOADED){
        DynamicModelData* data = (DynamicModelData*)mData;

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data->vertices.size() * sizeof(DynamicModelVertex), &data->vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data->indices.size() * sizeof(ModelIndex), &data->indices[0], GL_STATIC_DRAW);



        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DynamicModelVertex), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DynamicModelVertex), (void*)offsetof(DynamicModelVertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(DynamicModelVertex), (void*)offsetof(DynamicModelVertex, tex));
        glEnableVertexAttribArray(2);

        glVertexAttribIPointer(3, 4, GL_UNSIGNED_INT, sizeof(DynamicModelVertex), (void*)offsetof(DynamicModelVertex, bone));
        glEnableVertexAttribArray(3);

        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(DynamicModelVertex), (void*)offsetof(DynamicModelVertex, boneweight));
        glEnableVertexAttribArray(4);

        glVertexAttribIPointer(5, 1, GL_UNSIGNED_INT, sizeof(DynamicModelVertex), (void*)offsetof(DynamicModelVertex, texture));
        glEnableVertexAttribArray(5);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

        delete mData;
        mData = nullptr;

        status = READY;
        //AddToModelReminderQueue(this);

        return;
    }
}


bool Model::Unload(){
    // TODO: add the code that deletes models
    // like the gldeletebuffers()
    return false;
}




void Model::LoadFromDisk(){
    std::ifstream file;
    char path[200];

    std::vector<ModelIndex> bucket[6];      //buckets for material types

    strcpy(path, "data/models/");
    strcat(path, ReverseUID(name));
    strcat(path, ".stmdl");


    file.open(path);
    if (file.is_open()) {
        vertForm = STATIC_VERTEX;
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

        if(mcount > MAX_MATERIALS_PER_MODEL){
            std::cout << "Too many materials in model: " << ReverseUID(name) << std::endl;
            mcount = MAX_MATERIALS_PER_MODEL;
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

            if(matIndex > MAX_MATERIALS_PER_MODEL - 1) matIndex = 0;
            bucket[matType].push_back(t);
            eboMat[matType] = matType;


            data->vertices[t.tri.x].texture = matIndex;
            data->vertices[t.tri.y].texture = matIndex;
            data->vertices[t.tri.z].texture = matIndex;
        }
        file.close();

        data->indices.reserve(tcount);

        for (size_t i = 0; i < 6; i++){
            eboOff[i] = data->indices.size();
            eboLen[i] = bucket[i].size();
            if (bucket[i].size() > 0){
                data->indices.insert(data->indices.end(), bucket[i].begin(), bucket[i].end());
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
    strcat(path, ReverseUID(name));
    strcat(path, ".dymdl");

    file.open(path);
    if (file.is_open()) {
        vertForm = DYNAMIC_VERTEX;
        DynamicModelData* data = new DynamicModelData;
        mData = data;

        std::cout << "Loading: " << path << std::endl;

        assert(data);

        uint32_t vcount = 0;     // number of vertices
        uint32_t tcount = 0;     // number of triangles
        uint32_t mcount = 0;     // number of materials
        uint32_t bcount = 0;     // number of bones
        uint32_t gcount = 0;     // number of vertex groups
        file >> vcount;
        file >> tcount;
        file >> mcount;
        file >> bcount;
        file >> gcount;

        if(mcount > MAX_MATERIALS_PER_MODEL){
            std::cout << "Too many materials in model: " << ReverseUID(name) << std::endl;
            mcount = MAX_MATERIALS_PER_MODEL;
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

            if(matIndex > MAX_MATERIALS_PER_MODEL - 1) matIndex = 0;
            bucket[matType].push_back(t);
            eboMat[matType] = matType;

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

            b.name = UID(bonename);
            armature.push_back(b);
        }
        for(uint32_t i = 0; i < gcount; i++){
            std::string g;
            file >> g;
            data->groups.push_back(UID(g));
        }
        file.close();


        data->indices.reserve(tcount);

        for (size_t i = 0; i < 6; i++){
            eboOff[i] = data->indices.size();
            eboLen[i] = bucket[i].size();
            if (bucket[i].size() > 0){
                data->indices.insert(data->indices.end(), bucket[i].begin(), bucket[i].end());
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

    std::cout << "Model " << ReverseUID(name) << " couldn't be loaded!" << std::endl;

    // copy the error model stuff
    vao = error_model->vao;
    vbo = error_model->vbo;
    ebo = error_model->ebo;

    for (size_t i = 0; i < 6; i++){
        eboLen[i] = error_model->eboLen[i];
        eboOff[i] = error_model->eboOff[i];
        eboMat[i] = error_model->eboMat[i];
    }

    materials = error_model->materials;
    armature = error_model->armature;

    status = READY;
    load_fail = true;
    //AddToModelReminderQueue(this);

    return;
}
