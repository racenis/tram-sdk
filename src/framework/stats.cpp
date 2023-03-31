// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/stats.h>
#include <framework/core.h>

#include <platform/time.h>

#include <vector>

namespace tram::Stats {    
    struct SystemStat {
        double time_started = -1.0f;        //< time when system was started, -1.0f if hasn't started
        double time_spent = 0.0f;           //< total time spent since last collation
        double time_spent_collated = 0.0f;  //< total time spent during previous period
        double time_spent_total = 0.0f;     //< total time spend during all periods
    };
    
    std::vector<SystemStat> ALL_STATS;
    std::vector<size_t> ALL_RESOURCES;
    
    uint32_t LAST_COLLATE = -1;
    
    /// Starts counting time spent on a system.
    void Start (uint32_t system) {
        if (ALL_STATS.size() <= system) {
            ALL_STATS.resize(system + 1);
        }
        
        assert(ALL_STATS[system].time_started == -1.0f && "System has already been started.");
        
        ALL_STATS[system].time_started = GetTime();
    }
    
    /// Stops counting time spent on a system.
    void Stop (uint32_t system) {
        assert(ALL_STATS.size() > system &&  "System does not exist.");
        assert(ALL_STATS[system].time_started != -1.0f && "System was not started.");
        
        ALL_STATS[system].time_spent += GetTime() - ALL_STATS[system].time_started;
        ALL_STATS[system].time_started = -1.0f;
    }
    
    /// Adds an ammount of a resource.
    void Add (Resource resource, size_t ammount) {
        if (ALL_RESOURCES.size() <= resource) {
            ALL_RESOURCES.resize(resource + 1);
        }
        
        ALL_RESOURCES[resource] += ammount;
    }
    
    /// Removes an ammount of a resource.
    void Remove (Resource resource, size_t ammount) {
        assert(ALL_RESOURCES.size() > resource && "Resource does not exist.");
        
        ALL_RESOURCES[resource] -= ammount;
    }
    
    /// Collates all of the time statistics.
    void Collate () {
        assert(LAST_COLLATE != GetTick() && "Don't call Collate() twice in an update cycle.");
        
        LAST_COLLATE = GetTick();
        
        for (auto& stat : ALL_STATS) {
            assert(stat.time_started == -1.0f);
            
            stat.time_spent_total += stat.time_spent_collated;
            stat.time_spent_collated = stat.time_spent;
            
            stat.time_spent = 0.0f;
        }
    }
    
    /// Returns the collated time of a system.
    double GetStat (uint32_t system) {
        assert(ALL_STATS.size() > system);
        return ALL_STATS[system].time_spent_collated;
    }
    
    /// Returns the uncollated time of a system.
    double GetStatUncollated (uint32_t system) {
        assert(ALL_STATS.size() > system);
        return ALL_STATS[system].time_spent;
    }
    
    /// Returns the uncollated ammount of a resource.
    double GetStatUncollated (Resource resource) {
        assert(ALL_RESOURCES.size() > resource);
        return ALL_RESOURCES[resource];
    }
}