#include <audio/openal/openal.h>

#include <framework/logging.h>
#include <framework/system.h>

#ifdef __EMSCRIPTEN__
    #include <AL/alc.h>
    #include <AL/al.h>
#else
    #include <alc.h>
    #include <al.h>
#endif


namespace tram::Audio::OpenAL {

ALCdevice* sound_device = nullptr;
ALCcontext* sound_context = nullptr;

void Init() {
    sound_device = alcOpenDevice(nullptr);
    if (!sound_device) Log (SEVERITY_ERROR, System::SYSTEM_AUDIO, "Audio device didn't open!");
    sound_context = alcCreateContext(sound_device, nullptr);
    if (!sound_context) Log (SEVERITY_ERROR, System::SYSTEM_AUDIO, "Audio context didn't create!");
    if (!alcMakeContextCurrent(sound_context)) Log (SEVERITY_ERROR, System::SYSTEM_AUDIO, "Audio context didn't get currented!");
    const char* device_name = nullptr;
    if (alcIsExtensionPresent(sound_device, "ALC_ENUMERATE_ALL_EXT")) device_name = alcGetString(sound_device, ALC_ALL_DEVICES_SPECIFIER);
    if (!device_name || alcGetError(sound_device) != ALC_NO_ERROR) device_name = alcGetString(sound_device, ALC_DEVICE_SPECIFIER);
    Log (SEVERITY_INFO, System::SYSTEM_AUDIO, "{}", device_name);
}

void Update() {        
    // haha nothing in here
}

void Uninit() {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(sound_context);
    alcCloseDevice(sound_device);
}

void SetListenerPosition(const glm::vec3& position) {
    alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void SetListenerOrientation(const glm::quat& orientation) {
    vec3 combined_orientation[2] = {
        orientation * DIRECTION_FORWARD, 
        DIRECTION_UP
    };
    
    alListenerfv(AL_ORIENTATION, &combined_orientation[0][0]);
}

audiobuffer_t* MakeAudioBuffer(const int16_t* audio_data, int32_t length, int32_t samples, int32_t channels, int32_t& buffer_count) {
    static const int32_t buffer_size = 64 * 1024;
    buffer_count = (length + buffer_size - 1) / buffer_size;

    Log (SEVERITY_INFO, System::SYSTEM_AUDIO, "Generating {} buffers", buffer_count);

    audiobuffer_t* generated_buffers = new audiobuffer_t[buffer_count];
    alGenBuffers(buffer_count, generated_buffers);

    int32_t format;

    switch (channels) {
        case 1: format = AL_FORMAT_MONO16;      break;
        case 2: format = AL_FORMAT_STEREO16;    break;
        default: assert(false && "Invalid channel format"); break;
    }

    for (int32_t i = 0; i < buffer_count; i++) {
        alBufferData (
            generated_buffers[i], 
            format, 
            audio_data + (buffer_size * i), 
            (i + 1 == buffer_count ? length - (buffer_size * (buffer_count - 1)) : buffer_size) * sizeof(int16_t),
            samples
        );
    }
    
    return generated_buffers;
}

void RemoveAudioBuffer(audiobuffer_t* buffers, int32_t buffer_count) {
    alDeleteBuffers(buffer_count, buffers);
    delete[] buffers;
}

audiosource_t MakeAudioSource() {
    audiosource_t source;
    alGenSources(1, &source);
    return source;
}


void SetAudioSourcePitch (audiosource_t source, float pitch) {
    alSourcef(source, AL_PITCH, pitch);
}


void SetAudioSourceGain (audiosource_t source, float gain) {
    alSourcef(source, AL_GAIN, gain);
}


void SetAudioSourcePosition (audiosource_t source, vec3 position) {
    alSource3f(source, AL_POSITION, position.x, position.y, position.z);
}


void SetAudioSourceVelocity (audiosource_t source, vec3 velocity) {
    alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}


void SetAudioSourceRepeating (audiosource_t source, bool repeating) {
    alSourcei(source, AL_LOOPING, repeating ? AL_TRUE : AL_FALSE);
}


void SetAudioSourceBuffer(audiosource_t source, const audiobuffer_t* buffers, int32_t buffer_count) {
    if (buffer_count == 1) {
        alSourcei(source, AL_BUFFER, *buffers);
    } else {
        alSourceQueueBuffers(source, buffer_count, buffers);
    }
}


void PlayAudioSource (audiosource_t source) {
    alSourcePlay(source);
}


void PauseAudioSource (audiosource_t source) {
    alSourcePause(source);
}


void StopAudioSource (audiosource_t source) {
    alSourceStop(source);
}


bool IsAudioSourcePlaying (audiosource_t source) {
    int32_t state;
    alGetSourcei (source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}


void RemoveAudioSource (audiosource_t source) {
    alDeleteSources(1, &source);
}

    
}