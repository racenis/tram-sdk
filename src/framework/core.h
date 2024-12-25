// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#ifndef TRAM_SDK_FRAMEWORK_CORE_H
#define TRAM_SDK_FRAMEWORK_CORE_H

#include <cstdint>

namespace tram {

typedef uint32_t id_t;

id_t GenerateID();

uint32_t GetTick();
double GetTickTime();
float GetDeltaTime();

const char* GetVersion();

namespace Core {
    void Init();
    void Update();
}

}

#endif // TRAM_SDK_FRAMEWORK_CORE_H
