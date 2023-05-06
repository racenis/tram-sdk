// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_AUDIO_SPATIAL_PATHTRACER_H
#define TRAM_SDK_AUDIO_SPATIAL_PATHTRACER_H

#include <audio/spatial/definitions.h>

#include <vector>

namespace tram::Audio::Spatial {

void FindPaths(std::vector<PathTracingResult>& paths, vec3 position);

}

#endif // TRAM_SDK_AUDIO_SPATIAL_PATHTRACER_H