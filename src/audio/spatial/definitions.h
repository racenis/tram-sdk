// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_AUDIO_SPATIAL_DEFINITIONS_H
#define TRAM_SDK_AUDIO_SPATIAL_DEFINITIONS_H

#include <framework/math.h>

#include <templates/spinlock.h>

namespace tram::Audio::Spatial {

const size_t PATHS_FOR_SOURCE = 100;

struct PathResult {
    float force;
    float distance;
    vec3 direction;
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

struct AudioSource {
    Spinlock lock;
    uint16_t flags;
    uint32_t sample;
    vec3 position;
    AudioBuffer* buffer;
    PathResult* paths;
    size_t last_path;
};

extern AudioBuffer audiobuffers[1000];
extern AudioSource audiosources[200];

extern vec3 listener_position;
extern quat listener_orientation;

}

#endif // TRAM_SDK_AUDIO_SPATIAL_DEFINITIONS_H