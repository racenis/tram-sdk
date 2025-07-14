// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_RAYTRACE_MATH_H
#define TRAM_SDK_RENDER_RAYTRACE_MATH_H

#include <render/render.h>
#include <render/renderer.h>

namespace tram::Render::API {

inline uint16_t IntColor(vec3 color) {
    return (((uint16_t)(31.0f * color.r)) << 11)
         | (((uint16_t)(63.0f * color.g)) << 5)
         | (((uint16_t)(31.0f * color.b)));
}

inline vec3 RGBColor(uint16_t color) {
    float r = (color & 0xf800) >> 11;
    float g = (color & 0x07e0) >> 5;
    float b = color & 0x001f;
    
    return {r/31.0f, g/63.0f, b/31.0f};
}

inline void BlitLineOctant0(uint32_t x0, uint32_t y0, uint32_t delta_x, uint32_t delta_y, int32_t direction, auto blit_func) {
    int32_t delta_y2 = delta_y * 2;
    int32_t delta_y2_x2 = delta_y2 - (int32_t) ( delta_x * 2 );
    int32_t error = delta_y2 - (int32_t) delta_x;

    blit_func(x0, y0);

    while (delta_x--) {
        if (error >= 0) {
            y0++;
            error += delta_y2_x2;
        } else {
            error += delta_y2;
        }
        
        x0 += direction;
        
        blit_func(x0, y0);
    }
}

inline void BlitLineOctant1(uint32_t x0, uint32_t y0, uint32_t delta_x, uint32_t delta_y, int32_t direction, auto blit_func) {
    int32_t delta_x2 = delta_x * 2;
    int32_t delta_x2_y2 = delta_x2 - (int32_t) ( delta_y * 2 );
    int32_t error = delta_x2 - (int32_t) delta_y;

    blit_func(x0, y0);
    
    while (delta_y--) {
        if (error >= 0) {
            x0 += direction;
            error += delta_x2_y2;
        } else {
            error += delta_x2;
        }
        
        y0++;
        
        blit_func(x0, y0);
    }
}

inline void BlitLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, auto blit_func) {
    
    // for some reason some vertices get a NaN bone weight?? and this crashes?
    // it seems to happen for vertices where all bones have a zero weight, which
    // seems to indicate some sort of a normalization error, but who knows
    // TODO: fix this hack
    if (x0 < 0 || y0 < 0 || x1 < 0 || y1 < 0) return;
    if (x0 > 1000 || y0 > 1000 || x1 > 1000 || y1 > 1000) return;
    
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
    }

    int32_t delta_x = x1 - x0;
    int32_t delta_y = y1 - y0;
    
    if (delta_x > 0) {
        if (delta_x > delta_y) {
            BlitLineOctant0(x0, y0, delta_x, delta_y, 1, blit_func);
        } else {
            BlitLineOctant1(x0, y0, delta_x, delta_y, 1, blit_func);
        }
    } else {
        delta_x = -delta_x;
        if (delta_x > delta_y) {
            BlitLineOctant0(x0, y0, delta_x, delta_y, -1, blit_func);
        } else {
            BlitLineOctant1(x0, y0, delta_x, delta_y, -1, blit_func);
        }
    }
}
 
