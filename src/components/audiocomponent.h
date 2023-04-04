// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_AUDIOCOMPONENT_H
#define COMPONENTS_AUDIOCOMPONENT_H

#include <framework/entitycomponent.h>
#include <framework/resource.h>

#include <audio/sound.h>

namespace tram {

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
protected:
    ResourceProxy<Audio::Sound> sound;
    Audio::audiosource_t source;
    vec3 location = {0.0f, 0.0f, 0.0f};
    bool repeat = false;
};

}

#endif // COMPONENTS_AUDIOCOMPONENT_H