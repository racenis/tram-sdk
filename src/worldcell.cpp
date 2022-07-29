// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// WORLDCELL.CPP -- implementations for the worldcell and transition class methods.

#include <core.h>
#include <fstream>
#include <cstring>

using namespace Core;

Pool<WorldCell> WorldCell::cellPool("worldcellpool", 20, false);
std::unordered_map<uint64_t, WorldCell*> WorldCell::List;
PoolAddOnly<unsigned char> WorldCell::Transition::Pool("transition pool", 2000);

WorldCell::WorldCell(uint64_t cellName, glm::vec3 center, bool isInterior, bool hasInteriorLighting){
    name = cellName;
    origin = center;
    interior = isInterior;
    interiorLights = hasInteriorLighting;
    List[name] = this;
    std::cout << "Added cell: " << ReverseUID(name) << std::endl;
};

WorldCell* WorldCell::Find(name_t name){
    std::unordered_map<uint64_t, WorldCell*>::iterator ff = WorldCell::List.find(name);

    if(ff == WorldCell::List.end()){
        return nullptr;
    } else {
        return ff->second;
    }
}

WorldCell* WorldCell::Find (glm::vec3& point){
    WorldCell* ptr = cellPool.begin();
    for(; ptr < cellPool.end(); ptr++){
        if(*((uint64_t*) ptr) == 0) continue;
        if(!ptr->IsInterior()) continue;
        if(ptr->IsInside(point)) return ptr;
    }

    ptr = cellPool.begin();
    for(; ptr < cellPool.end(); ptr++){
        if(*((uint64_t*) ptr) == 0) continue;
        if(ptr->IsInterior()) continue;
        if(ptr->IsInside(point)) return ptr;
    }

    return nullptr;
}

void WorldCell::Load(){
    std::cout << "Loading cell: " << ReverseUID(name) << std::endl;
    for (auto& it : entities)
        if(it->IsAutoLoad()) it->Load();

    loaded = true;
};

void WorldCell::Unload(){
    std::cout << "Unloading cell: " << ReverseUID(name) << std::endl;
    for (auto& it : entities)
        if(it->IsAutoLoad()) it->Unload();


    loaded = false;
};


void WorldCell::LoadFromDisk(){
    //std::cout << "DONT USE THIS METHOD" << std::endl;
    //return;
    char path[100] = "data/";
    strcat(path, ReverseUID(name));
    strcat(path, ".cell");
    std::ifstream file;
    file.open(path);

    if (!file){ std::cout << "Can't find worldcell: " << path << std::endl; return; }

    std::string line;

    while (std::getline(file, line)){
        size_t firstchar = line.find_first_not_of(" \t");
        if (firstchar != std::string::npos && line[firstchar] == '#') continue;
        std::cout << "line : " << line << std::endl;

        std::string_view line_view (line);
        //Entity* ent = MakeEntityFromString(lvew);
        Entity* ent = Entity::Make(line_view);
        this->AddEntity(ent);
    }
};

void WorldCell::AddEntity(Entity* entPtr){
    entities.push_back(entPtr);

    entPtr->Init();

    if (!(entPtr->cell)) entPtr->location += origin;
    entPtr->cell = this;

    if(loaded && !entPtr->isloaded && entPtr->auto_load){
        entPtr->Load();
    }

    if (!loaded && entPtr->isloaded && entPtr->auto_load){
        // or delete if not persistent or whatever
        entPtr->Unload();
    }
}

WorldCell::Transition::Transition(WorldCell* cell, size_t planecount){
    into = cell;
    max_planes = planecount;
    current_planes = 0;
    firstplane = (glm::vec4*) Pool.AddNew(planecount * sizeof(glm::vec4));
}

void WorldCell::Transition::AddPlane(glm::vec4& nplane){
    if(current_planes == max_planes) return;
    firstplane[current_planes] = nplane;
    current_planes++;
}

bool WorldCell::Transition::IsInside(glm::vec3& point){
    glm::vec3 pp;
    into->GetOrigin(pp);

    pp = point - pp;

    for(size_t i = 0; i < current_planes; i++)
        if(glm::dot(firstplane[i], glm::vec4(pp, 1.0f)) < 0.0f) return false;

    return true;
}