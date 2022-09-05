// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// WORLDCELL.CPP -- implementations for the worldcell and transition class methods.

#include <core.h>
#include <fstream>
#include <cstring>

#include <render.h>

using namespace Core;
std::unordered_map<uint64_t, WorldCell*> WorldCell::List;
//PoolAddOnly<unsigned char> WorldCell::Transition::Pool("transition pool", 2000);
template <> Pool<WorldCell> PoolProxy<WorldCell>::pool("worldcell pool", 250, false);
template <> Pool<WorldCell::Transition> PoolProxy<WorldCell::Transition>::pool("worldcelltransition pool", 250, false);


//WorldCell::WorldCell(uint64_t cellName, bool isInterior, bool hasInteriorLighting){
    //name = cellName;
    //interior = isInterior;
    //interiorLights = hasInteriorLighting;
    //List[name] = this;
    //std::cout << "Added cell: " << ReverseUID(name) << std::endl;
//};

void WorldCell::SetName(name_t new_name) {
    if (name) {
        abort();
        // TODO: add code to remove the cell from worldcell list
    }
    
    name = new_name;
    List[name] = this;
}

WorldCell* WorldCell::Find(name_t name){
    std::unordered_map<uint64_t, WorldCell*>::iterator ff = WorldCell::List.find(name);

    if(ff == WorldCell::List.end()){
        return nullptr;
    } else {
        return ff->second;
    }
}

WorldCell* WorldCell::Find (const glm::vec3& point){
    auto& cellPool = PoolProxy<WorldCell>::GetPool();
    WorldCell* ptr = cellPool.begin().ptr;
    for(; ptr < cellPool.end().ptr; ptr++){
        if(*((uint64_t*) ptr) == 0) continue;
        if(!ptr->IsInterior()) continue;
        if(ptr->IsInside(point)) return ptr;
    }

    ptr = cellPool.begin().ptr;
    for(; ptr < cellPool.end().ptr; ptr++){
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
        //std::cout << "line : " << line << std::endl;
        
        size_t first_name_end = line.find_first_of(" \t", firstchar);
        std::string type_name = line.substr(firstchar, first_name_end);
        
        std::cout << "ent type: " << type_name << std::endl;
        
        if (type_name == "transition" || type_name == "path" || type_name == "navmesh") {
            std::cout << "not an ent" << std::endl;
            continue;
        }

        std::string_view line_view (line);
        //Entity* ent = MakeEntityFromString(lvew);
        Entity* ent = Entity::Make(line_view);
        this->AddEntity(ent);
    }
};

void WorldCell::AddEntity(Entity* entPtr){
    entities.push_back(entPtr);

    entPtr->Init();

   // if (!(entPtr->cell)) entPtr->location += origin;
    entPtr->cell = this;

    if(loaded && !entPtr->isloaded && entPtr->auto_load){
        entPtr->Load();
    }

    if (!loaded && entPtr->isloaded && entPtr->auto_load){
        // or delete if not persistent or whatever
        entPtr->Unload();
    }
}

/*WorldCell::Transition::Transition(WorldCell* cell){
    into = cell;
    //max_planes = planecount;
    //current_planes = 0;
    //firstplane = (glm::vec4*) Pool.AddNew(planecount * sizeof(glm::vec4));
}*/

void WorldCell::Transition::AddPoint(const glm::vec3& point){
    points.push_back(point);
}

bool WorldCell::Transition::IsInside(const glm::vec3& point){
    for(size_t i = 0; i < planes.size(); i++)
        if(glm::dot(planes[i], glm::vec4(point, 1.0f)) < 0.0f) return false;    
    return true;
}

void WorldCell::Transition::GeneratePlanes(bool disp) {
        assert(points.size() > 3);
        
        planes.clear();
        
        for (size_t i = 0; i < points.size(); i++) {
            for (size_t j = i+1; j < points.size(); j++) {
                for (size_t k = j+1; k < points.size(); k++) {
                    auto& A = points[i];
                    auto& B = points[j];
                    auto& C = points[k];
                    auto AB = B - A;
                    auto AC = C - A;
                    auto cross = glm::cross(AB, AC);
                    auto d = -(cross.x*A.x + cross.y*A.y + cross.z*A.z);
                    
                    glm::vec4 plane = glm::vec4(cross, d);
                    
                    for (size_t it = 0; it < points.size(); it++) {
                        if(i == it || j == it || k == it) continue;
                        if(glm::dot(plane, glm::vec4(points[it], 1.0f)) < 0.0f) {
                            plane *= -1.0f;
                            goto tryagain;
                        }
                    }
                    
                    okay:
                    
                    if (disp) {
                        Render::AddLine(A, B, Render::COLOR_WHITE);
                        Render::AddLine(A, C, Render::COLOR_WHITE);
                        Render::AddLine(B, C, Render::COLOR_WHITE);
                        
                        Render::AddLine(A, A+(cross*0.1f)+glm::vec3(0.002f), Render::COLOR_CYAN);
                        Render::AddLine(B, B+(cross*0.1f)+glm::vec3(0.002f), Render::COLOR_CYAN);
                        Render::AddLine(C, C+(cross*0.1f)+glm::vec3(0.002f), Render::COLOR_CYAN);
                    }
                    
                    planes.push_back(plane);
                    
                    yeet:
                    continue;
                    
                    tryagain:
                    
                    for (size_t it = 0; it < points.size(); it++) {
                        if(i == it || j == it || k == it) continue;
                        if(glm::dot(plane, glm::vec4(points[it], 1.0f)) < 0.0f) {
                            goto yeet;
                        }
                    }
                    
                    goto okay;
                }
            }
        }
        
        std::sort(planes.begin(), planes.end(), [](const glm::vec4& a, const glm::vec4& b){ 
            if (a.x != b.x) return a.x < b.x;
            if (a.y != b.y) return a.y < b.y;
            if (a.z != b.z) return a.z < b.z;
            return a.w < b.w;});
        planes.erase(unique( planes.begin(), planes.end() ), planes.end());
        
        
    }
    
    void WorldCell::Draw(){
        for (auto& it : trans_in) it->GeneratePlanes(true);
    }
    
    void WorldCell::Loader::LoadCells() {
        
        }