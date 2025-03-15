// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_OPENGL_MATERIAL_H
#define TRAM_SDK_RENDER_OPENGL_MATERIAL_H

#include <render/render.h>
#include <render/renderer.h>

namespace tram::Render::API {

struct GLMaterial {
    uint32_t gl_texture = 0;
    float specular_weight = 0.0f;
    float specular_exponent = 1.0f;
    float specular_transparency = 0.0f;
    float reflectivity = 0.0f;
    vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
};

}

#endif // TRAM_SDK_RENDER_OPENGL_MATERIAL_H