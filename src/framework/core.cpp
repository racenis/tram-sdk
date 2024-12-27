// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/core.h>
#include <framework/event.h>
#include <framework/system.h>
#include <framework/uid.h>

#include <platform/api.h>

#include <iostream>
#include <cassert>

/**
 * @file framework/core.cpp
 * 
 * Core functionality of the framework.
 */

/**
 * TODO:
 * - properly implement TICK event
 *          it gets emitted 60 times a second, no matter the frames
 *          set the tick number as data_int
 * - properly implement FRAME event
 *          it gets called every Core::Update()
 *          set the frame number as data_int
 * - properly implement GetTick()
 *          it returns f
 * - implement a GetTickDelta()
 *          it returns the delta between ticks for every frame
 * - implement a GetFrame()
 *          it returns the frames since the beginning of time
 */

namespace tram {

static double frame_time = 0.0f;
static float delta_time = 0.0f;
static uint32_t frame_tick = 0;

/// Generates a unique ID number.
id_t GenerateID() {
    static id_t num = 0;
    num++;
    return num;
}

/// Initializes the core of the framework.
void Core::Init() {
    assert(!System::IsInitialized(System::CORE));
    
    System::SetInitialized(System::CORE, true);
    
    std::cout << GetVersion() << std::endl;
}

/// Updates the core system.
/// @note This should be called only once per update cycle.
void Core::Update() {
    double last_frame_time = frame_time;
    frame_time = Platform::Window::GetTime();
    delta_time = frame_time - last_frame_time;
    frame_tick++;

    Event::Post({
        .type = Event::TICK,
        .poster_id = 0
    });
    
    // TODO: implement this properly
    Event::Post({
        .type = Event::FRAME,
        .poster_id = 0
    });
    
    
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
double GetTickTime() {
    return frame_time;
}

/// Returns the time passed between ticks.
/// This value changes only once per update cycle.
/// @return Time, in seconds, that has passed between the start of the previous
///         and the start of the current tick.
float GetDeltaTime() {
    return delta_time;
}

const char* GetVersion() {
    return "Tramway SDK v0.1.0";
}

}


