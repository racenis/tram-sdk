#include <audio/api.h>

#include <framework/logging.h>
#include <framework/system.h>

#ifdef __EMSCRIPTEN__
    #include <AL/alc.h>
    #include <AL/al.h>
#else
    #include <openal/alc.h>
    #include <openal/al.h>
#endif


namespace tram::Audio::API {

static ALCdevice* sound_device = nullptr;
static ALCcontext* sound_context = nullptr;

void Init() {
    sound_device = alcOpenDevice(nullptr);
    if (!sound_device) Log(SEVERITY_ERROR, System::SYSTEM_AUDIO, "Audio device didn't open!");
    sound_context = alcCreateContext(sound_device, nullptr);
    if (!sound_context) Log(SEVERITY_ERROR, System::SYSTEM_AUDIO, "Audio context didn't create!");
    if (!alcMakeContextCurrent(sound_context)) Log(SEVERITY_ERROR, System::SYSTEM_AUDIO, "Audio context didn't get currented!");
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

/// Sets the listener position.
/// @param position Listener position. Under normal circumstances, it should be
///                 the same as render view position.
void SetListenerPosition(vec3 position) {
    alListener3f(AL_POSITION, position.x, position.y, position.z);
}

/// Sets the listener orientation.
/// @param orientation  Listener orientation. Under normal circumstances, it should be
///                     the same as render view rotation.
void SetListenerOrientation(quat orientation) {
    vec3 combined_orientation[2] = {
        orientation * DIRECTION_FORWARD, 
        DIRECTION_UP
    };
    
    alListenerfv(AL_ORIENTATION, &combined_orientation[0][0]);
}

/// Creates audio buffers from raw audio data.
/// @param audio_data       Pointer to the raw audio data, stored as 16 bit PCM.
/// @param length           Length of the raw audio data, in samples.
/// @param buffer_count     Created buffer count will be stored in this variable.
audiobuffer_t MakeAudioBuffer(const int16_t* audio_data, int32_t length, int32_t samples, int32_t channels) {
    static const int32_t buffer_size = 64 * 1024;
    int32_t buffer_count = (length + buffer_size - 1) / buffer_size;
    audiobuffer_t generated_buffer = {.al_buffers = new uint32_t[buffer_count],
                                      .al_buffer_count = buffer_count};
                                      
    Log(SEVERITY_INFO, System::SYSTEM_AUDIO, "Generating {} buffers", buffer_count);

    alGenBuffers(buffer_count, generated_buffer.al_buffers);

    int32_t format;
    switch (channels) {
        case 1: format = AL_FORMAT_MONO16;      break;
        case 2: format = AL_FORMAT_STEREO16;    break;
        default: assert(false && "Invalid channel format"); break;
    }

    for (int32_t i = 0; i < buffer_count; i++) {
        alBufferData (
            generated_buffer.al_buffers[i], 
            format, 
            audio_data + (buffer_size * i), 
            (i + 1 == buffer_count ? length - (buffer_size * (buffer_count - 1)) : buffer_size) * sizeof(int16_t),
            samples
        );
    }
    
    return generated_buffer;
}

/// Deletes audio buffers.
void RemoveAudioBuffer(audiobuffer_t buffer) {
    alDeleteBuffers(buffer.al_buffer_count, buffer.al_buffers);
    delete[] buffer.al_buffers;
}

/// Creates an audio source.
/// @return A new audio source handle.
audiosource_t MakeAudioSource() {
    audiosource_t source;
    alGenSources(1, &source.al_source);
    return source;
}

/// Sets the pitch of an audio source.
/// @param pitch    1.0f is the regular pitch. Halving the value decreases the
///                 pitch by an octave. Doubling the value increases the pitch
///                 by an octave.
void SetAudioSourcePitch (audiosource_t source, float pitch) {
    alSourcef(source.al_source, AL_PITCH, pitch);
}

/// Sets the gain of an audio source.
/// @param gain     1.0f is the regular gain. Halving the value decreases the
///                 gain by 6dB.
void SetAudioSourceGain (audiosource_t source, float gain) {
    alSourcef(source.al_source, AL_GAIN, gain);
}

/// Sets the position of an audio source.
void SetAudioSourcePosition (audiosource_t source, vec3 position) {
    alSource3f(source.al_source, AL_POSITION, position.x, position.y, position.z);
}

/// Sets the velocity of an audio source.
void SetAudioSourceVelocity (audiosource_t source, vec3 velocity) {
    alSource3f(source.al_source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

/// Sets the repetition of an audio source.
/// @param repeating    True if the sound repeats after playing, false if
///                     it just stops.
void SetAudioSourceRepeating (audiosource_t source, bool repeating) {
    alSourcei(source.al_source, AL_LOOPING, repeating ? AL_TRUE : AL_FALSE);
}

/// Sets the buffers that the audio source will play.
void SetAudioSourceBuffer(audiosource_t source, audiobuffer_t buffer) {
    if (buffer.al_buffer_count == 1) {
        alSourcei(source.al_source, AL_BUFFER, *buffer.al_buffers);
    } else {
        alSourceQueueBuffers(source.al_source, buffer.al_buffer_count, buffer.al_buffers);
    }
}

/// Plays an audio source.
void PlayAudioSource (audiosource_t source) {
    alSourcePlay(source.al_source);
}

/// Pauses the audio source.
void PauseAudioSource (audiosource_t source) {
    alSourcePause(source.al_source);
}

/// Stops the audio source.
void StopAudioSource (audiosource_t source) {
    alSourceStop(source.al_source);
}

/// Checks if an audio source is playing.
/// @return True if the audio source is playing a sound, false otherwise.
bool IsAudioSourcePlaying (audiosource_t source) {
    int32_t state;
    alGetSourcei (source.al_source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}

/// Deletes an audio source.
/// If you do this, then the audio source handle will become invalid.
/// Call MakeAudioSource() to get a new one.
void RemoveAudioSource (audiosource_t source) {
    alDeleteSources(1, &source.al_source);
}

    
}