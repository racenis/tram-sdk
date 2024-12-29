// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/system.h>

#include <framework/logging.h>
#include <config.h>

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
    SystemState state;
};

static SystemInfo system_infos[SYSTEM_LIMIT] = {
    {"Invalid",                     "INV",      YEET},
    {"Core",                        "CORE",     YEET},
    {"Platform",                    "PLATFORM", YEET},
    {"User Interface",              "UI",       YEET},
    {"Graphical User Interface",    "GUI",      YEET},
    {"Async",                       "ASYNC",    YEET},
    {"Rendering",                   "RENDER",   YEET},
    {"Physics",                     "PHYSICS",  YEET},
    {"Audio",                       "AUDIO",    YEET},
    {"Misc",                        "MISC",     YEET},
};

static uint32_t last_system = SYSTEM_LAST; 

/// Registers a new system.
/// @param name Full name of the system.
/// @param short_name Name of the system that will be displayed in log messages.
uint32_t Register(char const* name, char const* short_name) {
    if (UID::is_empty(name)) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "System name '{}' is empty", name);
    }
    
    if (UID::is_empty(short_name)) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "System short name '{}' is empty", short_name);
    }
    
    if (!UID::no_quote(short_name)) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "System short name '{}' contains invalid characters", short_name);
    }
    
    for (system_t i = 0; i < last_system; i++) {
        if (strcmp(system_infos[i].name, name) != 0) continue;

        Log(Severity::CRITICAL_ERROR, System::CORE, "System name '{}' already in use", name);
    }
    
    for (system_t i = 0; i < last_system; i++) {
        if (strcmp(system_infos[i].short_name, short_name) != 0) continue;

        Log(Severity::CRITICAL_ERROR, System::CORE, "System name '{}' already in use", short_name);
    }
    
    if (last_system >= SYSTEM_LIMIT) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "System {} could not be registered, SYSTEM_LIMIT exceeded!", name);
        return System::INVALID;
    }

    system_infos[last_system] = SystemInfo {
        .name = name,
        .short_name = short_name,
        .state = YEET
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
    
    return System::INVALID;
}

/// Retrieves the full name of a system.
char const* GetName(uint32_t system) {
    if (system >= last_system) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Accessing invalid system_t {}", system);
    }
    return system_infos[system].name;
}

/// Retrieves the short name of a system.
char const* GetShortName(uint32_t system) {
    if (system >= last_system) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Accessing invalid system_t {}", system);
    }
    return system_infos[system].short_name;
}

/// Sets the initialization status of a system.
void SetInitialized(uint32_t system, bool is_initialized) {
    if (system == System::INVALID || system >= last_system) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Accessing invalid system_t {}", system);
    }
    system_infos[system].state = READY;
}

/// Checks if a system is initialized.
bool IsInitialized(uint32_t system) {
    if (system >= last_system) {
        return false;
    }
    
    return system_infos[system].state == READY;
}

/// Sets the state of a system.
/// - `YEET` when a system is uninitialized.
/// - `INIT` when a system is initializing.
/// - `READY` when a system is initialized.
void SetState(system_t system, SystemState state) {
    if (system == System::INVALID || system >= last_system) {
        Log(Severity::ERROR, System::CORE, "Attempting to set the state of an invalid system_t '{}'", system);
        return;
    }
    
    system_infos[system].state = state;
}

void AssertDependency(system_t system) {
    if (system == System::INVALID || system >= last_system) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Attempting assert dependency on invalid system_t '{}'", system);
        return;
    }
    
    // check if system is fully initialized
    if (system_infos[system].state == READY) return;
    
    // otherwise check which system is peforming initialization
    int systems_loading = 0;
    system_t system_loading = System::INVALID;
    for (system_t i = 0; i < last_system; i++) {
        if (system_infos[i].state == INIT) systems_loading++, system_loading = i;
    }
    
    // print out an error message
    if (!systems_loading) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Dependency on system {} assertion failed", GetName(system));
    } else if (systems_loading == 1) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Dependency on system {} assertion failed while loading", GetName(system), GetName(system_loading));
    } else {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Dependency on system {} assertion failed while loading multiple systems", GetName(system));
    }
    
    // and proceed to crash
    abort();
}

/// Returns the total count of registered systems.
system_t GetSystemCount() {
    return last_system;
}

}
