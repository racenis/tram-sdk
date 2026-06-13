// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <components/audio.h>

#include <framework/event.h>
#include <framework/settings.h>

#include <audio/api.h>
#include <audio/sound.h>

#include <render/render.h>

#include <config.h>

/**
 * @class tram::AudioComponent components/audio.h <components/audio.h>
 * 
 * Plays back Sounds.
 * @see https://racenis.github.io/tram-sdk/documentation/components/audio.html
 */

/* TODO:
 * I don't really like how the AudioComponent references the RenderComponent for
 * the component's visualization.
 * I feel like a better alternative would be to set up some kind of a
 * Component<RenderComponent> array in this file and then reference them based
 * Pool indices.
 */

namespace tram {

using namespace tram::Audio;
using namespace tram::Audio::API;

template <> Pool<AudioComponent> PoolProxy<AudioComponent>::pool("AudioComponent pool", COMPONENT_LIMIT_AUDIO);
template <> void Component<AudioComponent>::init() { ptr = PoolProxy<AudioComponent>::New(); }
template <> void Component<AudioComponent>::yeet() { PoolProxy<AudioComponent>::Delete(ptr); }

static Settings::Property<bool> draw_source = {false, "audio-draw-icon", Settings::NONE};
static Settings::Property<bool> draw_info = {false, "audio-draw-info", Settings::NONE};

static EventListener frame_event;

static void check_event(const char*) {
    if (draw_source || draw_info) {
        frame_event.make(Event::FRAME, [](Event&) {
            for (auto& source : PoolProxy<AudioComponent>::GetPool()) {
                if(draw_info) {
                    char str[100];
                    sprintf(str, "Playing: %s\nRepeats: %s\nSound: %s",
                            source.IsPlaying() ? "yes" : "no",
                            source.IsRepeating() ? "yes" : " no",
                            source.GetSound() ? (const char*)source.GetSound()->GetName() : "none");
                    Render::AddText(source.GetLocation(), str);
                }
                if (draw_source) {
                    if (glm::distance(source.GetLocation(), Render::GetViewPosition()) < 15.0f) {
                        Render::AddIcon(source.GetLocation(), 9);
                    }
                }
            }
        });
    } else {
        frame_event.clear();
    } 
}

static void make() {
    Settings::SetCallback("audio-draw-icon", check_event);
    Settings::SetCallback("audio-draw-info", check_event);
    
    check_event(nullptr);
}

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
    
    SetAudioSourceBuffer(source, sound->sound_buffer);
    
    if (play_on_start) {
        PlayAudioSource(source);
    }

    is_ready = true;
    
    static bool made = false;
    if (!made) made = true, make();
}

/// Sets the sound that the component will play.
/// @deprecated Use SetSound(Audio::Sound*) instead.
void AudioComponent::SetSound(name_t name) {
    SetSound(Sound::Find(name));
}

/// Sets the sound that the component will play.
void AudioComponent::SetSound(Audio::Sound* sound) {
    if (is_ready) {
        Log(Severity::WARNING, System::AUDIO, "Initialized AudioComponents cannot accept sounds! Ignoring AudioComponent::SetSound() call.");
        return;
    }
    
    this->sound = sound;
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