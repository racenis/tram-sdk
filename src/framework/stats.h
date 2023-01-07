// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_STATS_H
#define FRAMEWORK_STATS_H

#include <cstdint>

namespace Core::Stats {
    enum Resource : uint32_t {
        RESOURCE_VRAM
    };
    
    void Start (uint32_t system);
    void Stop (uint32_t system);
    
    void Add (Resource resource, size_t ammount);
    void Remove (Resource resource, size_t ammount);
    
    void Collate ();
    float GetStat (uint32_t system);
    float GetStat (Resource resource);
}

#endif // FRAMEWORK_STATS_H