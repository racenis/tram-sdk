#include <components/audiocomponent.h>

#include <audio/audio.h>
#include <audio/sound.h>

namespace tram {
    
using namespace tram::Audio;

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
    
    is_ready = true;
}

void AudioComponent::SetSound(name_t name) {
    this->sound = Sound::Find(name);
}

void AudioComponent::UpdateLocation(const glm::vec3& location) {
    this->location = location;
    
    if (is_ready) {
        SetAudioSourcePosition(source, location);
    }
}

void AudioComponent::SetRepeating(bool is_repeating) {
    this->repeat = is_repeating;
    
    if (is_ready) {
        SetAudioSourceRepeating(source, repeat);
    }
}

void AudioComponent::Play() {
    if (is_ready) PlayAudioSource(source);
}

void AudioComponent::Pause() {
    if (is_ready) PauseAudioSource(source);
}

void AudioComponent::Stop() {
    if (is_ready) StopAudioSource(source);
}

bool AudioComponent::IsPlaying() {
    if (is_ready) {
        return IsAudioSourcePlaying(source);
    } else {
        return false;
    }
}

}