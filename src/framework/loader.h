// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_LOADER_H
#define TRAM_SDK_FRAMEWORK_LOADER_H

namespace tram {
    class Entity;
}

namespace tram::Loader {

void Track(Entity* entity);
void Untrack(Entity* entity);

void Update();

}

#endif // TRAM_SDK_FRAMEWORK_LOADER_H