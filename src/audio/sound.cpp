// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

// using the stb_vorbis library as a header-only library. we could compile it
// separately, but I don't feel like it is worth the effort
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

#include <config.h>

/**
 * @class tram::Audio::Sound audio/sound.h <audio/sound.h>
 * 
 * Audio data resource.
 * 
 * Currently we only support `.ogg` files, via the `stb_vorbis` library.
 */

template <> tram::Pool<tram::Audio::Sound> tram::PoolProxy<tram::Audio::Sound>::pool("Sound Resource pool", RESOURCE_LIMIT_SOUND);

namespace tram::Audio {

static Hashmap<Sound*> sound_map("Sound Resource hashmap", RESOURCE_LIMIT_SOUND);

/// Loads the sound from the disk.
void Sound::LoadFromDisk() {
    if (status != UNLOADED) {
        Log(Severity::WARNING, System::AUDIO, "Sound {} already loaded! Ignoring Sound::LoadFromDisk() call.", name);
        return;
    }
    
    char path[PATH_LIMIT];
    snprintf(path, PATH_LIMIT, "data/audio/%s.ogg", (const char*)name);
    
    Log(Severity::INFO, System::AUDIO, "Loading: {} ", path);
    sound_length = stb_vorbis_decode_filename(path, &channels, &sample_rate, &sound_data);
    
    Log(Severity::INFO, System::AUDIO, "Bytelength: {} Channels: {} SampleRate: {} ", sound_length, channels, sample_rate);
    
    if (sound_length < 0) {
        load_fail = true;
        Log(Severity::WARNING, System::AUDIO, "Failed to load: {}", path);
        
        sample_rate = 11025;
        sound_length = 11025;
        sound_data = (int16_t*)malloc(sizeof(int16_t) * 11025);
        channels = 1;
        
        // generate some noise as a placeholder
        for (int i = 0; i < 11025; i++) {
            float signal = sinf((float)i / 11.025f * 2.0f * 3.14159f);
            float amplitude = sinf((float)i / 1378.125f * 2.0f * 3.14159f);
            sound_data[i] = 32000.0f * signal * amplitude;
        }
    }

    sound_buffer = API::MakeAudioBuffer(sound_data, sound_length, sample_rate, channels);

    free(sound_data);
    sound_data = nullptr;
    
    status = READY;
}

/// Removes the sound from memory.
void Sound::Unload() {
    API::RemoveAudioBuffer(sound_buffer);
    status = UNLOADED;
}

/// Finds a sound by a name.
/// If a sound by that name doesn't exist, it will be created.
/// @return Always returns a pointer to a Sound.
Sound* Sound::Find(name_t name) {
    auto sound = sound_map.Find(name);
    
    if (!sound) {
        sound = PoolProxy<Sound>::New(name);
        sound_map.Insert(name, sound);
    }
    
    return sound;
}

}