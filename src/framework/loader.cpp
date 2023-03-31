// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/loader.h>

#include <framework/transition.h>
#include <templates/pool.h>

#include <set>

namespace tram {

template <> Pool<Loader> PoolProxy<Loader>::pool("worldcellloader pool", 10, false);

void Loader::Update() {
    std::set<WorldCell*> active_cells;
    auto& loader_pool = PoolProxy<Loader>::GetPool();
    auto& cell_pool = PoolProxy<WorldCell>::GetPool();
    
    for (auto& loader : loader_pool) {
        if (loader.current_cell) {
            active_cells.insert(loader.current_cell);
            for (auto trans : loader.current_cell->transitions_from) {
                active_cells.insert(trans->GetInto());
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
    
}