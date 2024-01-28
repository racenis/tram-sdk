// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/worldcell.h>

#include <framework/entity.h>
#include <framework/navigation.h>
#include <framework/transition.h>
#include <framework/file.h>

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
    
    if (!entities.size()) {
        std::cout << "Cell " << name << " has no enitites. Forgot to load from disk?" << std::endl;
    }
    
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
}

void WorldCell::LoadFromDisk() {
    char path[100] = "data/worldcells/";
    strcat(path, name);
    strcat(path, ".cell");
    
    File file (path, MODE_READ | MODE_PAUSE_LINE);

    if (!file.is_open()) {
        std::cout << "Worldcell file " << path << " not found!" << std::endl;
        return;
    }

    if (file.read_name() != "CELLv2") {
        std::cout << "Cell format unrecognized " << path << "!" << std::endl;
        return;
    }
    
    file.read_name(); // skip over cell name in file
    
    this->interior = file.read_uint32();
    this->interior_lighting = file.read_uint32();
    
    file.skip_linebreak();
    
    while (file.is_continue()) {
        name_t entry_type = file.read_name();
        
        if (entry_type == "transition") {
            name_t transition_name = file.read_name();
            name_t transition_into = file.read_name();
            
            std::cout << "found transition " << transition_name << " into " << transition_into << std::endl;
            
            WorldCell* into_ptr = WorldCell::Find(transition_into);
            
            if (!into_ptr) {
                std::cout << "Transition into cell '" << transition_into << "' defined in " << path << ", but can't find said cell." << std::endl;
            }
            
            Transition* transition = Transition::Make(transition_name, this, into_ptr);
            
            size_t point_count = file.read_int32();

            for (size_t i = 0; i < point_count; i++) {
                transition->AddPoint({
                    file.read_float32(),
                    file.read_float32(),
                    file.read_float32()
                });
            }
            
            transition->GeneratePlanes();
            
            file.skip_linebreak();
            continue;
        }
        
        if (entry_type == "signal") {
            Entity* owner = Entity::Find(file.read_uint64());
            
            if (!owner) {
                std::cout << "Singal coudnlt find! find! signal ID for entity! " << std::endl;
                file.skip_linebreak();
                continue;
            }
            
            Signal signal;
            
            signal.type = Signal::GetType(file.read_name());
            signal.receiver = file.read_name();
            signal.delay = file.read_float32();
            signal.limit = file.read_int32();
            signal.message_type = Message::GetType(file.read_name());
            
            name_t data_type = file.read_name();
            if (!data_type) {
                signal.data = nullptr;
            } else {
                Value* val = new Value(); // this might cause a memory leak, but we don't care
                
                if (data_type == "int") {
                    *val = file.read_int32();
                } else if (data_type == "float") {
                    *val = file.read_float32();
                } else if (data_type == "name") {
                    *val = file.read_name();
                } else {
                    *val = file.read_name();
                    std::cout << "Unknown data type for signal: " << data_type << std::endl;
                }
                
                signal.data = val;
            }
            
            if (!owner->signals) {
                owner->signals = new SignalTable;
            }
            
            owner->signals->Add(signal);
            
            file.skip_linebreak();
            continue;
        }
        
        
        if (entry_type == "path" || entry_type == "navmesh" || entry_type == "group") {
            std::cout << "skipping " << entry_type << ": " << file.read_line() << std::endl;
            file.skip_linebreak();
            continue;
        }

        Entity* entity = Entity::Make(entry_type, &file);
        
        if (!entity) {
            std::cout << "Entity type '" << entry_type << "' not recognized; in file " << path << std::endl;
        } else {
            this->AddEntity(entity);
        }
        
        file.skip_linebreak();
    }
}

}