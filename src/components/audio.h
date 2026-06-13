// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_AUDIOCOMPONENT_H
#define TRAM_SDK_COMPONENTS_AUDIOCOMPONENT_H

#include <framework/entitycomponent.h>

#include <audio/sound.h>

namespace tram {

class RenderComponent;

class AudioComponent : public EntityComponent {
public:
    AudioComponent() : sound(this){}
    ~AudioComponent();
    
    void SetLocation(vec3 location);
    void SetSound(Audio::Sound* sound);
    void SetSound(name_t name);
    void SetRepeating(bool is_repeating);
    bool IsPlaying();
    void Play();
    void Pause();
    void Stop();
    
    Audio::Sound* GetSound() { return sound.get(); }
    bool IsRepeating() const { return repeat; }
    vec3 GetLocation() const { return location; }
    
    void EventHandler(Event &event) {}
protected:
    ResourceProxy<Audio::Sound> sound;
    Audio::audiosource_t source = {};
    vec3 location = {0.0f, 0.0f, 0.0f};
    bool repeat = false;
    bool play_on_start = false;
    
    void Start();
};

}

#endif // TRAM_SDK_COMPONENTS_AUDIOCOMPONENT_H