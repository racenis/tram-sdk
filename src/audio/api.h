// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_AUDIO_API_H
#define TRAM_SDK_AUDIO_API_H

#include <audio/audio.h>

/**
 * @file audio/api.h
 * 
 * Audio API implementation header.
 * 
 * This file contains the function signatures which need to be implemented by an
 * Audio API backend. 
 */

namespace tram::Audio::API {

void Init();
void Update();
void Uninit();

void SetListenerPosition(vec3 position);
void SetListenerOrientation(quat orientation);
void SetListenerGain(float gain);

audiobuffer_t MakeAudioBuffer(const int16_t* audio_data, int32_t length, int32_t samples, int32_t channels);
void RemoveAudioBuffer(audiobuffer_t buffer);

audiosource_t MakeAudioSource();

void SetAudioSourcePitch(audiosource_t source, float pitch);
void SetAudioSourceGain(audiosource_t source, float gain);
void SetAudioSourcePosition(audiosource_t source, vec3 position);
void SetAudioSourceVelocity(audiosource_t source, vec3 velocity);
void SetAudioSourceRepeating(audiosource_t source, bool repeating);

void SetAudioSourceBuffer(audiosource_t source, audiobuffer_t buffer);

void PlayAudioSource(audiosource_t source);
void PauseAudioSource(audiosource_t source);
void StopAudioSource(audiosource_t source);

bool IsAudioSourcePlaying(audiosource_t source);

void RemoveAudioSource(audiosource_t source);
    
}

#endif // TRAM_SDK_AUDIO_API_H