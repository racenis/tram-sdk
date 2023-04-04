// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_AUDIO_AUDIO_H
#define TRAM_SDK_AUDIO_AUDIO_H

#include <framework/core.h>
#include <framework/math.h>

namespace tram::Audio {

typedef uint32_t audiosource_t;
typedef uint32_t audiobuffer_t;

class Sound;

void Init();
void Update();
void Uninit();

void SetListenerPosition (vec3 position);
void SetListenerOrientation (quat orientation);

}

#endif // TRAM_SDK_AUDIO_AUDIO_H