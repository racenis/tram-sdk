// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <audio/api.h>

#include <framework/logging.h>

/**
 * @file audio/template/template.cpp
 * 
 * Template for Audio backend implementations.
 * 
 * This template can also be used as a blank Audio backend if no audio is needed
 * to be played and the system won't be used.
 */


namespace tram::Audio::API {

void Init() {
    Log(Severity::INFO, System::AUDIO, "Audio API initialized.");
}

void Update() {        
    
}

void Uninit() {
    Log(Severity::INFO, System::AUDIO, "Audio API uninitialized.");
}

/// Sets the listener position.
/// @param position Listener position. Under normal circumstances, it should be
///                 the same as render view position.
void SetListenerPosition(vec3 position) {
    
}

/// Sets the listener orientation.
/// @param orientation  Listener orientation. Under normal circumstances, it should be
///                     the same as render view rotation.
void SetListenerOrientation(quat orientation) {
    
}

/// Creates audio buffers from raw audio data.
/// @param audio_data       Pointer to the raw audio data, stored as 16 bit PCM.
/// @param length           Length of the raw audio data, in samples.
/// @param buffer_count     Created buffer count will be stored in this variable.
audiobuffer_t MakeAudioBuffer(const int16_t* audio_data, int32_t length, int32_t samples, int32_t channels) {
    Log(Severity::INFO, System::AUDIO, "Created an audio buffer of {}@{}hz!", length, samples);
    return audiobuffer_t {.generic = nullptr};
}

/// Deletes audio buffers.
void RemoveAudioBuffer(audiobuffer_t buffer) {
    Log(Severity::INFO, System::AUDIO, "Deleted audio buffer.");
}

/// Creates an audio source.
/// @return A new audio source handle.
audiosource_t MakeAudioSource() {
    Log(Severity::INFO, System::AUDIO, "Created an audio source.");
    return audiosource_t {.generic = nullptr};
}

/// Sets the pitch of an audio source.
/// @param pitch    1.0f is the regular pitch. Halving the value decreases the
///                 pitch by an octave. Doubling the value increases the pitch
///                 by an octave.
void SetAudioSourcePitch (audiosource_t source, float pitch) {
    
}

/// Sets the gain of an audio source.
/// @param gain     1.0f is the regular gain. Halving the value decreases the
///                 gain by 6dB.
void SetAudioSourceGain (audiosource_t source, float gain) {
    
}

/// Sets the position of an audio source.
void SetAudioSourcePosition (audiosource_t source, vec3 position) {
    
}

/// Sets the velocity of an audio source.
void SetAudioSourceVelocity (audiosource_t source, vec3 velocity) {
    
}

/// Sets the repetition of an audio source.
/// @param repeating    True if the sound repeats after playing, false if
///                     it just stops.
void SetAudioSourceRepeating (audiosource_t source, bool repeating) {
    
}

/// Sets the buffers that the audio source will play.
void SetAudioSourceBuffer(audiosource_t source, audiobuffer_t buffer) {
    
}

/// Plays an audio source.
void PlayAudioSource (audiosource_t source) {
    
}

/// Pauses the audio source.
void PauseAudioSource (audiosource_t source) {
    
}

/// Stops the audio source.
void StopAudioSource (audiosource_t source) {
    
}

/// Checks if an audio source is playing.
/// @return True if the audio source is playing a sound, false otherwise.
bool IsAudioSourcePlaying (audiosource_t source) {
    return false;
}

/// Deletes an audio source.
/// If you do this, then the audio source handle will become invalid.
/// Call MakeAudioSource() to get a new one.
void RemoveAudioSource (audiosource_t source) {
    
}

    
}