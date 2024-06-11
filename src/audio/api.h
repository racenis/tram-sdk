// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_AUDIO_API_H
#define TRAM_SDK_AUDIO_API_H

#include <audio/audio.h>

namespace tram::Audio::API {

void Init();
void Update();
void Uninit();

void SetListenerPosition(vec3 position);
void SetListenerOrientation(quat orientation);

audiobuffer_t* MakeAudioBuffer(const int16_t* audio_data, int32_t length, int32_t samples, int32_t channels, int32_t& buffer_count);
void RemoveAudioBuffer(audiobuffer_t* buffer, int32_t buffer_count);

audiosource_t MakeAudioSource();

void SetAudioSourcePitch (audiosource_t source, float pitch);
void SetAudioSourceGain (audiosource_t source, float gain);
void SetAudioSourcePosition (audiosource_t source, vec3 position);
void SetAudioSourceVelocity (audiosource_t source, vec3 velocity);
void SetAudioSourceRepeating (audiosource_t source, bool repeating);

void SetAudioSourceBuffer(audiosource_t source, const audiobuffer_t* buffers, int32_t buffer_count);

void PlayAudioSource (audiosource_t source);
void PauseAudioSource (audiosource_t source);
void StopAudioSource (audiosource_t source);

bool IsAudioSourcePlaying (audiosource_t source);

void RemoveAudioSource (audiosource_t source);
    
}

#endif // TRAM_SDK_AUDIO_API_H