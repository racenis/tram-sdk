// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <al.h>

// the stb_vorbis and glm use the same macroses
#undef L
#undef R
#undef C

#include <audio/source.h>

namespace tram::Audio {
    /// Creates an audio source.
    /// @return A new audio source handle.
    audiosource_t MakeAudioSource() {
        audiosource_t source;
        alGenSources(1, &source);
        return source;
    }
    
    /// Sets the pitch of an audio source.
    /// @param pitch    1.0f is the regular pitch. Halving the value decreases the
    ///                 pitch by an octave. Doubling the value increases the pitch
    ///                 by an octave.
    void SetAudioSourcePitch (audiosource_t source, float pitch) {
        alSourcef(source, AL_PITCH, pitch);
    }
    
    /// Sets the gain of an audio source.
    /// @param gain     1.0f is the regular gain. Halving the value decreases the
    ///                 gain by 6dB.
    void SetAudioSourceGain (audiosource_t source, float gain) {
        alSourcef(source, AL_GAIN, gain);
    }
    
    /// Sets the position of an audio source.
    void SetAudioSourcePosition (audiosource_t source, vec3 position) {
        alSource3f(source, AL_POSITION, position.x, position.y, position.z);
    }
    
    /// Sets the velocity of an audio source.
    void SetAudioSourceVelocity (audiosource_t source, vec3 velocity) {
        alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    }
    
    /// Sets the repetition of an audio source.
    /// @param repeating    True if the sound repeats after playing, false if
    ///                     it just stops.
    void SetAudioSourceRepeating (audiosource_t source, bool repeating) {
        alSourcei(source, AL_LOOPING, repeating ? AL_TRUE : AL_FALSE);
    }
    
    /// Sets the sound that an audio source will play.
    void SetAudioSourceSound (audiosource_t source, Sound* sound) {
        if (sound->sound_buffer_count == 1) {
            alSourcei(source, AL_BUFFER, sound->sound_buffers[0]);
        } else {
            alSourceQueueBuffers(source, sound->sound_buffer_count, sound->sound_buffers);
        }
    }
    
    /// Plays an audio source.
    void PlayAudioSource (audiosource_t source) {
        alSourcePlay(source);
    }
    
    /// Pauses the audio source.
    void PauseAudioSource (audiosource_t source) {
        alSourcePause(source);
    }
    
    /// Stops the audio source.
    void StopAudioSource (audiosource_t source) {
        alSourceStop(source);
    }
    
    /// Checks if an audio source is playing.
    /// @return True if the audio source is playing a sound, false otherwise.
    bool IsAudioSourcePlaying (audiosource_t source) {
        int32_t state;
        alGetSourcei (source, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }
    
    /// Deletes an audio source.
    /// If you do this, then the audio source handle will become invalid.
    /// Call MakeAudioSource() to get a new one.
    void RemoveAudioSource (audiosource_t source) {
        alDeleteSources(1, &source);
    }
}