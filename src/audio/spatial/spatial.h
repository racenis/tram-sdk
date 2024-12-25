// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

/**
 * @file audio/spatial/spatial.h
 * 
 * Definitions for the Spatial Audio backend.
 * 
 * It hasn't been compiled in years, so it is probably broken, but I will keep
 * the code here, just in case I want to revive it in the future.
 */

#ifndef TRAM_SDK_AUDIO_SPATIAL_SPATIAL_H
#define TRAM_SDK_AUDIO_SPATIAL_SPATIAL_H

/*#include <audio/audio.h>

namespace tram::Audio::Spatial {

void Init();
void Update();
void Uninit();

void SetListenerPosition(const vec3& position);
void SetListenerOrientation(const quat& orientation);

audiobuffer_t* MakeAudioBuffer(const int16_t* audio_data, int32_t length, int32_t samples, int32_t channels, int32_t& buffer_count);
void RemoveAudioBuffer(audiobuffer_t* buffers, int32_t buffer_count);

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

}*/

#endif // TRAM_SDK_AUDIO_SPATIAL_SPATIAL_H