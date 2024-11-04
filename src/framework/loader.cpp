// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/loader.h>

#include <framework/entity.h>
#include <framework/worldcell.h>

#include <framework/transition.h>
#include <templates/pool.h>

#include <set>

namespace tram::Loader {

std::set<id_t> tracked_entities;

void Track(Entity* entity) {
    tracked_entities.insert(entity->GetID());
}

void Untrack(Entity* entity) {
    tracked_entities.erase(entity->GetID());
}

void Update() {
    if (!tracked_entities.size()) return;
    
    auto safe_copy = tracked_entities;
    
    std::set<WorldCell*> active_cells;
    
    // find all cells with tracked entities and remove lost entities
    for (id_t entity_id : safe_copy) {
        Entity* entity = Entity::Find(entity_id);
        
        if (!entity) {
            std::cout << "Loader lost track of entity with id " << entity_id << std::endl;
            tracked_entities.erase(entity_id);
            continue;
        }
        
        if (entity->GetCell()) {
            active_cells.insert(entity->GetCell());
        } else if (WorldCell* cell = WorldCell::Find(entity->GetLocation()); cell) {
            active_cells.insert(cell);
        }
    }
    
    // find all cells that need to be loaded
    std::set<WorldCell*> marked_cells;
    for (WorldCell* cell : active_cells) {
        
        // mark all cells with tracked entities
        marked_cells.insert(cell);
        
        // mark all cells that are directly connected
        for (Transition* transition : cell->GetTransitions()) {
            marked_cells.insert(transition->GetCell());
        }
    }
    
    // load and unload cells as marked
    for (auto& cell : PoolProxy<WorldCell>::GetPool()) {
        if (!cell.HasAutomaticLoading()) continue;
        
        if (cell.IsLoaded() && !marked_cells.contains(&cell)) {
            cell.Unload();
        }
        
        if (!cell.IsLoaded() && marked_cells.contains(&cell)) {
            cell.Load();
        }
    }
}
    
}