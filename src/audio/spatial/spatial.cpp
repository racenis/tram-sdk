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
AudioRender audiorenders[200];

static std::vector<uint32_t> audiobuffer_free_list;
static std::vector<uint32_t> audiosource_free_list;

static size_t last_audiobuffer = 0;
static size_t last_audiosource = 0;

vec3 listener_position = {0.0f, 0.0f, 0.0f};
quat listener_orientation = {1.0f, 0.0f, 0.0f, 0.0f};

void Init() {
    for (auto& source : audiorenders) {
        source.flags = 0; // just in case; don't want renderer to render uninitialized sources
    }
    
    InitOutput();
}

void Update() {
    for (size_t i = 0; i < SOURCE_COUNT; i++) {
        if (~audiorenders[i].flags & SOURCE_PLAYING) continue;
        
        Render::AddLineMarker(audiosources[i].position, Render::COLOR_BLUE);
        
        std::vector<PathDirection> paths;
        paths.reserve(10);
        
        FindPaths(paths, audiosources[i].position);
        
        //if (paths.size()) std::cout << "found paths: " << paths.size() << std::endl;
        
        for (size_t k = 0; k < PATHS_FOR_SOURCE; k++) {
            Render::AddLine(listener_position, listener_position - (audiosources[i].paths[k].force * audiosources[i].paths[k].direction * 4.0f), Render::COLOR_PINK);
        }
        
        for (auto& path : paths) {
            if (audiosources[i].last_path >= PATHS_FOR_SOURCE) {
                audiosources[i].last_path = 0;
                //std::cout << "wraparpond" << std::endl;
            }
            
            audiosources[i].paths[audiosources[i].last_path] = path;
            audiosources[i].last_path++;
        }
        
        // copy path trace results into renderer
        for (size_t k = 0; k < PATHS_FOR_SOURCE; k++) {
            //audiorenders[i].paths[k].distance = audiosources[i].paths[k].distance;
            //audiorenders[i].paths[k].distance = 0.0f;
            //audiosources[i].paths[k].distance = 0.0f;
            
            //audiorenders[i].paths[k].direction = audiosources[i].paths[k].direction;
            
            
            float panning = glm::dot(audiosources[i].paths[k].direction, listener_orientation * DIRECTION_SIDE);
            int32_t panning_delay = panning * 20.0f; // 20 sample between ears
            
            float delay = audiosources[i].paths[k].distance / 331.0f; // 331 m/s sound velocity
            int32_t distance_delay = delay * -44100.0f; // 44100 hz sample rate
            
            
            audiorenders[i].paths[k].force = audiosources[i].paths[k].force;
            audiorenders[i].paths[k].panning = panning;
            audiorenders[i].paths[k].panning_delay = panning_delay;
            audiorenders[i].paths[k].distance_delay = distance_delay;
            
        }
        
        /*if (paths.size() == 0) {
            if (source.last_path >= PATHS_FOR_SOURCE) {
                source.last_path = 0;
            }
            
            source.paths[source.last_path].force *= 0.2f;
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
    auto& render = audiorenders[source_index];
    
    source.position = {0.0f, 0.0f, 0.0f};
    source.last_path = 0;
    
    source.paths = new PathDirection[PATHS_FOR_SOURCE];
    
    render.flags = 0;
    render.sample = 0;
    render.buffer = nullptr;
    
    render.paths = new PathResult[PATHS_FOR_SOURCE];
    
    
    for (size_t i = 0; i < PATHS_FOR_SOURCE; i++) {
        source.paths[i].direction = {0.0f, 1.0f, 0.0f};
        source.paths[i].distance = 0.0f;
        source.paths[i].force = 0.0f;
        
        render.paths[i].force = 0.0f;
        render.paths[i].panning = 0.0f;
        render.paths[i].panning_delay = 0;
        render.paths[i].distance_delay = 0;
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
        audiorenders[source].flags |= SOURCE_REPEATING;
    } else {
        audiorenders[source].flags &= ~SOURCE_REPEATING;
    }
}

void SetAudioSourceBuffer(audiosource_t source, const audiobuffer_t* buffers, int32_t buffer_count) {
    audiorenders[source].buffer = &audiobuffers[(uint32_t) ((uint64_t) buffers)];
}

void PlayAudioSource (audiosource_t source) {
    audiorenders[source].flags |= SOURCE_PLAYING;
}

void PauseAudioSource (audiosource_t source) {
    audiorenders[source].flags &= ~SOURCE_PLAYING;
}

void StopAudioSource (audiosource_t source) {
    audiorenders[source].flags &= ~SOURCE_PLAYING;
    audiorenders[source].sample = 0;
}

bool IsAudioSourcePlaying (audiosource_t source) {
    return audiorenders[source].flags & SOURCE_PLAYING;
}

void RemoveAudioSource (audiosource_t source) {
    delete[] audiosources[source].paths;
    delete[] audiorenders[source].paths;
    
    audiosources[source].paths = nullptr;
    
    // nulling flags prevents the renderer from playing removed source
    audiorenders[source].flags = 0;
    audiorenders[source].buffer = nullptr;
    audiorenders[source].paths = nullptr;

    audiosource_free_list.push_back(source);
}

}