#include <components/audio.h>

#include <audio/api.h>
#include <audio/sound.h>

namespace tram {
    
using namespace tram::Audio;
using namespace tram::Audio::API;

template <> Pool<AudioComponent> PoolProxy<AudioComponent>::pool("audio component pool", 150);

AudioComponent::~AudioComponent() {
    assert(is_ready);
    is_ready = false;
    
    RemoveAudioSource(source);
}

void AudioComponent::Start() {
    source = MakeAudioSource();

    SetAudioSourcePitch(source, 1.0f);
    SetAudioSourceGain(source, 1.0f);
    
    SetAudioSourcePosition(source, location);
    SetAudioSourceVelocity(source, {0.0f, 0.0f, 0.0f});
    
    SetAudioSourceRepeating(source, repeat);
    
    SetAudioSourceBuffer(source, sound->sound_buffers, sound->sound_buffer_count);
    
    if (play_on_start) {
        PlayAudioSource(source);
    }
    
    is_ready = true;
}

/// Sets the sound that the component will play.
void AudioComponent::SetSound(name_t name) {
    this->sound = Sound::Find(name);
}

/// Sets the location from which the sound will play.
void AudioComponent::SetLocation(vec3 location) {
    this->location = location;
    
    if (is_ready) {
        SetAudioSourcePosition(source, location);
    }
}

/// Sets whether the component will repeat its sound.
void AudioComponent::SetRepeating(bool is_repeating) {
    this->repeat = is_repeating;
    
    if (is_ready) {
        SetAudioSourceRepeating(source, repeat);
    }
}

/// Plays the sound.
/// Plays the sound that the component has been set to play.
void AudioComponent::Play() {
    if (is_ready) {
        PlayAudioSource(source);
    } else {
        play_on_start = true;
    }
}

/// Pauses the sound.
/// Pauses the sound that the component is playing, or does nothing, if no sound
/// is playing.
void AudioComponent::Pause() {
    if (is_ready) PauseAudioSource(source);
}

/// Stops the sound.
/// Stops the sound that the component is playing, or does nothing, if no sound
/// is playing.
void AudioComponent::Stop() {
    if (is_ready) {
        StopAudioSource(source);
    } else {
        play_on_start = false;
    }
}

/// Checks if component is playing a sound.
/// @return True, if a sound is playing, false if it hasn't started or has stopped.
bool AudioComponent::IsPlaying() {
    if (is_ready) {
        return IsAudioSourcePlaying(source);
    } else {
        return play_on_start;
    }
}

}