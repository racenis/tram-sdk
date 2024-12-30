// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_AUDIOCOMPONENT_H
#define TRAM_SDK_COMPONENTS_AUDIOCOMPONENT_H

#include <framework/entitycomponent.h>
#include <framework/resource.h>

#include <components/render.h>

#include <audio/sound.h>

namespace tram {

class RenderComponent;

class AudioComponent : public EntityComponent {
public:
    AudioComponent() : sound(this){}
    ~AudioComponent();
    void Start();
    void SetLocation(vec3 location);
    void SetSound(name_t name);
    void SetRepeating(bool is_repeating);
    bool IsPlaying();
    void Play();
    void Pause();
    void Stop();
    
    void EventHandler(Event &event){}
    
    static bool IsSourceDraw();
    static bool IsDebugInfoDraw();
    
    static void SetSourceDraw(bool);
    static void SetDebugInfoDraw(bool);
protected:
    ResourceProxy<Audio::Sound> sound;
    Audio::audiosource_t source = {};
    vec3 location = {0.0f, 0.0f, 0.0f};
    bool repeat = false;
    bool play_on_start = false;
    
    void SetupModel();
    Component<RenderComponent> model;
};

}

#endif // TRAM_SDK_COMPONENTS_AUDIOCOMPONENT_H