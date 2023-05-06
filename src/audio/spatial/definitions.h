// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_AUDIO_SPATIAL_DEFINITIONS_H
#define TRAM_SDK_AUDIO_SPATIAL_DEFINITIONS_H

#include <framework/math.h>

#include <templates/spinlock.h>

namespace tram::Audio::Spatial {

const size_t PATHS_FOR_SOURCE = 100;
const size_t BUFFER_COUNT = 1000;
const size_t SOURCE_COUNT = 200;

// Cached results for path tracing
struct PathTracingResult {
    float force;
    float distance;
    vec3 arrival_direction;
    vec3 sampling_direction;
};

// Cached results for audio rendering
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
    PathTracingResult* paths;
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