// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_AUDIO_SPATIAL_OUTPUT_H
#define TRAM_SDK_AUDIO_SPATIAL_OUTPUT_H

#include <audio/spatial/definitions.h>

#include <vector>

namespace tram::Audio::Spatial {

void InitOutput();
void UpdateOutput();
void UninitOutput();

void LockRenderlist();
void UnlockRenderlist();

}

#endif // TRAM_SDK_AUDIO_SPATIAL_OUTPUT_H