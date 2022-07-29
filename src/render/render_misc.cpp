// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// RENDER_MISC.CPP -- random render stuff.

#include <core.h>
#include <render.h>

#include <fstream>

using namespace Core;
using namespace Core::Render;

void RenderListObject::FillFromModel(Model* mdl, uint32_t eboIndex){
    vbo = mdl->vbo;
    ebo = mdl->ebo;
    vao = mdl->vao;
    eboLen = mdl->eboLen[eboIndex];
    eboOff = mdl->eboOff[eboIndex];
    shader = FindShader(mdl->vertForm, (Material::Type)mdl->eboMat[eboIndex]);
    texCount = mdl->materials.size();
    for (uint32_t i = 0; i < texCount; i++){
        textures[i] = mdl->materials[i]->GetTexture();
    }

    flags = FLAG_RENDER; // TODO: get the RFLAG_INTERIOR_LIGHTING in here somehow
};

void Core::Render::CalcLights(RenderListObject* obj){
    // TODO: write a search tree and replace this all of this with it
    uint32_t lights[4] = {0};
    float distances[4] = {2000.0f};

    LightListObject* light = lightPool.begin();

    for (uint32_t i = 1; i < lightPool.GetSize(); i++){
        if (*((uint64_t*)(light + i)) == 0) continue;


        float dist = pow(light[i].location[0] - obj->location[0], 2.0);
        dist += pow(light[i].location[1] - obj->location[1], 2.0);
        dist += pow(light[i].location[2] - obj->location[2], 2.0);
        dist = sqrt(dist);

        for (uint32_t j = 0; j < 4; j++){
            if (distances[j] > dist){
                for (uint32_t k = 3; k > j; k--){
                    distances[k] = distances[k-1];
                    lights[k] = lights[k-1];
                }
                distances[j] = dist;
                lights[j] = i;
                break;
            }
        }

    }
    obj->lights[0] = lights[0];
    obj->lights[1] = lights[1];
    obj->lights[2] = lights[2];
    obj->lights[3] = lights[3];
};


std::unordered_map<name_t, NameCount*> Core::Render::Animation::animationlist;

void Core::Render::Animation::LoadFromDisk(){
    using namespace Render;
    std::ifstream file;

    std::string filename = "data/animations/";
    filename += ReverseUID(name);
    filename += ".anim";

    file.open(filename);

    if(!file.is_open()){std::cout << "Can't open " << filename << "!" << std::endl; return;}

    std::cout << "Loading animation: " << filename << std::endl;

    std::string noot;
    uint64_t ac;
    uint64_t bc;
    uint64_t kc;
    uint64_t a_name;
    NameCount* nameptr = (NameCount*)animationpool.begin();//animationList.last;
    Keyframe* kframe;


    file >> noot;       //not actually used for anything
    file >> ac;         //count of animation entries in the file

    for (uint64_t i = 0; i < ac; i++){
        file >> noot;       //name of animation as string
        file >> bc;         //count of bones used by animation
        a_name = UID(noot);   //uint64 hash of name string

        nameptr->first = a_name;
        nameptr->second = bc;
        animationlist[a_name] = nameptr;
        //animationList.AddAnimationPtr(a_name, nameptr);   //important (NOTT XDDD)

        nameptr++;


        for (uint64_t j = 0; j < bc; j++){
            file >> noot;       //name of bone as string
            file >> kc;         //count of keyframes for the bone
            a_name = UID(noot);   //uint64 hash of bone name

            nameptr->first = a_name;
            nameptr->second = kc;
            nameptr++;


            for (uint64_t k = 0; k < kc; k++){
                kframe = (Keyframe*)nameptr;
                file >> kframe->frame;
                //float r[4];
                file >> kframe->location.x;
                file >> kframe->location.y;
                file >> kframe->location.z;
                //file >> r[0];
                //file >> r[1];
                //file >> r[2];
                //file >> r[3];
                //kframe->rotation.x = r[1];
                //kframe->rotation.y = r[3];
                //kframe->rotation.z = -r[2];
                //kframe->rotation.w = r[0];
                file >> kframe->rotation.x;
                file >> kframe->rotation.y;
                file >> kframe->rotation.z;
                file >> kframe->rotation.w;
                file >> kframe->scale.x;
                file >> kframe->scale.y;
                file >> kframe->scale.z;
                kframe++;
                nameptr = (NameCount*)kframe;
            }


        }


    }

    //animationList.last = nameptr;
    //std::cout << "animation length: " << (int)(nameptr - animationList.first) << std::endl;
    //std::cout << "animlist length: " << (int)(animationList.last - animationList.first) << std::endl;

    file.close();
}

