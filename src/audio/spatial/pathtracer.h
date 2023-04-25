// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_AUDIO_SPATIAL_PATHTRACER_H
#define TRAM_SDK_AUDIO_SPATIAL_PATHTRACER_H

#include <audio/audio.h>

#include <vector>

namespace tram::Audio::Spatial {
    struct PathResult {
        float force;
        float distance;
        vec3 direction;
    };
    
    void FindPaths(std::vector<PathResult>& paths, vec3 position);
    
    extern vec3 listener_position;
    extern quat listener_orientation;
}

#endif // TRAM_SDK_AUDIO_SPATIAL_PATHTRACER_H