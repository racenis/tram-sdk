// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_AUDIO_AUDIO_H
#define TRAM_SDK_AUDIO_AUDIO_H

#include <framework/core.h>
#include <framework/math.h>

namespace tram::Audio {

struct audiosource_t {
    uint32_t al_source;
};

struct audiobuffer_t {
    uint32_t* al_buffers;
    int32_t al_buffer_count;
};

class Sound;

void Init();
void Update();
void Uninit();

void SetListenerPosition(vec3 position);
void SetListenerOrientation(quat orientation);

}

#endif // TRAM_SDK_AUDIO_AUDIO_H