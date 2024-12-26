// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/stats.h>
#include <framework/core.h>

#include <platform/api.h>

#include <vector>
#include <cassert>

/**
 * @namespace tram::Stats
 * 
 * Resource usage statistics.
 * 
 * Keeps track of resource usage, time spent by different systems, as well as
 * various other counters.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/stats.html
 */

namespace tram::Stats {

struct SystemStat {
    double time_started = -1.0f;        //< time when system was started, -1.0f if hasn't started
    double time_spent = 0.0f;           //< total time spent since last collation
    double time_spent_collated = 0.0f;  //< total time spent during previous period
    double time_spent_total = 0.0f;     //< total time spend during all periods
    double time_spent_average = 0.0f;
    double time_spent_average_collated = 0.0f;
    int average_count = 0;
};

static std::vector<SystemStat> all_stats(100);
static std::vector<size_t> all_resources(100);

static uint32_t last_collate_frame = -1;
static uint32_t last_collate_time = -1;

/// Starts counting time spent on a system.
void Start(System::system_t system) {
    if (all_stats.size() <= system) {
        all_stats.resize(system + 1);
    }
    
    assert(all_stats[system].time_started == -1.0f && "System has already been started.");
    
    all_stats[system].time_started = Platform::Window::GetTime();
}

/// Stops counting time spent on a system.
void Stop(System::system_t system) {
    assert(all_stats.size() > system && "System does not exist.");
    assert(all_stats[system].time_started != -1.0f && "System statistics was not started.");
    
    all_stats[system].time_spent += Platform::Window::GetTime() - all_stats[system].time_started;
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
    assert(last_collate_frame != GetTick() && "Don't call Collate() twice in an update cycle.");
    
    if (last_collate_time != (uint32_t)Platform::Window::GetTime()) {
        last_collate_time = Platform::Window::GetTime();
        
        for (auto& stat : all_stats) {
            stat.time_spent_average_collated = stat.time_spent_average / (double)stat.average_count;
            stat.time_spent_average = 0.0f;
            stat.average_count = 0;
        }
    }
    
    last_collate_frame = GetTick();
    
    for (auto& stat : all_stats) {
        assert(stat.time_started == -1.0f);
        
        stat.time_spent_total += stat.time_spent_collated;
        stat.time_spent_collated = stat.time_spent;
        
        stat.time_spent = 0.0f;
        
        stat.time_spent_average += stat.time_spent_collated;
        stat.average_count++;
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

/// Returns the ammount of resource, averaged over the last second.
double GetStatAverage(System::system_t system) {
    assert(all_stats.size() > system);
    return all_stats[system].time_spent_average_collated;
}

}