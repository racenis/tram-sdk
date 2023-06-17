// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_LOADER_H
#define TRAM_SDK_FRAMEWORK_LOADER_H

#include <framework/worldcell.h>

namespace tram {

class Loader {
public:
    void SetLocation(vec3 location);
    void UpdateLocation(vec3 location);
    static void Update();
private:
    vec3 location = {0.0f, 0.0f, 0.0f};
    WorldCell* current_cell = nullptr;
};

}

#endif // TRAM_SDK_FRAMEWORK_LOADER_H