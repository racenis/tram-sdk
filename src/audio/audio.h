// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_AUDIO_AUDIO_H
#define TRAM_SDK_AUDIO_AUDIO_H

#include <framework/core.h>
#include <framework/math.h>

namespace tram::Audio {

struct audiosource_t {
    union {
        uint32_t al_source;
        void* generic;
    };
};

struct audiobuffer_t {
    union {
        struct {
            uint32_t* al_buffers;
            int32_t al_buffer_count;
        };
        void* generic;
    };
    
};

class Sound;

void Init();
void Update();
void Uninit();

void SetVolume(float value);
float GetVolume();

void SetListenerPosition(vec3 position);
void SetListenerOrientation(quat orientation);

}

#endif // TRAM_SDK_AUDIO_AUDIO_H