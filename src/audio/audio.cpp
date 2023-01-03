#include <stb_vorbis.c>
#include <alc.h>
#include <al.h>

// the stb_vorbis and glm use the same macroses
#undef L
#undef R
#undef C

#include <audio/audio.h>
#include <components/audiocomponent.h>

namespace Core::Audio {
    struct SoundSource {
        uint32_t source_name;
        
        char padding[12];
    };
    
    Pool<SoundSource> all_sounds("sound source pool", 100, false);
    std::unordered_map<uint64_t, Sound*> sound_map;
    
    glm::vec3 LISTENER_POSITION = glm::vec3(0.0f);
    glm::vec3 LISTENER_ORIENTATION[2] = {glm::vec3(0.0f), glm::vec3(0.0f)};
    
    ALCdevice* sound_device;
    ALCcontext* sound_context;
    
    
    void PlaySound(Sound* sound, const glm::vec3& location) {
        assert(false && "PlaySound() not implemented");
    }
    
    void Init() {
        sound_device = alcOpenDevice(nullptr);
        if (!sound_device) std::cout << "Audio device didn't open!" << std::endl;
        sound_context = alcCreateContext(sound_device, nullptr);
        if (!sound_context) std::cout << "Audio context didn't create!" << std::endl;
        if (!alcMakeContextCurrent(sound_context)) std::cout << "Audio context didn't get currented!" << std::endl;
        const char* device_name = nullptr;
        if (alcIsExtensionPresent(sound_device, "ALC_ENUMERATE_ALL_EXT")) device_name = alcGetString(sound_device, ALC_ALL_DEVICES_SPECIFIER);
        if (!device_name || alcGetError(sound_device) != ALC_NO_ERROR) device_name = alcGetString(sound_device, ALC_DEVICE_SPECIFIER);
        std::cout << "Audio device: " << device_name << std::endl;
    }
    
    void Update() {        
        alListener3f( AL_POSITION, LISTENER_POSITION.x, LISTENER_POSITION.y, LISTENER_POSITION.z);
        alListenerfv(AL_ORIENTATION, &LISTENER_ORIENTATION[0][0]);
    }
    
    void Uninit() {
        for (auto& it : PoolProxy<AudioComponent>::GetPool()) it.~AudioComponent();
        for (auto it : sound_map) it.second->Unload();
        
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(sound_context);
        alcCloseDevice(sound_device);
    }
    
    void SetListenerPosition(const glm::vec3& position) {
        LISTENER_POSITION = position;
    }
    
    void SetListenerOrientation(const glm::quat& orientation) {
        LISTENER_ORIENTATION[0] = orientation * DIRECTION_FORWARD;
        LISTENER_ORIENTATION[1] = DIRECTION_UP;
    }
    
    void Sound::LoadFromDisk() {
        sound_length = stb_vorbis_decode_filename((std::string("data/audio/") + std::string(name) + ".ogg").c_str(), &channels, &sample_rate, &sound_data);
        
        printf("data_len: %d, channels: %d, sample_rate: %d, audio: %lld\n", sound_length, channels, sample_rate, (long long)sound_data);
        
        if (sound_length < 0) {
            std::cout << "There was an error loading the sound " << name << std::endl;
        } else {
            static const int32_t buffer_size = 64 * 1024;
            sound_buffer_count = (sound_length + buffer_size - 1) / buffer_size;
            std::cout << "generating " << sound_buffer_count << "buffers" << std::endl;
            
            alGenBuffers(sound_buffer_count, sound_buffers);

            int32_t format;
            if(channels == 1)
                format = AL_FORMAT_MONO16;
            else if(channels == 2)
                format = AL_FORMAT_STEREO16;
            else
                assert(false && "Invalid channel format");
            
            for (int32_t i = 0; i < sound_buffer_count; i++) {
                alBufferData (sound_buffers[i], format, sound_data + (buffer_size * i), (i + 1 == sound_buffer_count ? sound_length - (buffer_size * (sound_buffer_count - 1)) : buffer_size) * sizeof(int16_t), sample_rate);
            }
            
            
            sound_map[name.key] = this;
        }
        
        status = READY;
    }
    
    void Sound::Unload() {
        alDeleteBuffers(sound_buffer_count, sound_buffers);
        status = UNLOADED;
    }
    
    Sound* Sound::Find (name_t name) {
        auto sound = sound_map.find(name.key);
        
        if (sound != sound_map.end()) {
            return sound->second;
        } else {
            std::cout << "Can't find the sound " << name << ", aborting." << std::endl;
            abort();
        }
    }
    
}

namespace Core {
    using namespace Core::Audio;
    
    template <> Pool<AudioComponent> PoolProxy<AudioComponent>::pool("audio component pool", 150);
    
    AudioComponent::~AudioComponent() {
        alDeleteSources(1, &source);
        is_ready = false;
    }
    
    void AudioComponent::Start() {        
        alGenSources(1, &source);
        alSourcef(source, AL_PITCH, 1);
        alSourcef(source, AL_GAIN, 1.0f);
        alSource3f(source, AL_POSITION, location.x, location.y, location.z);
        alSource3f(source, AL_VELOCITY, 0, 0, 0);
        alSourcei(source, AL_LOOPING, repeat ? AL_TRUE : AL_FALSE);
        if (sound->sound_buffer_count == 1) {
            alSourcei(source, AL_BUFFER, sound->sound_buffers[0]);
        } else {
            alSourceQueueBuffers(source, sound->sound_buffer_count, sound->sound_buffers);
        }
        
        is_ready = true;
    }
    
    void AudioComponent::UpdateLocation(const glm::vec3& location) {
        if (is_ready) alSource3f(source, AL_POSITION, location.x, location.y, location.z);
        this->location = location;
    }
    
    void AudioComponent::SetRepeating(bool is_repeating) {
        if (is_ready) alSourcei(source, AL_LOOPING, is_repeating ? AL_TRUE : AL_FALSE);
        repeat = is_repeating;
    }
    
    void AudioComponent::Play() {
        if (is_ready) alSourcePlay(source);
    }
    
    void AudioComponent::Pause() {
        if (is_ready) alSourcePause(source);
    }
    
    void AudioComponent::Stop() {
        if (is_ready) alSourceStop(source);
    }
    
    bool AudioComponent::IsPlaying() {
        int32_t state;
        alGetSourcei (source, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }
}