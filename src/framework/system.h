// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_SYSTEM_H
#define FRAMEWORK_SYSTEM_H

#include <cstdint>

namespace Core::System {
    enum System : uint32_t {
        SYSTEM_CORE,
        SYSTEM_UI,
        SYSTEM_ASYNC,
        SYSTEM_RENDER,
        SYSTEM_PHYSICS,
        SYSTEM_AUDIO,
        SYSTEM_MISC,
        SYSTEM_LAST
    };
    
    uint32_t Register (char const* name, char const* short_name);
    void SetInitialized (uint32_t system, bool is_initialized);
    bool IsInitialized (uint32_t system);
    char const* GetName (uint32_t system);
    char const* GetShortName (uint32_t system);
}

#endif // FRAMEWORK_SYSTEM_H