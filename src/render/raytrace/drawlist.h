// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_RAYTRACE_DRAWLIST_H
#define TRAM_SDK_RENDER_RAYTRACE_DRAWLIST_H

#include <render/render.h>
#include <render/renderer.h>

namespace tram::Render::API {

struct RTDrawListEntry {
    uint32_t flags = 0;

    Pose* pose = nullptr;
    
    mat4 matrix = mat4(1.0f);

    RTLight* lights[4] = {nullptr, nullptr, nullptr, nullptr};
    
    uint32_t layer = 0;
    RTTexture* lightmap = nullptr;
    RTVertexArray* vertex_array = nullptr;
    RTIndexArray* index_array = nullptr;
    uint32_t index_length = 0;
    uint32_t index_offset = 0;
    sphericalharmonic_t harmonic;
    RTMaterial* material = nullptr;
    RTTexture* environmentmap = nullptr;
    vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
    float specular_weight = 1.0f;
    float specular_exponent = 1.0f;
    float specular_transparency = 1.0f;
};

}

#endif //TRAM_SDK_RENDER_RAYTRACE_DRAWLIST_H