#include <audio/spatial/spatial.h>

#include <audio/spatial/definitions.h>
#include <audio/spatial/pathtracer.h>
#include <audio/spatial/output.h>

#include <render/render.h>

#include <vector>
#include <atomic>

namespace tram::Audio::Spatial {

AudioBuffer audiobuffers[1000];
AudioSource audiosources[200];

static std::vector<uint32_t> audiobuffer_free_list;
static std::vector<uint32_t> audiosource_free_list;

static size_t last_audiobuffer = 0;
static size_t last_audiosource = 0;

vec3 listener_position = {0.0f, 0.0f, 0.0f};
quat listener_orientation = {1.0f, 0.0f, 0.0f, 0.0f};

void Init() {
    for (auto& source : audiosources) {
        source.flags = 0; // just in case; don't want renderer to render uninitialized sources
    }
    
    InitOutput();
}

void Update() {
    for (auto& source: audiosources) {
        if (~source.flags & SOURCE_PLAYING) continue;
        
        Render::AddLineMarker(source.position, Render::COLOR_BLUE);
        
        std::vector<PathResult> paths;
        paths.reserve(10);
        
        FindPaths(paths, source.position);
        
        //std::cout << "found paths: " << paths.size() << std::endl;
        
        for (auto& path : paths) {
            if (source.last_path >= PATHS_FOR_SOURCE) {
                source.last_path = 0;
                //std::cout << "wraparpond" << std::endl;
            }
            
            source.paths[source.last_path] = path;
            source.last_path++;
        }
        
        /*if (paths.size() == 0) {
            if (source.last_path >= PATHS_FOR_SOURCE) {
                source.last_path = 0;
            }
            
            source.paths[source.last_path].force = 0.0f;
            source.last_path++;
        }*/
    }
    
    UpdateOutput();
}

void Uninit() {
    UninitOutput();
}

void SetListenerPosition(const glm::vec3& position) {
    listener_position = position;
}

void SetListenerOrientation(const glm::quat& orientation) {
    listener_orientation = orientation;
}

audiobuffer_t* MakeAudioBuffer(const int16_t* audio_data, int32_t length, int32_t samples, int32_t channels, int32_t& buffer_count) {
    const float compression = 1.0f / ((65535.0f - 1.0f) / 2.0f); // maps int16 to [-1.0f, 1.0f] 
    
    float* buffer_data = new float[length];
    
    for (int32_t i = 0; i < length; i++) {
        buffer_data[i] = ((float) audio_data[i]) * compression;
    }
    
    
    uint32_t buffer_index;
    
    if (audiobuffer_free_list.size()) {
        buffer_index = audiobuffer_free_list.back();
        audiobuffer_free_list.pop_back();
    } else {
        buffer_index = last_audiobuffer;
        last_audiobuffer++;
    }
    
    auto& buffer = audiobuffers[buffer_index];
    
    buffer.length = length;
    buffer.data = buffer_data;
    buffer.channels = channels;
    
    switch (samples) {
        case 44100:
            buffer.sample_rate = SAMPLERATE_44100;
            break;
        case 22050:
            buffer.sample_rate = SAMPLERATE_22050;
            break;
        case 11025:
            buffer.sample_rate = SAMPLERATE_11025;
            break;
        default:
            buffer.sample_rate = SAMPLERATE_44100;
            std::cout << "Unrecognized sample rate: " << samples << std::endl;
            break;
    }
    
    buffer_count = 1;
    
    // save space by packing id into pointer, since all sounds are always 1 buffer
    return (audiobuffer_t*) ((uint64_t) buffer_index);
}

void RemoveAudioBuffer(audiobuffer_t* buffers, int32_t buffer_count) {
    uint32_t buffer_index = (uint32_t) ((uint64_t) buffers);
    
    delete[] audiobuffers[buffer_index].data;
    audiobuffers[buffer_index].data = nullptr;
    
    audiosource_free_list.push_back(buffer_index);
}

audiosource_t MakeAudioSource() {    
    audiosource_t source_index;
    
    if (audiosource_free_list.size()) {
        source_index = audiosource_free_list.back();
        audiosource_free_list.pop_back();
    } else {
        source_index = last_audiosource;
        last_audiosource++;
    }
    
    auto& source = audiosources[source_index];
    
    source.flags = 0;
    source.sample = 0;
    source.position = {0.0f, 0.0f, 0.0f};
    source.buffer = nullptr;
    source.paths = new PathResult[PATHS_FOR_SOURCE];
    source.last_path = 0;
    
    for (size_t i = 0; i < PATHS_FOR_SOURCE; i++) {
        source.paths[i].distance = 0.0f;
        source.paths[i].direction = {0.0f, 1.0f, 0.0f};
        source.paths[i].force = 0.0f;
    }
    
    return source_index;
}

void SetAudioSourcePitch (audiosource_t source, float pitch) {
    // not implemented
}

void SetAudioSourceGain (audiosource_t source, float gain) {
    // not implemented
}

void SetAudioSourcePosition (audiosource_t source, vec3 position) {
    audiosources[source].position = position;
}

void SetAudioSourceVelocity (audiosource_t source, vec3 velocity) {
    // not implemented
}

void SetAudioSourceRepeating (audiosource_t source, bool repeating) {
    if (repeating) {
        audiosources[source].flags |= SOURCE_REPEATING;
    } else {
        audiosources[source].flags &= ~SOURCE_REPEATING;
    }
}

void SetAudioSourceBuffer(audiosource_t source, const audiobuffer_t* buffers, int32_t buffer_count) {
    audiosources[source].buffer = &audiobuffers[(uint32_t) ((uint64_t) buffers)];
}

void PlayAudioSource (audiosource_t source) {
    audiosources[source].flags |= SOURCE_PLAYING;
}

void PauseAudioSource (audiosource_t source) {
    audiosources[source].flags &= ~SOURCE_PLAYING;
}

void StopAudioSource (audiosource_t source) {
    audiosources[source].flags &= ~SOURCE_PLAYING;
    audiosources[source].sample = 0;
}

bool IsAudioSourcePlaying (audiosource_t source) {
    return audiosources[source].flags & SOURCE_PLAYING;
}

void RemoveAudioSource (audiosource_t source) {
    delete[] audiosources[source].paths;
    
    audiosources[source].flags = 0; 
    audiosources[source].buffer = nullptr;
    audiosources[source].paths = nullptr;

    audiosource_free_list.push_back(source);
}

}