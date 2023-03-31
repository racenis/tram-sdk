// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/worldcell.h>

#include <framework/entity.h>
#include <framework/navigation.h>
#include <framework/transition.h>

#include <templates/pool.h>
#include <templates/hashmap.h>

#include <render/render.h>

#include <fstream>
#include <cstring>
#include <sstream>



namespace tram {

template <> Pool<WorldCell> PoolProxy<WorldCell>::pool("worldcell pool", 250, false);

static Hashmap<WorldCell*> worldcell_list ("Worldcell list hashmap", 500);


WorldCell* WorldCell::Find (name_t name) {
    return worldcell_list.Find(name);
}

WorldCell* WorldCell::Make (name_t name) {
    auto cell = worldcell_list.Find(name);
    
    if (!cell) {
        cell = PoolProxy<WorldCell>::New(name);
        worldcell_list.Insert(name, cell);
    }
    
    return cell;
}

WorldCell* WorldCell::Find (vec3 point) {
    for (auto& cell : PoolProxy<WorldCell>::GetPool()) {
        if (!cell.IsInterior()) continue;
        if (cell.IsInside(point)) return &cell;
    }
    
    for (auto& cell : PoolProxy<WorldCell>::GetPool()) {
        if (cell.IsInside(point)) return &cell;
    }
    
    return nullptr;
}

void WorldCell::Load() {
    std::cout << "Loading cell: " << name << std::endl;
    for (auto it : entities) {
        if (it->IsAutoLoad()) it->Load();
    }
        
    loaded = true;
};

void WorldCell::Unload() {
    std::cout << "Unloading cell: " << name << std::endl;
    auto entities_copy = entities;
    for (auto& it : entities_copy) {
        if (!it->IsAutoLoad() || !it->IsLoaded()) continue;
        
        if (it->IsPersistent()) {
            it->Unload();
        } else {
            std::cout << "Yeeting " << it->GetName() << " out of existance!" << std::endl;
            delete it;
        }
    }

    loaded = false;
};

WorldCell* WorldCell::FindTransition (vec3 point) {
    for (auto it : transitions_from) {
        if (it->IsInside(point)) return it->GetInto();
    }
    
    return nullptr;
}

bool WorldCell::IsInside (vec3 point) {
    for (auto it : transitions_into) {
        if (it->IsInside(point)) return true;
    }
    
    return false;
}



void WorldCell::LoadFromDisk() {
    char path[100] = "data/worldcells/";
    strcat(path, name);
    strcat(path, ".cell");
    std::ifstream file;
    file.open(path);

    if (!file){ std::cout << "Can't find worldcell: " << path << std::endl; return; }
    std::cout << "Loading worldcell: " << name << std::endl;

    std::string line;

    file >> line;
    
    if (line != "CELLv2") {
        std::cout << "Cell file format error: " << path << std::endl;
        return;
    }
    
    file >> line; // cell name
    file >> line; // cell interior flag
    file >> line; // cell lighting flag

    std::getline(file, line); // skip the rest of the line

    while (std::getline(file, line)){
        size_t firstchar = line.find_first_not_of(" \t");
        if (firstchar != std::string::npos && line[firstchar] == '#') continue;
        //std::cout << "line : " << line << std::endl;
        
        size_t first_name_end = line.find_first_of(" \t", firstchar);
        std::string type_name = line.substr(firstchar, first_name_end);
        
        if (type_name == "transition") {
            // TODO: store the transition name in a lookupable map
            std::stringstream ss (line.substr(first_name_end));
            std::string name;
            ss >> name;
            name_t tran_name = UID(name);
            ss >> name;
            name_t into_cell_name = UID(name);
            auto trans = Transition::Make(tran_name, WorldCell::Find(this->name), WorldCell::Find(into_cell_name));
            size_t pointcount;
            ss >> pointcount;
            for (size_t i = 0; i < pointcount; i++) {
                glm::vec3 point;
                ss >> point.x;
                ss >> point.y;
                ss >> point.z;
                trans->AddPoint(point);
            }
            
            trans->GeneratePlanes(false);
            
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

void WorldCell::AddEntity (Entity* entity) {
    entities.push_back(entity);
    
    entity->cell = this;

    if(loaded && !entity->is_loaded && entity->auto_load){
        entity->Load();
    }

    if (!loaded && entity->is_loaded && entity->auto_load){
        entity->Unload();
    }
}

void WorldCell::RemoveEntity (Entity* entity) {
    entities.erase(std::find(entities.begin(), entities.end(), entity));
};

void WorldCell::DrawTransitions(){
    for (auto it : transitions_into) it->GeneratePlanes(true);
    for (auto it : transitions_from) it->GeneratePlanes(true);
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

}