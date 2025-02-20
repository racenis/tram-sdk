// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_OPENGL_LIGHT_H
#define TRAM_SDK_RENDER_OPENGL_LIGHT_H

#include <render/render.h>
#include <render/renderer.h>

namespace tram::Render::API {

struct GLLight {
    vec3 location = {0.0f, 0.0f, 0.0f};
    float padding;
    vec3 color = {0.0f, 0.0f, 0.0f};
    float distance = 0.0f;
    vec3 direction = {0.0f, 0.0f, -1.0f};
    float exponent = 0.0f;
    float padding2[4];
};

}

#endif // TRAM_SDK_RENDER_OPENGL_LIGHT_H