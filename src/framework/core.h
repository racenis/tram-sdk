// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_CORE_H
#define TRAM_SDK_FRAMEWORK_CORE_H

#include <cstdint>

namespace tram {

typedef uint32_t id_t;

id_t GenerateID();

uint32_t GetTick();
uint32_t GetFrame();

double GetTickTime();
double GetFrameTime();

uint32_t GetDeltaTick();
float GetDeltaTime();

const char* GetVersion();

namespace Core {
    void Init();
    void Update();
    
    void SetPlatformTime(bool);
    void SetTime(double);
    void AddTime(double);
    
    // TODO: maybe put GenerateID() and GetVersion() in here too
}

}

#endif // TRAM_SDK_FRAMEWORK_CORE_H
