// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_CORE_H
#define FRAMEWORK_CORE_H

#include <cstdint>

namespace Core {
    typedef uint64_t id_t;
    
    extern bool EXIT;
    extern char const* ENGINE_VERSION;
    
    void Init();
    void Update();
    
    id_t GenerateID();
    
    uint32_t GetTick();
    float GetTickTime();
}

#endif // FRAMEWORK_CORE_H
