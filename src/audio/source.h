// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#include <audio/audio.h>

namespace Core::Audio {
    audiosource_t MakeAudioSource();
    
    void SetAudioSourcePitch (audiosource_t source, float pitch);
    void SetAudioSourceGain (audiosource_t source, float gain);
    void SetAudioSourcePosition (audiosource_t source, vec3 position);
    void SetAudioSourceVelocity (audiosource_t source, vec3 velocity);
    void SetAudioSourceRepeating (audiosource_t source, bool repeating);
    
    void SetAudioSourceSound (audiosource_t source, Sound* sound);

    void PlayAudioSource (audiosource_t source);
    void PauseAudioSource (audiosource_t source);
    void StopAudioSource (audiosource_t source);

    bool IsAudioSourcePlaying (audiosource_t source);
    
    void RemoveAudioSource (audiosource_t source);
}

#endif // AUDIO_SOURCE_H