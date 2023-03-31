// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_LOADER_H
#define TRAM_SDK_FRAMEWORK_LOADER_H

#include <framework/worldcell.h>

namespace tram {

class Loader {
public:
    void SetLocation (const vec3& new_location) { location = new_location; current_cell = WorldCell::Find(new_location); }
    void UpdateLocation (const vec3& new_location) { location = new_location; auto n_trans = current_cell->FindTransition(location); if (n_trans) { current_cell = n_trans; } }
    static void Update();
private:
    vec3 location = {0.0f, 0.0f, 0.0f};
    WorldCell* current_cell = nullptr;
};

}

#endif // TRAM_SDK_FRAMEWORK_LOADER_H