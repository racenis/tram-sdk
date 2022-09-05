#ifndef AUDIO_H
#define AUDIO_H

#include <core.h>

namespace Core::Audio {
    extern glm::vec3 LISTENER_POSITION;
    extern glm::vec3 LISTENER_ORIENTATION[2];
    
    class Sound : public Resource {
    public:
        uint32_t sound_buffer;
        int32_t channels;
        int32_t sample_rate;
        int32_t sound_length;
        int16_t* sound_data;
        
        Sound(){}
        Sound(name_t name) : Resource(name){}
        
        void Load() { LoadFromDisk(); }
        void LoadFromDisk();
        void LoadFromMemory(){}
        void Unload();
        static Sound* Find (name_t name);
    };
    
    void PlaySound(Sound* sound, const glm::vec3& location);
    
    void Init();
    void Update();
    void Uninit();
    
}

#endif // AUDIO_H