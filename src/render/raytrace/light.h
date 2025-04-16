// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_RAYTRACE_LIGHT_H
#define TRAM_SDK_RENDER_RAYTRACE_LIGHT_H

#include <render/render.h>
#include <render/renderer.h>

namespace tram::Render::API {

struct RTLight {
    vec3 location = {0.0f, 0.0f, 0.0f};
    vec3 color = {0.0f, 0.0f, 0.0f};
    float distance = 0.0f;
    vec3 direction = {0.0f, 0.0f, -1.0f};
    float exponent = 0.0f;
};


}

#endif //TRAM_SDK_RENDER_RAYTRACE_LIGHT_H