// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <audio/audio.h>
#include <audio/sound.h>

#include <framework/system.h>
#include <framework/logging.h>

#include <components/audiocomponent.h>
#include <unordered_map>


#include <audio/openal/openal.h>


namespace tram::Audio {

/// Sets the listener position.
/// @param orientation  Listener position. Basically the same as camera location.
void SetListenerPosition (vec3 position) {
    OpenAL::SetListenerPosition (position);
}

/// Sets the listener orientation.
/// @param orientation  Listener orientation. Basically the same as camera rotation.
void SetListenerOrientation (quat orientation) {
    OpenAL::SetListenerOrientation (orientation);
}

/// Creates audio buffers from raw audio data.
/// @param audio_data       Pointer to the raw audio data.
/// @param length           Length of the raw audio data, in samples.
/// @param buffer_count     Created buffer count will be stored in this variable.
audiobuffer_t* MakeAudioBuffer (const int16_t* audio_data, int32_t length, int32_t samples, int32_t channels, int32_t& buffer_count) {
    return OpenAL::MakeAudioBuffer (audio_data, length, samples, channels, buffer_count);
}

/// Deletes audio buffers.
void RemoveAudioBuffer (audiobuffer_t* buffer, int32_t buffer_count) {
    OpenAL::RemoveAudioBuffer (buffer, buffer_count);
    delete buffer;
}

/// Creates an audio source.
/// @return A new audio source handle.
audiosource_t MakeAudioSource() {
    return OpenAL::MakeAudioSource();
}

/// Sets the pitch of an audio source.
/// @param pitch    1.0f is the regular pitch. Halving the value decreases the
///                 pitch by an octave. Doubling the value increases the pitch
///                 by an octave.
void SetAudioSourcePitch (audiosource_t source, float pitch) {
    OpenAL::SetAudioSourcePitch (source, pitch);
}

/// Sets the gain of an audio source.
/// @param gain     1.0f is the regular gain. Halving the value decreases the
///                 gain by 6dB.
void SetAudioSourceGain (audiosource_t source, float gain) {
    OpenAL::SetAudioSourceGain (source, gain);
}

/// Sets the position of an audio source.
void SetAudioSourcePosition (audiosource_t source, vec3 position) {
    OpenAL::SetAudioSourcePosition (source, position);
}

/// Sets the velocity of an audio source.
void SetAudioSourceVelocity (audiosource_t source, vec3 velocity) {
    OpenAL::SetAudioSourceVelocity (source, velocity);
}

/// Sets the repetition of an audio source.
/// @param repeating    True if the sound repeats after playing, false if
///                     it just stops.
void SetAudioSourceRepeating (audiosource_t source, bool repeating) {
    OpenAL::SetAudioSourceRepeating (source, repeating);
}

/// Sets the buffers that the audio source will play.
void SetAudioSourceBuffer (audiosource_t source, const audiobuffer_t* buffers, int32_t buffer_count) {
    OpenAL::SetAudioSourceBuffer (source, buffers, buffer_count);
}

/// Plays an audio source.
void PlayAudioSource (audiosource_t source) {
    OpenAL::PlayAudioSource (source);
}

/// Pauses the audio source.
void PauseAudioSource (audiosource_t source) {
    OpenAL::PauseAudioSource (source);
}

/// Stops the audio source.
void StopAudioSource (audiosource_t source) {
    OpenAL::StopAudioSource (source);
}

/// Checks if an audio source is playing.
/// @return True if the audio source is playing a sound, false otherwise.
bool IsAudioSourcePlaying (audiosource_t source) {
    return OpenAL::IsAudioSourcePlaying (source);
}

/// Deletes an audio source.
/// If you do this, then the audio source handle will become invalid.
/// Call MakeAudioSource() to get a new one.
void RemoveAudioSource (audiosource_t source) {
    OpenAL::RemoveAudioSource (source);
}

}
