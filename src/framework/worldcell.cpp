// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/worldcell.h>

#include <framework/entity.h>
#include <framework/transition.h>
#include <framework/file.h>
#include <framework/logging.h>

#include <templates/pool.h>
#include <templates/hashmap.h>

#include <render/render.h>

#include <config.h>

#include <algorithm>

#include <fstream>
#include <cstring>
#include <sstream>

/**
 * @struct tram::WorldCell framework/worldcell.h <framework/worldcell.h>
 * 
 * World streaming unit.
 * 
 * Contains entities, can be used to load and unload them.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/worldcell.html
 */

namespace tram {

template <> Pool<WorldCell> PoolProxy<WorldCell>::pool("Worldcell pool", WORLDCELL_LIMIT);

static Hashmap<WorldCell*> worldcell_list("Worldcell list hashmap", WORLDCELL_LIMIT);

/// Finds a WorldCell by its name.
/// @return Pointer to the cell if found, nullptr otherwise.
WorldCell* WorldCell::Find(name_t name) {
    return worldcell_list.Find(name);
}

/// Creates a WorldCell by name.
/// If a cell with the given name already exists, this method will return that
/// same cell, otherwise a new cell will be created.
/// @return Always returns the pointer to the created WorldCell.
WorldCell* WorldCell::Make(name_t name) {
    auto cell = worldcell_list.Find(name);
    
    if (!cell) {
        cell = PoolProxy<WorldCell>::New(name);
        worldcell_list.Insert(name, cell);
    }
    
    return cell;
}

/// Finds the WorldCell which contains a given point.
/// This method will iterate through WorldCells and will try to find a WorldCell
/// with a volume that contains the given point.
/// It will first try finding a volume for an interior cell, otherwise it will
/// try finding a volume for an exterior cell. If it can't find a cell, it will
/// return a nullptr.
/// @return Pointer to a WorldCell or a nullptr.
WorldCell* WorldCell::Find(vec3 point) {
    for (auto& cell : PoolProxy<WorldCell>::GetPool()) {
        if (!cell.IsInterior()) continue;
        if (cell.IsInside(point)) return &cell;
    }
    
    for (auto& cell : PoolProxy<WorldCell>::GetPool()) {
        if (cell.IsInside(point)) return &cell;
    }
    
    return nullptr;
}

/// Loads the cell.
/// This will flag the cell as loaded and will load all of the entities that
/// have been flagged as being automatically loaded.
void WorldCell::Load() {
    Log(Severity::INFO, System::CORE, "Loading cell:", name);
    
    if (!entities.size() && !(flags & LOADED_FROM_DISK)) {
        Log(Severity::ERROR, System::CORE, "Cell '{}' has no entities. Forgot to load from disk?", name);
    }
    
    for (auto it : entities) {
        if (it->IsAutoLoad() && !it->IsDeleted()) it->Load();
    }
        
    SetFlag(LOADED, true);
};

/// Unloads the cell.
/// This will flag the cell as unloaded and will unload all of the entities that
/// have been flagged as being automatically loaded. It will also delete
/// entities that have been flagged as being non-persistent.
void WorldCell::Unload() {
    Log(Severity::INFO, System::CORE, "Unloading cell: {}", name);
    
    auto entities_copy = entities;
    for (auto& it : entities_copy) {
        if (!it->IsAutoLoad() || !it->IsLoaded()) continue;
        
        if (it->IsPersistent()) {
            it->Unload();
        } else {
            Log(Severity::INFO, System::CORE, "Yeeting {} out of existence!", it->GetName());
            it->Yeet();
        }
    }

   SetFlag(LOADED, false);
};

/// Adds a transition.
/// Adds an outgoing transition to the worldcell.
void WorldCell::Add(Transition* transition) {
    if (transition->cell_into == this) {
        volume.push_back(transition);
    } else {
        transitions.push_back(transition);
    }
}

/// Finds a transition from a position.
/// This is meant to be used by entities that are seeking a WorldCell into which
/// they can transition into.
WorldCell* WorldCell::FindTransition(vec3 point) {
    // find if transition is possible
    WorldCell* next = nullptr;
    for (auto transition : transitions) {
        if (transition->IsInside(point)) {
            next = transition->GetCell();
            break;
        }
    }
    
    // if not, then return
    if (!next) return nullptr;
    
    // if it is, then check if we still are in parent cell
    bool in_parent = false;
    for (auto transition : volume) {
        if (transition->IsInside(point)) {
            in_parent = true;
            break;
        }
    }
    
    // if we are not, then transition
    if (!in_parent) return next;
    
    // if we are, then check if parent is exterior and next is interior
    bool this_exterior = !(this->flags & INTERIOR);
    bool next_interior = next->flags & INTERIOR;
    
    // if it is, then transition
    if (this_exterior && next_interior) return next;
    
    return nullptr;
}

/// Checks if point is inside the worldcell.
bool WorldCell::IsInside(vec3 point) {
    for (auto transition : volume) {
        if (transition->IsInside(point)) return true;
    }
    
    return false;
}

/// Links a worldcell.
/// This method will take the volume transitions assigned to the worldcell in
/// other pointer parameter and assign them as transitions to the given cell.
void WorldCell::Link(WorldCell* other) {
    if (!other) {
        Log(Severity::WARNING, System::CORE, "Can't link '{}' cell to a nullptr", name);
        return;
    }
    
    for (auto transition : other->GetVolume()) {
        this->transitions.push_back(transition);
    }
}

/// Adds an entity to the worldcell.
void WorldCell::Add(Entity* entity) {
    if (!entity) {
        Log(Severity::WARNING, System::CORE, "Can't put a nullptr entity in '{}' cell", name);
        return;
    }

    /* the reason why we check for if the entity is nullptr is because sometimes
     * the user might do something like cell->Add(Entity::Find("non-existing"));
     * and if an entity can't be found, the Entity::Find() function returns a
     * nullptr, so it is useful to check for that.
     */

    if (entity->cell == this) {
        Log(Severity::WARNING, System::CORE, "Can't put entity '{}' into '{}' cell, it is already in it", name, entity->name);
        return;
    }
    
    if (entity->GetCell()) {
        entity->GetCell()->Remove(entity);
    }
    
    entities.push_back(entity);
    
    entity->cell = this;

    if (flags & LOADED && !entity->IsLoaded() && entity->IsAutoLoad()) {
        entity->Load();
    }

    if (!(flags & LOADED) && entity->IsLoaded() && entity->IsAutoLoad()) {
        entity->Unload();
    }
}

/// Removes an entity from the worldcell.
void WorldCell::Remove(Entity* entity) {
    if (!entity) {
        Log(Severity::WARNING, System::CORE, "Can't remove a nullptr entity from '{}' cell", name);
    }
    
    if (entity->cell == this) {
        entity->cell = nullptr;
    }
    
    auto ptr = std::find(entities.begin(), entities.end(), entity);
    if (ptr != entities.end()) {
        entities.erase(ptr);
    } else {
        if (entity->name) {
            Log(Severity::WARNING, System::CORE, "Can't remove '{}' from '{}' cell, it is not in it", entity->name, name);
        } else {
            Log(Severity::WARNING, System::CORE, "Can't remove {} from '{}' cell, it is not in it", entity->id, name);
        }
    }
}

/// Loads worldcell data from disk.
void WorldCell::LoadFromDisk() {
    char path[PATH_LIMIT] = "data/worldcells/";
    strcat(path, name);
    strcat(path, ".cell");
    
    File file(path, File::READ | File::PAUSE_LINE);

    if (!file.is_open()) {
        Log(Severity::ERROR, System::CORE, "Worldcell file{} not found!", path);
        return;
    }

    if (file.read_name() != "CELLv3") {
        Log(Severity::ERROR, System::CORE, "Cell format unrecognized in {}!", path);
        return;
    }
    
    
    SetFlag(LOADED_FROM_DISK, true);
    
    file.read_name(); // skip over cell name in file
    
    // wait why does the cell need a name inside the file
    // the file already has a name
    // also what happens when it doens't match???
    // TODO: fix
    
    SetFlag(INTERIOR, file.read_uint32());
    SetFlag(INTERIOR_LIGHTING, file.read_uint32());
    
    file.skip_linebreak();
    
    while (file.is_continue()) {
        name_t entry_type = file.read_name();
        
        if (entry_type == "transition") {
            name_t transition_name = file.read_name();
            name_t transition_into = file.read_name();
            
            WorldCell* into_ptr = WorldCell::Find(transition_into);
            
            if (!into_ptr) {
                Log(Severity::WARNING, System::CORE, "Transition into cell '{}' defined in {}, but can't find said cell", transition_into, path);
            }
            
            Transition* transition = Transition::Make(transition_name, into_ptr);
            
            this->Add(transition);
            
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
            id_t owner_id = file.read_uint32();
            Entity* owner = Entity::Find(owner_id);
            
            if (!owner) {
                Log(Severity::WARNING, System::CORE, "Cell '{}' defines a signal for entity {} but can't find it", name, owner_id);
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
                } else if (data_type == "vec3") {
                    *val = vec3 {file.read_float32(), file.read_float32(), file.read_float32()};
                }  else {
                    *val = file.read_name();
                    Log(Severity::WARNING, System::CORE, "Unknown data type for signal: ", data_type);
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
        
        
        if (entry_type == "path" || entry_type == "navmesh" || entry_type == "group" || entry_type == "version") {
            file.skip_linebreak();
            continue;
        }

        Entity* entity = Entity::Make(entry_type, &file);
        
        if (!entity) {
            Log(Severity::WARNING, System::CORE, "Entity type '{}' not recognized; in file {}", entry_type, path);
        } else {
            this->Add(entity);
        }
        
        file.skip_linebreak();
    }
}

}