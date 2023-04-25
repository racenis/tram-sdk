#include <stb_vorbis.c>

// the stb_vorbis and glm use these same macroses
#undef L
#undef R
#undef C

#include <audio/sound.h>
#include <audio/api.h>

#include <framework/logging.h>
#include <framework/system.h>

#include <templates/hashmap.h>

template <> tram::Pool<tram::Audio::Sound> tram::PoolProxy<tram::Audio::Sound>::pool("sound pool", 100, false);

namespace tram::Audio {

Hashmap<Sound*> SOUND_MAP ("Sound hash map", 500);

/// Loads the sound from the disk.
void Sound::LoadFromDisk() {
    sound_length = stb_vorbis_decode_filename((std::string("data/audio/") + std::string(name) + ".ogg").c_str(), &channels, &sample_rate, &sound_data);
    
    Log (SEVERITY_INFO, System::SYSTEM_AUDIO, "Bytelength: {} Channels: {} SampleRate: {} ", sound_length, channels, sample_rate);
    
    if (sound_length < 0) {
        Log (SEVERITY_ERROR, System::SYSTEM_AUDIO, "There was an error loading the sound {}", name);
    } else {
        sound_buffers = MakeAudioBuffer(sound_data, sound_length, sample_rate, channels, sound_buffer_count);
    }
    
    free (sound_data);
    sound_data = nullptr;
    
    status = READY;
}

/// Removes the sound from memory.
void Sound::Unload() {
    RemoveAudioBuffer(sound_buffers, sound_buffer_count);
    status = UNLOADED;
}

/// Finds a sound by a name.
/// If a sound by that name doesn't exist, then it will be created.
/// @return Always returns a pointer to a Sound.
Sound* Sound::Find (name_t name) {
    auto sound = SOUND_MAP.Find(name);
    
    if (!sound) {
        sound = PoolProxy<Sound>::New(name);
        SOUND_MAP.Insert(name, sound);
    }
    
    return sound;
}

}