// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_SYSTEM_H
#define TRAM_SDK_FRAMEWORK_SYSTEM_H

#include <cstdint>

namespace tram::System {

typedef uint32_t system_t;

enum System : system_t {
    INVALID,
    CORE,
    PLATFORM,
    UI,
    GUI,
    ASYNC,
    RENDER,
    PHYSICS,
    AUDIO,
    MISC,
    SYSTEM_LAST
};

enum SystemState {
    YEET,
    INIT,
    READY
};

system_t Register(char const* name, char const* short_name);
system_t Find(char const* name);

void SetInitialized(system_t system, bool is_initialized);
bool IsInitialized(system_t system);

void SetState(system_t system, SystemState state); 
void AssertDependency(system_t system);

char const* GetName(system_t system);
char const* GetShortName(system_t system);
uint32_t GetSystemCount();

}

#endif // TRAM_SDK_FRAMEWORK_SYSTEM_H