inline void ClipLineInClipSpace(vec4& point0, vec4& point1) {
    vec4 p0 = point0;
    vec4 p1 = point1;
    
    // clip against left plane
    if (p0.w + p0.x < 0.0f) {
        float a = (p0.w + p0.x) / ((p0.w + p0.x) - (p1.w + p1.x));
        p0 = (1.0f - a) * p0 + a * p1;
    }
    if (p1.w + p1.x < 0.0f) {
        float a = (p1.w + p1.x) / ((p1.w + p1.x) - (p0.w + p0.x));
        p1 = (1.0f - a) * p1 + a * p0;
    }
    
    // clip against right plane
    if (p0.w - p0.x < 0.0f) {
        float a = (p0.w - p0.x) / ((p0.w - p0.x) - (p1.w - p1.x));
        p0 = (1.0f - a) * p0 + a * p1;
    }
    if (p1.w - p1.x < 0.0f) {
        float a = (p1.w - p1.x) / ((p1.w - p1.x) - (p0.w - p0.x));
        p1 = (1.0f - a) * p1 + a * p0;
    }
    
    // clip against bottom plane
    if (p0.w + p0.y < 0.0f) {
        float a = (p0.w + p0.y) / ((p0.w + p0.y) - (p1.w + p1.y));
        p0 = (1.0f - a) * p0 + a * p1;
    }
    if (p1.w + p1.y < 0.0f) {
        float a = (p1.w + p1.y) / ((p1.w + p1.y) - (p0.w + p0.y));
        p1 = (1.0f - a) * p1 + a * p0;
    }
    
    // clip against top plane
    if (p0.w - p0.y < 0.0f) {
        float a = (p0.w - p0.y) / ((p0.w - p0.y) - (p1.w - p1.y));
        p0 = (1.0f - a) * p0 + a * p1;
    }
    if (p1.w - p1.y < 0.0f) {
        float a = (p1.w - p1.y) / ((p1.w - p1.y) - (p0.w - p0.y));
        p1 = (1.0f - a) * p1 + a * p0;
    }
    
    // clip against near plane
    if (p0.w + p0.z < 0.0f) {
        float a = (p0.w + p0.z) / ((p0.w + p0.z) - (p1.w + p1.z));
        p0 = (1.0f - a) * p0 + a * p1;
    }
    if (p1.w + p1.z < 0.0f) {
        float a = (p1.w + p1.z) / ((p1.w + p1.z) - (p0.w + p0.z));
        p1 = (1.0f - a) * p1 + a * p0;
    }
    
    // clip against far plane
    if (p0.w - p0.z < 0.0f) {
        float a = (p0.w - p0.z) / ((p0.w - p0.z) - (p1.w - p1.z));
        p0 = (1.0f - a) * p0 + a * p1;
    }
    if (p1.w - p1.z < 0.0f) {
        float a = (p1.w - p1.z) / ((p1.w - p1.z) - (p0.w - p0.z));
        p1 = (1.0f - a) * p1 + a * p0;
    }
    
    point0 = p0;
    point1 = p1;
}

inline void PerspectiveDivision(vec4& p0, vec4& p1) {
    if (p0.w != 0.0f) {
        p0.x /= p0.w;
        p0.y /= p0.w;
    }
    
    if (p1.w != 0.0f) {
        p1.x /= p1.w;
        p1.y /= p1.w;
    }
}

inline bool LineVisible(vec4 p0, vec4 p1) {    
    // line outside left plane
    if (p0.w + p0.x < 0.0f && p1.w + p1.x < 0.0f) {
        return false;
    }

    // line outside right plane
    if (p0.w - p0.x < 0.0f && p1.w - p1.x < 0.0f) {
        return false;
    }

    // line outside bottom plane
    if (p0.w + p0.y < 0.0f && p1.w + p1.y < 0.0f) {
        return false;
    }

    // line outside top plane
    if (p0.w - p0.y < 0.0f && p1.w - p1.y < 0.0f) {
        return false;
    }

    // line outside near plane
    if (p0.w + p0.z < 0.0f && p1.w + p1.z < 0.0f) {
        return false;
    }
    
    // line outside far plane
    if (p0.w - p0.z < 0.0f && p1.w - p1.z < 0.0f) {
        return false;
    }
    
    return true;
}

inline vec3 GetBarycentric(vec3 a, vec3 b, vec3 c, vec3 p) {
    const vec3 v1 = b - a;
    const vec3 v2 = c - a; 
    const vec3 v3 = p - a;
    
    const float d1 = glm::dot(v1, v1);
    const float d2 = glm::dot(v1, v2);
    const float d3 = glm::dot(v2, v2);
    const float d4 = glm::dot(v3, v1);
    const float d5 = glm::dot(v3, v2);
    
    const float dominator = d1 * d3 - d2 * d2;
    
    const float v = (d3 * d4 - d2 * d5) / dominator;
    const float w = (d1 * d5 - d2 * d4) / dominator;
    const float u = 1.0f - v - w;
    
    return {u, v, w};
}

}

#endif //TRAM_SDK_RENDER_RAYTRACE_MATH_H