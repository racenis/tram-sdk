// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/system.h>
#include <cassert>
#include <cstring>

/**
 * @namespace tram::System
 * 
 * System tracking.
 * 
 * This system keeps track of other systems, their initialization status, as
 * well as their names.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/system.html
 */

/**
 * @typedef uint32_t system_t
 * System type index.
 * Can be set to any of the values in System::System, or you can use
 * System::Register() to allocate new system types.
 */

namespace tram::System {

struct SystemInfo {
    char const* name;
    char const* short_name;
    bool is_initialized;
    bool is_updated;
};

static SystemInfo system_infos[128] = {
    {"Invalid",                     "INV",      false, false},
    {"Core",                        "CORE",     false, false},
    {"Platform",                    "PLATFORM", false, false},
    {"User Interface",              "UI",       false, false},
    {"Graphical User Interface",    "GUI",      false, false},
    {"Async",                       "ASYNC",    false, false},
    {"Rendering",                   "RENDER",   false, false},
    {"Physics",                     "PHYSICS",  false, false},
    {"Audio",                       "AUDIO",    false, false},
    {"Misc",                        "MISC",     false, false},
};

static uint32_t last_system = SYSTEM_LAST; 

/// Registers a new system.
/// @param name Full name of the system.
/// @param short_name Name of the system that will be displayed in log messages.
uint32_t Register(char const* name, char const* short_name) {
    assert(last_system < 128);
    
    system_infos[last_system] = SystemInfo {
        .name = name,
        .short_name = short_name,
        .is_initialized = false,
        .is_updated = false
    };
    
    return last_system++;
}

/// Registers a registered system.
/// If the system was registered with the Register() function, its system_t
/// identificator will be returned. If a system with the name was not
/// registered, this function will return a SYSTEM_INVALID identificator.
system_t Find(char const* name) {
    for (system_t system = 0; system < last_system; system++) {
        if (strcmp(system_infos[system].name, name) == 0) {
            return system;
        }
    }
    
    return SYSTEM_INVALID;
}

/// Retrieves the full name of a system.
char const* GetName(uint32_t system) {
    assert(system < last_system);
    return system_infos[system].name;
}

/// Retrieves the short name of a system.
char const* GetShortName(uint32_t system) {
    assert(system < last_system);
    return system_infos[system].short_name;
}

/// Sets the initialization status of a system.
void SetInitialized(uint32_t system, bool is_initialized) {
    assert(system < last_system);
    assert(system != SYSTEM_INVALID);
    system_infos[system].is_initialized = is_initialized;
}

/// Checks if a system is initialized.
bool IsInitialized(uint32_t system) {
    if (system >= last_system) {
        return false;
    }
    
    return system_infos[system].is_initialized;
}

/// Sets the update status of a system.
void SetUpdated(uint32_t system, bool is_updated) {
    assert(system < last_system);
    assert(system != SYSTEM_INVALID);
    system_infos[system].is_updated = is_updated;
}

/// Checks if a system is updated.
bool IsUpdated(uint32_t system) {
    if (system >= last_system) {
        return false;
    }
    
    return system_infos[system].is_updated;
}

/// Returns the total count of registered systems.
system_t GetSystemCount() {
    return last_system;
}

}
