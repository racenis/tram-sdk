// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_AUDIO_SPATIAL_DEFINITIONS_H
#define TRAM_SDK_AUDIO_SPATIAL_DEFINITIONS_H

#include <framework/math.h>

#include <templates/spinlock.h>

namespace tram::Audio::Spatial {

const size_t PATHS_FOR_RENDERING = 10;
const size_t PATHS_FOR_SOURCE = 5;
const size_t PATHS_FOR_LISTENER = 5;

const size_t BUFFER_COUNT = 1000;
const size_t SOURCE_COUNT = 200;

const size_t SOURCE_DEPTH_LIMIT = 3;
const size_t LISTENER_DEPTH_LIMIT = 6;

// actually this might be useful!!! for both. or not
struct PathExplorationResult {
    float force;
    vec3 sampling_direction;
    uint32_t cycles_since_last_hit;
};

// this is the successful path that might need to be revalidated
struct PathTracingResult {
    float force;
    float distance;
    uint32_t cycles_since_last_hit;
    uint32_t reflection_count;
    vec3 arrival_direction;
    
    float reflection_absorption;
    vec3 listener_ideal_angle;
    vec3 source_ideal_angle;
    
    struct {
        vec3 point;
    } reflections[SOURCE_DEPTH_LIMIT * LISTENER_DEPTH_LIMIT];
};

struct PathFromAudioSource {
    struct {
        vec3 point;
        vec3 direction;
        float force;
    } reflections[LISTENER_DEPTH_LIMIT];
    vec3 source_direction;
};

// this is handed over to the rendering algorithm
struct PathRenderingInfo {
    float force;
    float panning;
    int32_t panning_delay;
    int32_t distance_delay;
};

enum AudioSourceFlags : uint16_t {
    SOURCE_PLAYING = 1,
    SOURCE_REPEATING = 2
};

enum AudioSampleRate : uint16_t {
    SAMPLERATE_44100 = 1,
    SAMPLERATE_22050 = 2,
    SAMPLERATE_11025 = 4
};

struct AudioBuffer {
    AudioSampleRate sample_rate;
    uint32_t channels;
    size_t length;
    float* data;
};

// Audio source information for path tracing
struct AudioSource {
    vec3 position;
    
    PathFromAudioSource* paths; // bidirectional paths
    
    PathTracingResult* result_paths;
    size_t last_path;
};

// Audio source information for rendering
struct AudioRender {
    Spinlock lock;
    uint16_t flags;
    uint32_t sample;
    AudioBuffer* buffer;
    PathRenderingInfo* paths;
};

extern AudioBuffer audiobuffers[BUFFER_COUNT];
extern AudioSource audiosources[SOURCE_COUNT];
extern AudioRender audiorenders[SOURCE_COUNT];

extern vec3 listener_position;
extern quat listener_orientation;

}

#endif // TRAM_SDK_AUDIO_SPATIAL_DEFINITIONS_H