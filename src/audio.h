#ifndef AUDIO_H
#define AUDIO_H

#include <core.h>

namespace Core::Audio {
    extern glm::vec3 LISTENER_POSITION;
    extern glm::vec3 LISTENER_ORIENTATION[2];
    const size_t SOUND_BUFFERS_PER_SOUND = 10;
    
    class Sound : public Resource {
    public:
        int32_t sound_buffer_count = 0;
        uint32_t sound_buffers[SOUND_BUFFERS_PER_SOUND] = {0};
        int32_t channels = 0;
        int32_t sample_rate = 0;
        int32_t sound_length = 0;
        int16_t* sound_data = nullptr;
        
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