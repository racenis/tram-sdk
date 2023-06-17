// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <framework/event.h>
#include <framework/system.h>
#include <framework/uid.h>

#include <platform/time.h>

#include <iostream>
#include <cassert>

namespace tram {
    
/// Engine version string.
/// If you fork the engine, you should probably change this.
char const* ENGINE_VERSION = "Tramway SDK v0.0.7";

/// If set to true, then the main loop should stop and animation should exit.
bool EXIT = false;

static float frame_time = 0.0f;
static uint32_t frame_tick = 0;

/// Generates a unique ID number.
id_t GenerateID() {
    static id_t num = 0;
    num++;
    return num;
}

/// Initializes the core of the framework.
void Core::Init() {
    // set the 0th string to 'none'
    UID none("none");
    
    // this will register all of the default events
    for (event_t i = 0; i < Event::LAST_EVENT; i++) {
        Event::Register();
    }
    
    assert(!System::IsInitialized(System::SYSTEM_CORE));
    
    System::SetInitialized(System::SYSTEM_CORE, true);
    
    std::cout << ENGINE_VERSION << std::endl;
}

/// Updates the core system.
/// @note This should be called only once per update cycle.
void Core::Update() {
    frame_time = GetTime();
    frame_tick++;
    
    for (System::system_t i = 0; i < System::GetSystemCount(); i++) {
        System::SetUpdated(i, false);
    }
}

/// Returns the current tick.
/// This value gets incremented once per update cycle.
/// @return Count of update cycles since the application was started.
uint32_t GetTick() {
    return frame_tick;
}

/// Returns the time at the start of the current tick.
/// This value changes only once per update cycle.
/// @return Time, in seconds, since the application was started.
float GetTickTime() {
    return frame_time;
}

}


