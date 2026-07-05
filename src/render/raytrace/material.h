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
    int assembly_index = -1;
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

inline vec4 GetColorFromCoordinate(RTTexture* texture, ivec2 coords) {
    coords.x = coords.x % texture->width;
    coords.y = coords.y % texture->height;
    
    if (coords.x < 0) coords.x += texture->width;
    if (coords.y < 0) coords.y += texture->height;
    
    int offset = (coords.y * texture->width + coords.x);

    return texture->pixels[offset];
}

inline vec4 SampleTexture(RTTexture* texture, vec2 coords) {
    if (texture->mode == RT_BLENDED) {
        coords.x = coords.x * (float)texture->width;
        coords.y = coords.y * (float)texture->height;
        
        int lo_x = floorf(coords.x);
        int lo_y = floorf(coords.y);
        
        int hi_x = lo_x + 1;
        int hi_y = lo_y + 1;
        
        vec4 color_lo_lo = GetColorFromCoordinate(texture, {lo_x, lo_y});
        vec4 color_lo_hi = GetColorFromCoordinate(texture, {lo_x, hi_y});
        vec4 color_hi_lo = GetColorFromCoordinate(texture, {hi_x, lo_y});
        vec4 color_hi_hi = GetColorFromCoordinate(texture, {hi_x, hi_y});
        
        vec4 color_lo = glm::mix(color_lo_lo, color_lo_hi, coords.y - lo_y);
        vec4 color_hi = glm::mix(color_hi_lo, color_hi_hi, coords.y - lo_y);
        
        return glm::mix(color_lo, color_hi, coords.x - lo_x);
    }
    
    coords.x = coords.x * (float)texture->width;
    coords.y = coords.y * (float)texture->height;
    
    return GetColorFromCoordinate(texture, coords);
}

}

#endif //TRAM_SDK_RENDER_RAYTRACE_MATERIAl_H