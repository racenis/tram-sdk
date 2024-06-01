// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/stats.h>
#include <framework/core.h>

#include <platform/time.h>

#include <vector>
#include <cassert>

namespace tram::Stats {

struct SystemStat {
    double time_started = -1.0f;        //< time when system was started, -1.0f if hasn't started
    double time_spent = 0.0f;           //< total time spent since last collation
    double time_spent_collated = 0.0f;  //< total time spent during previous period
    double time_spent_total = 0.0f;     //< total time spend during all periods
};

static std::vector<SystemStat> all_stats (100);
static std::vector<size_t> all_resources (100);

static uint32_t last_collate = -1;

/// Starts counting time spent on a system.
void Start(System::system_t system) {
    if (all_stats.size() <= system) {
        all_stats.resize(system + 1);
    }
    
    assert(all_stats[system].time_started == -1.0f && "System has already been started.");
    
    all_stats[system].time_started = GetTime();
}

/// Stops counting time spent on a system.
void Stop(System::system_t system) {
    assert(all_stats.size() > system && "System does not exist.");
    assert(all_stats[system].time_started != -1.0f && "System statistics was not started.");
    
    all_stats[system].time_spent += GetTime() - all_stats[system].time_started;
    all_stats[system].time_started = -1.0f;
}

/// Adds an ammount of a resource.
void Add(Resource resource, size_t ammount) {
    assert(all_resources.size() > resource && "Resource does not exist.");
    
    all_resources[resource] += ammount;
}

/// Removes an ammount of a resource.
void Remove(Resource resource, size_t ammount) {
    assert(all_resources.size() > resource && "Resource does not exist.");
    
    all_resources[resource] -= ammount;
}

/// Collates all of the time statistics.
void Collate() {
    assert(last_collate != GetTick() && "Don't call Collate() twice in an update cycle.");
    
    last_collate = GetTick();
    
    for (auto& stat : all_stats) {
        assert(stat.time_started == -1.0f);
        
        stat.time_spent_total += stat.time_spent_collated;
        stat.time_spent_collated = stat.time_spent;
        
        stat.time_spent = 0.0f;
    }
}

/// Returns the uncollated ammount of a resource.
size_t GetStat(Resource resource) {
    assert(all_resources.size() > resource);
    return all_resources[resource];
}

/// Returns the collated time of a system.
double GetStat(System::system_t system) {
    assert(all_stats.size() > system);
    return all_stats[system].time_spent_collated;
}

/// Returns the uncollated time of a system.
double GetStatUncollated(System::system_t system) {
    assert(all_stats.size() > system);
    return all_stats[system].time_spent;
}

}