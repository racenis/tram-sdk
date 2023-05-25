// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/system.h>
#include <vector>

namespace tram::System {

struct SystemInfo {
    char const* name;
    char const* short_name;
    bool is_initialized;
    bool is_updated;
};

std::vector<SystemInfo> ALL_SYSTEMS;

/// Registers a new system.
/// @param name Full name of the system.
/// @param short_name Name of the system that will be displayed in log messages.
uint32_t Register (char const* name, char const* short_name) {
    static uint32_t last_issued_ID = 0;
    
    ALL_SYSTEMS.push_back(SystemInfo{
        .name = name,
        .short_name = short_name,
        .is_initialized = false
    });
    
    return last_issued_ID++;
}

/// Retrieves the full name of a system.
char const* GetName (uint32_t system) {
    return ALL_SYSTEMS[system].name;
}

/// Retrieves the short name of a system.
char const* GetShortName (uint32_t system) {
    return ALL_SYSTEMS[system].short_name;
}

/// Sets the initialization status of a system.
void SetInitialized (uint32_t system, bool is_initialized) {
    ALL_SYSTEMS[system].is_initialized = is_initialized;
}

/// Checks if a system is initialized.
bool IsInitialized (uint32_t system) {
    return ALL_SYSTEMS[system].is_initialized;
}

/// Sets the update status of a system.
void SetUpdated (uint32_t system, bool is_updated) {
    ALL_SYSTEMS[system].is_updated = is_updated;
}

/// Checks if a system is updated.
bool IsUpdated (uint32_t system) {
    return ALL_SYSTEMS[system].is_updated;
}

/// Returns the total count of registered systems.
system_t GetSystemCount () {
    return ALL_SYSTEMS.size();
}

}
