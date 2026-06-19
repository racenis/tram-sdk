// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_ERROR_H
#define TRAM_SDK_RENDER_ERROR_H

#include <render/vertices.h>

namespace tram::Render {

uint8_t* MakeNewErrorTexture(vec3 color, vec3 color2);
uint8_t* GetDebugFont();
uint8_t* GetDebugIcon();

StaticModelData* MakeNewErrorModel();
    
}

#endif // TRAM_SDK_RENDER_ERROR_H