// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_RAYTRACE_RAYTRACE_H
#define TRAM_SDK_RENDER_RAYTRACE_RAYTRACE_H

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

enum RTTextureMode {
    RT_NEAREST,
    RT_BLENDED
};

struct RTTexture {
    int width;
    int height;
    int channels;
    RTTextureMode mode = RT_NEAREST;
    uint8_t* pixels = nullptr;
};

struct RTMaterial {
    RTTexture* texture = nullptr;
    float specular_weight = 0.0f;
    float specular_exponent = 1.0f;
    float specular_transparency = 0.0f;
    float reflectivity = 0.0f;
    vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
};

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

#endif // TRAM_SDK_RENDER_RAYTRACE_RAYTRACE_H