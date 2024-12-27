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
    if (last_system >= SYSTEM_LIMIT) {
        Log("System {} could not be registered, SYSTEM_LIMIT exceeded!", name);
        return System::INVALID;
    }

    // TODO: check if system with name doesn't already exist
    // also short name

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
    assert(system != System::INVALID);
    system_infos[system].state = READY;
}

/// Checks if a system is initialized.
bool IsInitialized(uint32_t system) {
    if (system >= last_system) {
        return false;
    }
    
    return system_infos[system].state == READY;
}

void SetState(system_t system, SystemState state) {
    if (system >= last_system) {
        Log(Severity::ERROR, System::CORE, "Attempting to set the state of an invalid system_t '{}'", system);
        return;
    }
    
    system_infos[system].state = state;
}

void AssertDependency(system_t system) {
    if (system >= last_system) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Attempting assert dependency on invalid system_t '{}'", system);
        return;
    }
    
    // check if system is fully initialized
    if (system_infos[system].state == READY) return;
    
    // otherwise proceed to crash
    Log(Severity::CRITICAL_ERROR, System::CORE, "Dependency on system {} assertion failed", system);
    
    abort();
    
    // 3 possible messages:
    // - assert while no system is initing
    // - assert while a single system is initing
    // - assert when multiple are initing
    
    // TODO: finish
}

/// Returns the total count of registered systems.
system_t GetSystemCount() {
    return last_system;
}

}
