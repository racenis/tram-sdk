// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_STATS_H
#define TRAM_SDK_FRAMEWORK_STATS_H

#include <framework/system.h>

#include <cstddef>

namespace tram::Stats {
    
enum Resource : uint32_t {
    RESOURCE_VRAM,
    RESOURCE_DRAWCALL
};

void Start(System::system_t system);
void Stop(System::system_t system);

void Add(Resource resource, size_t ammount);
void Remove(Resource resource, size_t ammount);

void Collate();

size_t GetStat(Resource resource);
double GetStat(System::system_t system);
double GetStatAverage(System::system_t system);
double GetStatUncollated(System::system_t system);

}

#endif // TRAM_SDK_FRAMEWORK_STATS_H