NameCount* Core::Render::Animation::Find(name_t name){
    std::unordered_map<uint64_t, NameCount*>::iterator ff = animationlist.find(name);

    if(ff == animationlist.end()){
        std::cout << "Can't find animation " << ReverseUID(name) << "!" << std::endl;
        return nullptr;
    } else {
        return ff->second;
    }
}


void Render::SetSun(float timeOfDay){
    struct sunparam {
        float time;
        glm::vec3 sundir;
        glm::vec3 suncolor;
        glm::vec3 ambientcolor;
    };

    // glm::vec3(0.0f, 1.0f, 0.0f)  <- sun directly above
    // glm::vec3(0.0f, 0.0f, 1.0f)  <- sun directly west
    // glm::vec3(0.0f, 0.0f, -1.0f) <- sun directly east
    // glm::vec3(1.0f, 0.0f, 0.0f)  <- sun directly south
    // glm::vec3(-1.0f, 0.0f, 0.0f) <- sun directly north

    sunparam suns[8];

    // midnight
    suns[0].time = 0.0f;
    suns[0].sundir = glm::vec3(0.0f, 1.0f, 0.0f);
    suns[0].suncolor = glm::vec3(230.0f*0.7f, 246.0f*0.7f, 255.0f*0.7f);
    suns[0].ambientcolor = suns[0].suncolor * 0.2f;

    // 5.00, just before dawn
    suns[1].time = 0.20833f;
    suns[1].sundir = glm::vec3(-0.3f, 1.0f, 0.0f);
    suns[1].suncolor = glm::vec3(202.0f, 216.0f, 224.0f);
    suns[1].ambientcolor = suns[1].suncolor * 0.2f;

    // 6.00, dawn
    suns[2].time = 0.25f;
    suns[2].sundir = glm::vec3(0.0f, 0.0f, -1.0f);
    suns[2].suncolor = glm::vec3(255.0f, 214.0f, 163.0f);
    suns[2].ambientcolor = suns[2].suncolor * 0.2f;

    // midday
    suns[3].time = 0.5f;
    suns[3].sundir = glm::vec3(0.0f, 1.0f, 0.0f);
    suns[3].suncolor = glm::vec3(255.0f, 255.0f, 255.0f);
    suns[3].ambientcolor = suns[3].suncolor * 0.2f;

    // 17.00 evening
    suns[4].time = 0.70833f;
    suns[4].sundir = glm::vec3(0.0f, 1.0f, 0.5f);
    suns[4].suncolor = glm::vec3(255.0f, 237.0f, 184.0f);
    suns[4].ambientcolor = suns[4].suncolor * 0.2f;

    // 22.00 sunset
    suns[5].time = 0.91666;
    suns[5].sundir = glm::vec3(0.0f, 0.0f, 1.0f);
    suns[5].suncolor = glm::vec3(250.0f, 214.0f, 165.0f);
    suns[5].ambientcolor = suns[5].suncolor * 0.2f;

    // 23.00 darkness
    suns[6].time = 0.95833f;
    suns[6].sundir = glm::vec3(0.0f, 1.0f, 0.3f);
    suns[6].suncolor = glm::vec3(230.0f*0.5f, 246.0f*0.5f, 255.0f*0.5f);
    suns[6].ambientcolor = suns[6].suncolor * 0.2f;

    // midnight, again
    suns[7].time = 1.0f;
    suns[7].sundir = suns[0].sundir;
    suns[7].suncolor = suns[0].suncolor;
    suns[7].ambientcolor = suns[0].ambientcolor;

    timeOfDay = fmod(timeOfDay, 1.0f);

    size_t t = 0;
    for (size_t i = 0; i < 7; i++){
        if(suns[i].time > timeOfDay) break;
        t = i;
    }

    size_t nt = (t + 1) % 8;

    float mixf = (timeOfDay - suns[t].time) / (suns[nt].time - suns[t].time);


    glm::vec3 sundir = glm::normalize(glm::mix(suns[t].sundir, suns[nt].sundir, mixf));
    glm::vec3 suncolor = glm::mix(suns[t].suncolor, suns[nt].suncolor, mixf) / 256.0f*1.2f;
    glm::vec3 ambientcolor = glm::mix(suns[t].ambientcolor, suns[nt].ambientcolor, mixf) / 256.0f*1.2f;

    Render::modelMatrices.sunDirection = glm::vec4(sundir.x, sundir.y, sundir.z, 1.0f);
    Render::modelMatrices.sunColor = glm::vec4(suncolor.x, suncolor.y, suncolor.z, 1.0f);
    Render::modelMatrices.ambientColor = glm::vec4(ambientcolor.x, ambientcolor.y, ambientcolor.z, 1.0f);
}