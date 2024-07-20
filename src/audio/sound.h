// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_AUDIO_SOUND_H
#define TRAM_SDK_AUDIO_SOUND_H

#include <audio/audio.h>

#include <framework/resource.h>

namespace tram::Audio {

class Sound : public Resource {
public:
    audiobuffer_t sound_buffer = {0};
    int32_t channels = 0;
    int32_t sample_rate = 0;
    int32_t sound_length = 0;
    int16_t* sound_data = nullptr;
    
    void LoadFromDisk();
    void LoadFromMemory(){}
    void Unload();
    
    /// Returns sound length in seconds.
    inline float GetLength() { return (float)sound_length/(float)sample_rate; }
    
    static Sound* Find (name_t name);
protected:
    Sound(name_t name) : Resource(name) {}
    template <typename> friend class tram::Pool;
};
    
}

#endif // TRAM_SDK_AUDIO_SOUND_H
    