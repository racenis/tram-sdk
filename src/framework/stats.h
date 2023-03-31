// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_STATS_H
#define TRAM_SDK_FRAMEWORK_STATS_H

#include <framework/math.h>

namespace tram::Stats {
    enum Resource : uint32_t {
        RESOURCE_VRAM
    };
    
    void Start (uint32_t system);
    void Stop (uint32_t system);
    
    void Add (Resource resource, size_t ammount);
    void Remove (Resource resource, size_t ammount);
    
    void Collate ();
    
    double GetStat (uint32_t system);
    double GetStatUncollated (uint32_t system);
    double GetStatUncollated (Resource resource);
}

#endif // TRAM_SDK_FRAMEWORK_STATS_H