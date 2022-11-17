// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// WORLDCELL.CPP -- implementations for the worldcell and transition class methods.

#include <core.h>
#include <fstream>
#include <cstring>
#include <sstream>

#include <set>

#include <render/render.h>
#include <navigation.h>

using namespace Core;
std::unordered_map<uint64_t, WorldCell*> WorldCell::List;
template <> Pool<WorldCell> PoolProxy<WorldCell>::pool("worldcell pool", 250, false);
template <> Pool<WorldCell::Transition> PoolProxy<WorldCell::Transition>::pool("worldcelltransition pool", 250, false);
template <> Pool<WorldCell::Loader> PoolProxy<WorldCell::Loader>::pool("worldcellloader pool", 10, false);

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
    auto entities_copy = entities;
    for (auto& it : entities_copy) {
        if (it->IsAutoLoad()) {
            if (it->IsPersistent()) {
                it->Unload();
            } else {
                std::cout << "Yeeting " << ReverseUID(it->GetName()) << " out of existance!" << std::endl;
                //it->Yeet();
                it->~Entity();
                // TODO: actually delete the entity
            }
        }
    }

    loaded = false;
};

void WorldCell::Loader::LoadCells() {
    std::set<WorldCell*> active_cells;
    auto& loader_pool = PoolProxy<Loader>::GetPool();
    auto& cell_pool = PoolProxy<WorldCell>::GetPool();
    
    for (auto& loader : loader_pool) {
        if (loader.current_cell) {
            active_cells.insert(loader.current_cell);
            for (auto trans : loader.current_cell->trans_out) {
                active_cells.insert(trans->into);
            }
        }
    }
    
    for (auto& cell : cell_pool) {
        if(cell.IsLoaded() && !active_cells.contains(&cell)) {
            cell.Unload();
        }
    }
    
    for (auto cell : active_cells) {
        if (!cell->IsLoaded()) cell->Load();
    }
}

