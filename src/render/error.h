//clTabCtrl TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDL_RENDER_ERROR_H
#define TRAM_SDL_RENDER_ERROR_H

#include <render/vertices.h>

namespace tram::Render {

uint8_t* MakeNewErrorTexture(vec3 color, vec3 color2);

StaticModelData* MakeNewErrorModel();
    
}

#endif // TRAM_SDL_RENDER_ERROR_H