// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_AUDIO_SPATIAL_OUTPUT_H
#define TRAM_SDK_AUDIO_SPATIAL_OUTPUT_H

#include <audio/audio.h>

#include <vector>

namespace tram::Audio::Spatial {

extern size_t next_buffer;
extern float output_buffer[4][4096];

void InitOutput();
void UpdateOutput();
void UninitOutput();

}

#endif // TRAM_SDK_AUDIO_SPATIAL_OUTPUT_H