void WorldCell::LoadFromDisk(){
    char path[100] = "data/";
    strcat(path, ReverseUID(name));
    strcat(path, ".cell");
    std::ifstream file;
    file.open(path);

    if (!file){ std::cout << "Can't find worldcell: " << path << std::endl; return; }
    std::cout << "Loading worldcell: " << ReverseUID(name) << std::endl;

    std::string line;

    while (std::getline(file, line)){
        size_t firstchar = line.find_first_not_of(" \t");
        if (firstchar != std::string::npos && line[firstchar] == '#') continue;
        //std::cout << "line : " << line << std::endl;
        
        size_t first_name_end = line.find_first_of(" \t", firstchar);
        std::string type_name = line.substr(firstchar, first_name_end);
        
        if (type_name == "transition") {
            // TODO: store the transition name in a lookupable map
            auto trans = PoolProxy<WorldCell::Transition>::New();
            std::stringstream ss (line.substr(first_name_end));
            std::string name;
            ss >> name;
            trans->name = UID(name);
            ss >> name;
            name_t into_cell_name = UID(name);
            size_t pointcount;
            ss >> pointcount;
            for (size_t i = 0; i < pointcount; i++) {
                glm::vec3 point;
                ss >> point.x;
                ss >> point.y;
                ss >> point.z;
                trans->AddPoint(point);
            }
            
            auto into_cell_ptr = WorldCell::Find(into_cell_name);
            assert(into_cell_ptr);
            
            trans->SetInto(into_cell_ptr);
            trans->GeneratePlanes(false);
            
            if (into_cell_ptr == this) {
                AddTransition(trans);
            } else {
                AddTransitionFrom(trans);
            }
            
            continue;
        } else if (type_name == "path") {
            // TODO: store the path name in a lookupable map
            auto path = PoolProxy<Path>::New();
            std::stringstream ss (line.substr(first_name_end));
            std::string name;
            ss >> name;
            path->name = UID(name);
            size_t pointcount;
            ss >> pointcount;
            std::unordered_map<uint32_t, Path::Node*> id_lookup;
            for (size_t i = 0; i < pointcount; i++) {
                uint32_t id;
                ss >> id;
                Path::Node nod;
                ss >> *((uint32_t*)&nod.next);
                ss >> *((uint32_t*)&nod.prev);
                ss >> *((uint32_t*)&nod.left);
                ss >> *((uint32_t*)&nod.right);
                ss >> nod.p1.x;
                ss >> nod.p1.y;
                ss >> nod.p1.z;
                ss >> nod.p2.x;
                ss >> nod.p2.y;
                ss >> nod.p2.z;
                ss >> nod.p3.x;
                ss >> nod.p3.y;
                ss >> nod.p3.z;
                ss >> nod.p4.x;
                ss >> nod.p4.y;
                ss >> nod.p4.z;
                path->nodes.push_back(nod);
                id_lookup[id] = &(path->nodes.back());
            }
            for (auto& it : path->nodes) {
                it.next = id_lookup[*((uint32_t*)&it.next)];
                it.prev = id_lookup[*((uint32_t*)&it.prev)];
                it.left = id_lookup[*((uint32_t*)&it.left)];
                it.right = id_lookup[*((uint32_t*)&it.right)];
            }
            paths.push_back(path);
            continue;
        } else if (type_name == "navmesh") {
            // TODO: store the path name in a lookupable map
            auto navmesh = PoolProxy<Navmesh>::New();
            std::stringstream ss (line.substr(first_name_end));
            std::string name;
            ss >> name;
            navmesh->name = UID(name);
            size_t pointcount;
            ss >> pointcount;
            std::unordered_map<uint32_t, Navmesh::Node*> id_lookup;
            for (size_t i = 0; i < pointcount; i++) {
                uint32_t id;
                ss >> id;
                Navmesh::Node nod;
                ss >> *((uint32_t*)&nod.next);
                ss >> *((uint32_t*)&nod.prev);
                ss >> *((uint32_t*)&nod.left);
                ss >> *((uint32_t*)&nod.right);
                ss >> nod.location.x;
                ss >> nod.location.y;
                ss >> nod.location.z;
                navmesh->nodes.push_back(nod);
                id_lookup[id] = &(navmesh->nodes.back());
                Navmesh::all_nodes.AddLeaf(&(navmesh->nodes.back()), nod.location.x, nod.location.y, nod.location.z);
            }
            for (auto& it : navmesh->nodes) {
                it.next = id_lookup[*((uint32_t*)&it.next)];
                it.prev = id_lookup[*((uint32_t*)&it.prev)];
                it.left = id_lookup[*((uint32_t*)&it.left)];
                it.right = id_lookup[*((uint32_t*)&it.right)];
            }
            navmeshes.push_back(navmesh);
            continue;
        }

        std::string_view line_view (line);
        Entity* ent = Entity::Make(line_view);
        if (ent == nullptr) {
            std::cout << "Entity type '" << type_name << "' not recognized; in file " << path << std::endl;
            continue;
        }
        
        this->AddEntity(ent);
    }
};

void WorldCell::AddEntity(Entity* entity){
    entities.push_back(entity);

    //entity->Init();

    entity->cell = this;

    if(loaded && !entity->isloaded && entity->auto_load){
        entity->Load();
    }

    if (!loaded && entity->isloaded && entity->auto_load){
        entity->Unload();
    }
}

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
                    
                    Render::AddLine(A, A+(glm::normalize(cross)*0.5f)+glm::vec3(0.002f), Render::COLOR_CYAN);
                    Render::AddLine(B, B+(glm::normalize(cross)*0.1f)+glm::vec3(0.002f), Render::COLOR_CYAN);
                    Render::AddLine(C, C+(glm::normalize(cross)*0.1f)+glm::vec3(0.002f), Render::COLOR_CYAN);
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
    
void WorldCell::DrawTransitions(){
    for (auto& it : trans_in) it->GeneratePlanes(true);
    for (auto& it : trans_out) it->GeneratePlanes(true);
}

void WorldCell::DrawNavmeshes() {
    for (auto nv : navmeshes)
        for (auto& nd : nv->nodes) {
            Render::AddLineMarker(nd.location, Render::COLOR_YELLOW);
            if (nd.next) Render::AddLine(nd.location, nd.next->location, Render::COLOR_YELLOW);
            if (nd.prev) Render::AddLine(nd.location, nd.prev->location, Render::COLOR_YELLOW);
            if (nd.left) Render::AddLine(nd.location, nd.left->location, Render::COLOR_YELLOW);
            if (nd.right) Render::AddLine(nd.location, nd.right->location, Render::COLOR_YELLOW);
        }
}

void WorldCell::DrawPaths() {
    for (auto pt : paths)
        for (auto& nd : pt->nodes)
            nd.Render();
}


