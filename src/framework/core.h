// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_CORE_H
#define TRAM_SDK_FRAMEWORK_CORE_H

#include <cstdint>

namespace tram {

typedef uint32_t id_t;

extern bool EXIT;
extern char const* ENGINE_VERSION;

id_t GenerateID();

uint32_t GetTick();
float GetTickTime();

namespace Core {
    void Init();
    void Update();
}

}

#endif // TRAM_SDK_FRAMEWORK_CORE_H
