// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#ifndef TRAM_SDK_AUDIO_SPATIAL_PATHTRACER_H
#define TRAM_SDK_AUDIO_SPATIAL_PATHTRACER_H

#include <audio/spatial/definitions.h>

#include <vector>

namespace tram::Audio::Spatial {

//void FindPaths(std::vector<PathTracingResult>& paths, vec3 position);

void FindPaths(PathExplorationResult& result, bool metropolis, vec3 position);

void FindPathsMetropolis(PathExplorationResult* exploration, PathTracingResult* result, vec3 position);

void InitExplorationPaths(PathExplorationResult* paths);

void ValidateResult(PathTracingResult& result, vec3 position);
void RenderResult(PathTracingResult& result, vec3 position);

void CheckReverb(PathExplorationResult* paths);

void MakeSomeSourcePaths(PathFromAudioSource& path, vec3 source_position);
}

#endif // TRAM_SDK_AUDIO_SPATIAL_PATHTRACER_H