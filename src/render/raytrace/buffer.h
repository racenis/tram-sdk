// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_RAYTRACE_BUFFER_H
#define TRAM_SDK_RENDER_RAYTRACE_BUFFER_H

#include <render/render.h>
#include <render/renderer.h>

namespace tram::Render::API {

enum RTVertexType {
    RT_STATIC_LIGHTMAPPED,
    RT_DYNAMIC_BLENDED,
    RT_STATIC_COLORED,
    RT_SPRITE
};

struct FFPHelper {
    int position = -1;
    int normal = -1;
    int color = -1;
    int texture = -1;
    int lightmap = -1;
    int bone_index = -1;
    int bone_weight = -1;
};

struct RTVertexArray {
    RTVertexType type = RT_STATIC_LIGHTMAPPED;
    void* vertices = nullptr;
    size_t vertex_count = 0;
    VertexDefinition format;
    FFPHelper helper;
};

struct RTIndexArray {
    size_t index_count = 0;
    uint32_t* indices = nullptr;
#ifndef __x86_64__
    void* padding;
#endif
};

struct StaticVertex {
    vec3 pos;
    vec3 nrm;
    vec2 tex;
    vec2 lit;
};

struct DynamicVertex {
    vec3 pos;
    vec3 nrm;
    vec2 tex;
    vec4 wgt;
    ivec4 ind;
};

struct ColorVertex {
    vec3 pos;
    vec3 col;
};

}

#endif //TRAM_SDK_RENDER_RAYTRACE_BUFFER_H