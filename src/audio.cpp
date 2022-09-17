#include <stb_vorbis.c>
#include <alc.h>
#include <al.h>

// the stb_vorbis and glm use the same macroses
#undef L
#undef R
#undef C

#include <audio.h>
#include <components/audiocomponent.h>

namespace Core::Audio {
    struct SoundSource {
        uint32_t source_name;
        
        char padding[12];
    };
    
    Pool<SoundSource> all_sounds("sound source pool", 100, false);
    std::unordered_map<name_t, Sound*> sound_map;
    
    glm::vec3 LISTENER_POSITION = glm::vec3(0.0f);
    glm::vec3 LISTENER_ORIENTATION[2] = {glm::vec3(0.0f), glm::vec3(0.0f)};
    
    ALCdevice* sound_device;
    ALCcontext* sound_context;
    
    
    void PlaySound(Sound* sound, const glm::vec3& location) {
        abort();
        SoundSource* source = all_sounds.AddNew();
        
        alGenSources(1, &source->source_name);
        alSourcef( source->source_name, AL_PITCH, 1);
        alSourcef( source->source_name, AL_GAIN, 1.0f);
        alSource3f( source->source_name, AL_POSITION, location.x, location.y, location.z);
        alSource3f( source->source_name, AL_VELOCITY, 0, 0, 0);
        alSourcei( source->source_name, AL_LOOPING, AL_FALSE);
        alSourcei( source->source_name, AL_BUFFER, sound->sound_buffer);

        alSourcePlay(source->source_name);
            
        std::cout << "Playing the sound " << ReverseUID(sound->GetName()) << std::endl;
    }
    
    void Init() {
        sound_device = alcOpenDevice(nullptr);
        if (!sound_device) printf("Audio device didn't initialize!\n");
        sound_context = alcCreateContext(sound_device, nullptr);
        if (!sound_context) printf("Audio context didn't initialize!\n");
        if (!alcMakeContextCurrent(sound_context)) printf("Audio context didn't get currented!\n");
        const char* name = nullptr;
        if (alcIsExtensionPresent(sound_device, "ALC_ENUMERATE_ALL_EXT"))
            name = alcGetString(sound_device, ALC_ALL_DEVICES_SPECIFIER);
        if (!name || alcGetError(sound_device) != ALC_NO_ERROR)
            name = alcGetString(sound_device, ALC_DEVICE_SPECIFIER);
        printf("Opened \"%s\"\n", name);
    }
    
    void Update() {
        /*SoundSource* it = all_sounds.begin().ptr;
        while (it < all_sounds.end().ptr) {
            if (*((uint64_t*)it) != 0) {
                int32_t state;
                alGetSourcei (it->source_name, AL_SOURCE_STATE, &state);
                
                if (state != AL_PLAYING) {
                    alDeleteSources(1, &it->source_name);
                    all_sounds.Remove(it);
                    std::cout << "Sound yeeted" << std::endl;
                }
            }
            
            it++;
        }*/
        
        
        
        alListener3f( AL_POSITION, LISTENER_POSITION.x, LISTENER_POSITION.y, LISTENER_POSITION.z);
        alListenerfv(AL_ORIENTATION, &LISTENER_ORIENTATION[0][0]);
    }
    
    void Uninit() {
        for (auto it : all_sounds) {
            alDeleteSources(1, &it.source_name);
        }
        
        for (auto it : sound_map) {
            alDeleteBuffers(1, &it.second->sound_buffer);
        }
        
        
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(sound_context);
        alcCloseDevice(sound_device);
    }
    
    void Sound::LoadFromDisk() {
        sound_length = stb_vorbis_decode_filename((std::string("data/audio/") + ReverseUID(name) + ".ogg").c_str(), &channels, &sample_rate, &sound_data);
        
        printf("data_len: %d, channels: %d, sample_rate: %d, audio: %lld\n", sound_length, channels, sample_rate, (long long)sound_data);
        
        if (sound_length < 0) {
            std::cout << "There was an error loading the sound " << ReverseUID(name) << std::endl;
        } else {
            alGenBuffers(1, &sound_buffer);

            int32_t format;
            if(channels == 1)
                format = AL_FORMAT_MONO16;
            else if(channels == 2)
                format = AL_FORMAT_STEREO16;

            alBufferData (sound_buffer, format, sound_data, sound_length, sample_rate);
            
            sound_map[name] = this;
        }
    }
    
    void Sound::Unload() {
        alDeleteBuffers(1, &sound_buffer);
    }
    
    Sound* Sound::Find (name_t name) {
        auto sound = sound_map.find(name);
        
        if (sound != sound_map.end()) {
            return sound->second;
        } else {
            std::cout << "Can't find the sound " << ReverseUID(name) << ", aborting." << std::endl;
            abort();
        }
    }
    
}

namespace Core {
    using namespace Core::Audio;
    
    template <> Pool<AudioComponent> PoolProxy<AudioComponent>::pool("audio component pool", 150);
    
    void AudioComponent::Init() {
        if (resources_waiting == 0) Start();
    }
    
    void AudioComponent::Uninit() {
        alDeleteSources(1, &source->source_name);
        is_ready = false;
    }
    
    void AudioComponent::Start() {
        source = all_sounds.AddNew();
        
        alGenSources(1, &source->source_name);
        alSourcef(source->source_name, AL_PITCH, 1);
        alSourcef(source->source_name, AL_GAIN, 1.0f);
        alSource3f(source->source_name, AL_POSITION, location.x, location.y, location.z);
        alSource3f(source->source_name, AL_VELOCITY, 0, 0, 0);
        alSourcei(source->source_name, AL_LOOPING, repeat ? AL_TRUE : AL_FALSE);
        alSourcei(source->source_name, AL_BUFFER, sound->sound_buffer);
        
        is_ready = true;
    }
    
    void AudioComponent::UpdateLocation(const glm::vec3& location) {
        if (is_ready) alSource3f(source->source_name, AL_POSITION, location.x, location.y, location.z);
        this->location = location;
    }
    
    void AudioComponent::SetRepeating(bool is_repeating) {
        if (is_ready) alSourcei(source->source_name, AL_LOOPING, is_repeating ? AL_TRUE : AL_FALSE);
        repeat = is_repeating;
    }
    
    void AudioComponent::Play() {
        if (is_ready) alSourcePlay(source->source_name);
    }
    
    void AudioComponent::Pause() {
        if (is_ready) alSourcePause(source->source_name);
    }
    
    void AudioComponent::Stop() {
        if (is_ready) alSourceStop(source->source_name);
    }
}