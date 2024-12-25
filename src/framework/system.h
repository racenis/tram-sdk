// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#ifndef TRAM_SDK_FRAMEWORK_SYSTEM_H
#define TRAM_SDK_FRAMEWORK_SYSTEM_H

#include <cstdint>

namespace tram::System {

typedef uint32_t system_t;
enum System : system_t {
    SYSTEM_INVALID,
    SYSTEM_CORE,
    SYSTEM_PLATFORM,
    SYSTEM_UI,
    SYSTEM_GUI,
    SYSTEM_ASYNC,
    SYSTEM_RENDER,
    SYSTEM_PHYSICS,
    SYSTEM_AUDIO,
    SYSTEM_MISC,
    SYSTEM_LAST
};

system_t Register(char const* name, char const* short_name);
system_t Find(char const* name);
void SetInitialized(system_t system, bool is_initialized);
bool IsInitialized(system_t system);
void SetUpdated(system_t system, bool is_updated);
bool IsUpdated(system_t system);
char const* GetName(system_t system);
char const* GetShortName(system_t system);
uint32_t GetSystemCount();

}

#endif // TRAM_SDK_FRAMEWORK_SYSTEM_H