// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_RAYTRACE_MATERIAl_H
#define TRAM_SDK_RENDER_RAYTRACE_MATERIAl_H

#include <render/render.h>
#include <render/renderer.h>

namespace tram::Render::API {

enum RTTextureMode {
    RT_NEAREST,
    RT_BLENDED
};

struct RTTexture {
    int width;
    int height;
    int channels;
    RTTextureMode mode = RT_NEAREST;
    vec4* pixels = nullptr;
};

struct RTMaterial {
    RTTexture* texture = nullptr;
    RTTexture* normal_map = nullptr;
    float specular_weight = 0.0f;
    float specular_exponent = 1.0f;
    float specular_transparency = 0.0f;
    float reflectivity = 0.0f;
    vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
};

inline vec4 SampleTexture(RTTexture* texture, vec2 coords) {
    int tex_x = coords.x * (float)texture->width;
    int tex_y = coords.y * (float)texture->height;
    
    tex_x = tex_x % texture->width;
    tex_y = tex_y % texture->height;
    
    if (tex_x < 0) tex_x += texture->width;
    if (tex_y < 0) tex_y += texture->height;
    
    int offset = (tex_y * texture->width + tex_x);

    return texture->pixels[offset];
}

}

#endif //TRAM_SDK_RENDER_RAYTRACE_MATERIAl_H