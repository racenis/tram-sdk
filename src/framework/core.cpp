// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/core.h>
#include <framework/event.h>
#include <framework/system.h>
#include <framework/uid.h>
#include <framework/logging.h>

#include <platform/api.h>

#include <config.h>

#include <iostream>
#include <cassert>

/**
 * @file framework/core.cpp
 * 
 * Core functionality of the framework.
 */

namespace tram {

static uint32_t ticks = 0;
static uint32_t frames = 0;

static uint32_t delta_tick = 0;
static float delta_time = 0.0f;

static double frame_time = 0.0f;

static float time_since_tick = 0.0f;

/// Generates a unique ID number.
id_t GenerateID() {
    static id_t num = 0;
    num++;
    return num;
}

/// Initializes the core of the framework.
void Core::Init() {
    if (System::IsInitialized(System::CORE)) {
       Log(Severity::CRITICAL_ERROR, System::CORE, "Can not initialize Core, it is already initialized"); 
    }
    
    // this will print out the version of the runtime to the console. I think
    // that doing so looks very nice. maybe you disagree. if you disagree, you
    // can comment out the following line
    Log(GetVersion()); 
    
    System::SetState(System::CORE, System::READY);
}

/// Updates the core system.
/// @note This should be called only once per update cycle.
void Core::Update() {
    const double last_frame_time = frame_time;
    frame_time = Platform::Window::GetTime();
    delta_time = frame_time - last_frame_time;
    
    frames++;
    
    Event::Post({
        .type = Event::FRAME,
        .poster = 0
    });
    
    time_since_tick += delta_time;
    
    // avoid overflowing event buffer on severe (1000ms+) lag
    if (time_since_tick > 1.0f) {
        time_since_tick = 0.0f;
    }
    
    delta_tick = 0;
    while (time_since_tick > TICK_RATE) {
        time_since_tick -= TICK_RATE;
        
        delta_tick++;
        ticks++;
        
        Event::Post({
            .type = Event::TICK,
            .poster = ticks
        });
    }
}

/// Returns the current tick.
/// This value gets incremented at a rate of 60 ticks per second.
/// @note   Tick rate of 60 ticks per second is only the default. The actual
///         value might have been changed in the runtime configuration.
/// @return Count of ticks since the application was started.
uint32_t GetTick() {
    return ticks;
}

/// Returns the current frame.
/// @return The number of times Core::Update() has been called.
uint32_t GetFrame() {
    return frames;
}

/// Returns the tick delta.
/// If multiple ticks have occured between calls to Core::Update(), this value
/// will reflect that.
uint32_t GetDeltaTick() {
    return delta_tick;
}


/// Same as GetFrameTime()
double GetTickTime() {
    return frame_time;
}

/// Returns the time at the start of the current frame.
/// This value changes only when Core::Update() is called.
/// @return Time, in seconds, since the application was started.
double GetFrameTime() {
    return frame_time;
}

/// Returns the time passed between frames.
/// This value changes only when Core::Update() is called.
/// @return Time, in seconds, that has passed between the start of the previous
///         and the start of the current frame.
float GetDeltaTime() {
    return delta_time;
}

/// Returns the version identifier of the runtime.
const char* GetVersion() {
    return "Tramway SDK v0.1.1";
}

}


