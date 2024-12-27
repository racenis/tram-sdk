// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <components/audio.h>

#include <framework/event.h>

#include <audio/api.h>
#include <audio/sound.h>

#include <config.h>

/**
 * @class tram::AudioComponent
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

// TODO: convert these into Settings?
static bool draw_source = false;
static bool draw_info = false;

static EventListener frame_event;

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
    
    SetupModel();
    
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
    
    if (model) {
        model->SetLocation(location);
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


void AudioComponent::SetupModel() {
    if (draw_source) {
        model.make();
        model->SetModel("dev/sound");
        model->SetLocation(this->location);
        model->SetParent(this->parent);
        model->SetLightmap("fullbright");
        model->Init();
    } else {
        model.clear();
    }
}

/// Checks whether the audio source is drawn.
/// Check SetSourceDraw() for more info.
bool AudioComponent::IsSourceDraw() {
    return draw_source;
}

/// Checks whether the debug text is drawn.
/// Check SetDebugInfoDraw() for more info.
bool AudioComponent::IsDebugInfoDraw() {
    return draw_info;
}

/// Sets the drawing of audio sources.
/// If set to true, each audio component will initialize a 3D model, which will
/// allow you to see the posiiton of the audio sources. Useful for debugging.
void AudioComponent::SetSourceDraw(bool draw) {
    if (draw_source == draw) return;
    draw_source = draw;
    
    for (auto& source : PoolProxy<AudioComponent>::GetPool()) {
        source.SetupModel();
    }
}

/// Sets the drawing of debug info.
/// If set to true, each frame some debug text will be drawn for each audio
/// source. This is useful for debugging.
void AudioComponent::SetDebugInfoDraw(bool draw) {
    if (draw_info == draw) return;
    draw_info = draw;
    
    if (draw) {
        frame_event.make(Event::FRAME, [](Event&) {
            for (auto& source : PoolProxy<AudioComponent>::GetPool()) {
                char str[100];
                sprintf(str, "Playing: %s\nRepeats: %s\nSound: %s",
                        source.IsPlaying() ? "yes" : "no",
                        source.repeat ? "yes" : " no",
                        source.sound ? (const char*)source.sound->GetName() : "none");
                Render::AddText(source.location, str);
            }
        });
    } else {
        frame_event.clear();
    } 
}

}