// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_CORE_H
#define FRAMEWORK_CORE_H

#include <cstdint>

namespace Core {
    typedef uint64_t id_t;
    
    extern bool EXIT;
    extern float TIME;
    extern uint32_t TICK;
        
    extern char const* ENGINE_VERSION;
    
    
    void Init();
    
    id_t GenerateID();
}

#endif // FRAMEWORK_CORE_H
