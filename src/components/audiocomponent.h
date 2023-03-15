// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_AUDIOCOMPONENT_H
#define COMPONENTS_AUDIOCOMPONENT_H

#include <audio/audio.h>

namespace tram::Audio {
    struct SoundSource;
}

namespace tram {
    
    class AudioComponent : public EntityComponent {
    public:
        AudioComponent() : sound(this){}
        ~AudioComponent();
        void Start();
        void UpdateLocation(const glm::vec3& location);
        void SetSound(Audio::Sound* sound) { this->sound = sound; }
        void SetRepeating(bool is_repeating);
        bool IsPlaying();
        void Play();
        void Pause();
        void Stop();
        
        void EventHandler(Event &event){}
    protected:
        ResourceProxy<Audio::Sound> sound;
        uint32_t source;
        glm::vec3 location;
        bool repeat = false;
    };
    
}

#endif // COMPONENTS_AUDIOCOMPONENT_H