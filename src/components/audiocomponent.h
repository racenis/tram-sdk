// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// AUDIOCOMPONENT.H -- Sound player component

#ifndef AUDIOCOMPONENT_H
#define AUDIOCOMPONENT_H

#include <audio.h>

namespace Core::Audio {
    struct SoundSource;
}

namespace Core {
    
    class AudioComponent : public EntityComponent {
    public:
        AudioComponent() : sound(this){}
        void Init();
        void Uninit();
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

#endif //AUDIOCOMPONENT_H