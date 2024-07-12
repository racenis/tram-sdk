// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/software/software.h>
#include <render/api.h>

#include <templates/octree.h>

#include <cstdlib>
#include <cstring>

namespace tram::Render::API {

Pool<SWDrawListEntry> draw_list("render list", 500, false);
Pool<SWLight> light_list("light list", 200, false);
Pool<SWTexture> texture_list("texture list", 200, false);
Pool<SWVertexArray> vertex_arrays("vertex_arrays list", 200, false);
Pool<SWIndexArray> index_arrays("index_arrays list", 200, false);
Octree<uint32_t> light_tree;
std::vector<uint32_t> light_tree_ids (200);

struct LayerParameters {
    mat4 projection_matrix = mat4(1.0f);
    mat4 view_matrix = mat4(1.0f);
    vec3 view_position = {1.0f, 1.0f, 1.0f};
    
    vec3 sun_direction = {0.0f, 1.0f, 0.0f};
    vec3 sun_color = {1.0f, 1.0f, 1.0f};
    vec3 ambient_color = {0.3f, 0.3f, 0.3f};
};

static LayerParameters layers[7];

static vec3 screen_clear_color = {0.2f, 0.3f, 0.3f};
static bool clear_screen = true;

static uint16_t screen_width = 800.0f;
static uint16_t screen_height = 600.0f;

static uint16_t* screen_buffer = nullptr;
static uint16_t* depth_buffer = nullptr;

struct Span {
    uint16_t begin;     // x coord on which the span starts
    uint16_t end;       // x coord on which the span ends
    uint16_t begin_p0;  // first point for begin
    uint16_t begin_p1;  // second point for begin
    int32_t begin_adv;  // how far from the first point in begin, mapped to [0x0000, 0xFFFF]
    uint16_t end_p0;    // same as for begin, but for end
    uint16_t end_p1;    // ditto
    int32_t end_adv;    // ditto
};

static Span spans[1000];

void SetLightingParameters (vec3 sun_direction, vec3 sun_color, vec3 ambient_color, uint32_t layer) {
    layers[layer].sun_direction = sun_direction;
    layers[layer].sun_color = sun_color;
    layers[layer].ambient_color = ambient_color;
}

void SetScreenSize(float width, float height) {
    //screen_width = width;
    //screen_height = height;
}

void SetScreenClear (vec3 clear_color, bool clear) {
    clear_screen = clear;
    screen_clear_color = clear_color;
}

uint16_t IntColor(vec3 color) {
    return (((uint16_t)(31.0f * color.r)) << 11)
         | (((uint16_t)(63.0f * color.g)) << 5)
         | (((uint16_t)(31.0f * color.b)));
}

vec3 RGBColor(uint16_t color) {
    float r = (color & 0xf800) >> 11;
    float g = (color & 0x07e0) >> 5;
    float b = color & 0x001f;
    
    return {r/31.0f, g/63.0f, b/31.0f};
}

void BlitDot(uint32_t x, uint32_t y, uint16_t color) {
    if (x < 0 || x >= screen_width) return;
    if (y < 0 || y >= screen_height) return;
    screen_buffer[screen_width * y + x] = color;
}

// TODO: replace color parameter with a blitting function object
void BlitLineOctant0(uint32_t x0, uint32_t y0, uint32_t delta_x, uint32_t delta_y, int32_t direction, uint32_t color) {
    int32_t delta_y2 = delta_y * 2;
    int32_t delta_y2_x2 = delta_y2 - (int32_t) ( delta_x * 2 );
    int32_t error = delta_y2 - (int32_t) delta_x;

    BlitDot(x0, y0, color);

    while (delta_x--) {
        if (error >= 0) {
            y0++;
            error += delta_y2_x2;
        } else {
            error += delta_y2;
        }
        
        x0 += direction;
        
        BlitDot(x0, y0, color);
    }
}

void BlitLineOctant1(uint32_t x0, uint32_t y0, uint32_t delta_x, uint32_t delta_y, int32_t direction, uint32_t color) {
    int32_t delta_x2 = delta_x * 2;
    int32_t delta_x2_y2 = delta_x2 - (int32_t) ( delta_y * 2 );
    int32_t error = delta_x2 - (int32_t) delta_y;

    BlitDot(x0, y0, color);
    
    while (delta_y--) {
        if (error >= 0) {
            x0 += direction;
            error += delta_x2_y2;
        } else {
            error += delta_x2;
        }
        
        y0++;
        
        BlitDot(x0, y0, color);
    }
}

void BlitLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color) {
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
    }

    int32_t delta_x = x1 - x0;
    int32_t delta_y = y1 - y0;
    
    if (delta_x > 0) {
        if (delta_x > delta_y) {
            BlitLineOctant0(x0, y0, delta_x, delta_y, 1, color);
        } else {
            BlitLineOctant1(x0, y0, delta_x, delta_y, 1, color);
        }
    } else {
        delta_x = -delta_x;
        if (delta_x > delta_y) {
            BlitLineOctant0(x0, y0, delta_x, delta_y, -1, color);
        } else {
            BlitLineOctant1(x0, y0, delta_x, delta_y, -1, color);
        }
    }
}



struct Point2D {
  int32_t x;
  int32_t y;
  uint32_t depth;
};

template <bool set_span_first>
void MakeSpans(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t p0, int32_t p1, int32_t skip_first, Span** span_array) {
    Span* span = *span_array;

    int32_t delta_x = x1 - x0;
    int32_t advance = delta_x > 0 ? 1 : -1;

    int32_t width = abs(delta_x);
    int32_t height = y1 - y0;

    if (height <= 0) return;
    
    int32_t total_spans = height - skip_first;
    int32_t point_advance = -(0xFFFF / height);
    int32_t point_sum = 0xFFFF;

    if (width == 0) {
        for (int32_t i = total_spans; i-- > 0; span++) {
            if (set_span_first) {
                span->begin = x0;
                span->begin_p0 = p0;
                span->begin_p1 = p1;
                span->begin_adv = point_sum;
            } else {
                span->end = x0;
                span->end_p0 = p0;
                span->end_p1 = p1;
                span->end_adv = point_sum;
            }
            
            point_sum += point_advance;
        }
    } else if (width == height) {
        if (skip_first) {
            x0 += advance; 
        }

        for (int32_t i = total_spans; i-- > 0; span++) {
            if (set_span_first) {
                span->begin = x0;
                span->begin_p0 = p0;
                span->begin_p1 = p1;
                span->begin_adv = point_sum;
            } else {
                span->end = x0;
                span->end_p0 = p0;
                span->end_p1 = p1;
                span->end_adv = point_sum;
            }
            
            point_sum += point_advance;
            x0 += advance; 
        }
    } else if (height > width) {
        int32_t error = 0;
        if (delta_x < 0) {
            error = -height + 1;
        }

        if (skip_first) {
            error += width;
            if (error > 0) {
                x0 += advance; 
                error -= height; 
            }
        }

        for (int32_t i = total_spans; i-- > 0; span++) {
            if (set_span_first) {
                span->begin = x0;
                span->begin_p0 = p0;
                span->begin_p1 = p1;
                span->begin_adv = point_sum;
            } else {
                span->end = x0;
                span->end_p0 = p0;
                span->end_p1 = p1;
                span->end_adv = point_sum;
            }

            point_sum += point_advance;
            
            error += width;
            if (error > 0) {
                x0 += advance; 
                error -= height; 
            }
        }
    } else {
        int32_t major_advance = (width / height) * advance;
        int32_t error_advance = width % height;

        int32_t error = 0;
        if (delta_x < 0) {
            error = -height + 1;
        }

        if (skip_first) {   
            x0 += major_advance;    

            error += error_advance;
            if (error > 0) {
                x0 += advance;   
                error -= height; 
            }
        }

        for (int32_t i = total_spans; i-- > 0; span++) {
            if (set_span_first) {
                span->begin = x0;
                span->begin_p0 = p0;
                span->begin_p1 = p1;
                span->begin_adv = point_sum;
            } else {
                span->end = x0;
                span->end_p0 = p0;
                span->end_p1 = p1;
                span->end_adv = point_sum;
            }
            
            point_sum += point_advance;
            x0 += major_advance;   
 
            error += error_advance;
            if (error > 0) {
                x0 += advance;      
                error -= height; 
            }
        }
    }

    *span_array = span;
}

void RasterizeTriangle(Point2D* vertices, uint16_t color) {
    Span* span_ptr;

    int32_t min_index = 0;
    int32_t max_index = 0;

    int32_t max_point_y = vertices->y;
    int32_t min_point_y = vertices->y;

    for (int32_t i = 1; i < 3; i++) {
        if (vertices[i].y < min_point_y) {
            min_index = i;
            min_point_y = vertices[min_index].y;
        } else if (vertices[i].y > max_point_y) {
            max_index = i;
            max_point_y = vertices[max_index].y;
        }
    }

    if (min_point_y == max_point_y) return;

    int32_t min_index_r = min_index;
    int32_t min_index_l = min_index;

    if (int32_t next_index = (min_index_r + 1) % 3; vertices[next_index].y == min_point_y) {
        min_index_r = next_index;
    }

    if (int32_t prev_index = (min_index_l - 1 + 3) % 3; vertices[prev_index].y == min_point_y) {
        min_index_l = prev_index;
    }

    int32_t left_edge_dir = -1;
    bool top_is_flat = vertices[min_index_l].x != vertices[min_index_r].x;
    if (top_is_flat) {
        if (vertices[min_index_l].x > vertices[min_index_r].x) {
            left_edge_dir = 1;
            std::swap(min_index_l, min_index_r);
        }
    } else {
        int32_t next_index = (min_index_r + 1) % 3;
        int32_t prev_index = (min_index_l - 1 + 3) % 3;

        int32_t delta_xn = vertices[next_index].x - vertices[min_index_l].x;
        int32_t delta_yn = vertices[next_index].y - vertices[min_index_l].y;
        int32_t delta_xp = vertices[prev_index].x - vertices[min_index_l].x;
        int32_t delta_yp = vertices[prev_index].y - vertices[min_index_l].y;
        
        if (delta_xn * delta_yp - delta_yn * delta_xp < 0) {
            left_edge_dir = 1;
            std::swap(min_index_l, min_index_r);
        }
    }

    int32_t span_length = max_point_y - min_point_y - 1 + top_is_flat;
    int32_t start_y = min_point_y + 1 - top_is_flat;

    if (span_length <= 0) return;

    span_ptr = spans;
    int32_t prev_index = min_index_l;
    int32_t this_index = min_index_l;
    bool skip_first = !top_is_flat;

    do {                                
        if (left_edge_dir > 0) {
            this_index = (this_index + 1) % 3;
        } else {
            this_index = (this_index - 1 + 3) % 3;
        }

        MakeSpans<true>(vertices[prev_index].x,
                        vertices[prev_index].y,
                        vertices[this_index].x,
                        vertices[this_index].y,
                        prev_index,
                        this_index,
                        skip_first,
                        &span_ptr);
                 
         prev_index = this_index;
         skip_first = 0;
    } while (this_index != max_index);

    span_ptr = spans;
    prev_index = min_index_r;
    this_index = min_index_r;
    skip_first = !top_is_flat;

    do {
        if (left_edge_dir < 0) {
            this_index = (this_index + 1) % 3;
        } else {
            this_index = (this_index - 1 + 3) % 3;
        }
        
        MakeSpans<false>(vertices[prev_index].x - 1,
                         vertices[prev_index].y,
                         vertices[this_index].x - 1,
                         vertices[this_index].y,
                         prev_index,
                         this_index,
                         skip_first,
                         &span_ptr);
                 
        prev_index = this_index;
        skip_first = 0; 
    } while (this_index != max_index);

    span_ptr = spans;
    for (int32_t y = start_y; y < (start_y + span_length); y++, span_ptr++) {
        int32_t begin_mix[3] = {0, 0, 0};
        int32_t end_mix[3] = {0, 0, 0};
        
        begin_mix[span_ptr->begin_p0] = span_ptr->begin_adv;
        begin_mix[span_ptr->begin_p1] = 0xFFFF - span_ptr->begin_adv;
        
        end_mix[span_ptr->end_p0] = span_ptr->end_adv;
        end_mix[span_ptr->end_p1] = 0xFFFF - span_ptr->end_adv;
        
        int32_t span_length = span_ptr->end - span_ptr->begin + 1;
        
        if (span_length <= 0) continue;
        
        int32_t p0_adv = (end_mix[0] - begin_mix[0]) / span_length;
        int32_t p1_adv = (end_mix[1] - begin_mix[1]) / span_length;
        int32_t p2_adv = (end_mix[2] - begin_mix[2]) / span_length;
        
        int32_t p0_mix = begin_mix[0];
        int32_t p1_mix = begin_mix[1];
        int32_t p2_mix = begin_mix[2];
        
        for (int32_t x = span_ptr->begin; x <= span_ptr->end; x++) {
            int32_t r = (p0_mix & 0xFFFF) >> (16 - 5);
            int32_t g = (p1_mix & 0xFFFF) >> (16 - 6);
            int32_t b = (p2_mix & 0xFFFF) >> (16 - 5);
            
            int32_t c = (r << (5 + 6)) | (g << (5)) | b;
            
            
            int32_t depth = ((p0_mix * vertices[0].depth) >> 16)
                          + ((p1_mix * vertices[1].depth) >> 16)
                          + ((p2_mix * vertices[2].depth) >> 16);
            
            //int32_t r = (depth & 0xFFFF) >> (16 - 5);
            //int32_t g = (depth & 0xFFFF) >> (16 - 6);
            // b = (depth & 0xFFFF) >> (16 - 5);
            //int32_t c = (r << (5 + 6)) | (g << (5)) | b;
            
            if (depth_buffer[y * screen_width + x] < depth) {
                depth_buffer[y * screen_width + x] = depth;
                BlitDot(x, y, c);
            }
            
            p0_mix += p0_adv;
            p1_mix += p1_adv;
            p2_mix += p2_adv;
        }
    }
}


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

void ClipLineInClipSpace(vec4& point0, vec4& point1) {
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

vec4 ClipSinglePointLine(vec4* outside, vec4* inside) {
    vec4 p0 = *outside;
    vec4 p1 = *inside;
    
    // clip against left plane
    if (p0.w + p0.x < 0.0f) {
        float a = (p0.w + p0.x) / ((p0.w + p0.x) - (p1.w + p1.x));
        p0 = (1.0f - a) * p0 + a * p1;
    }
    
    // clip against right plane
    if (p0.w - p0.x < 0.0f) {
        float a = (p0.w - p0.x) / ((p0.w - p0.x) - (p1.w - p1.x));
        p0 = (1.0f - a) * p0 + a * p1;
    }
    
    // clip against bottom plane
    if (p0.w + p0.y < 0.0f) {
        float a = (p0.w + p0.y) / ((p0.w + p0.y) - (p1.w + p1.y));
        p0 = (1.0f - a) * p0 + a * p1;
    }
    
    // clip against top plane
    if (p0.w - p0.y < 0.0f) {
        float a = (p0.w - p0.y) / ((p0.w - p0.y) - (p1.w - p1.y));
        p0 = (1.0f - a) * p0 + a * p1;
    }
    
    // clip against near plane
    if (p0.w + p0.z < 0.0f) {
        float a = (p0.w + p0.z) / ((p0.w + p0.z) - (p1.w + p1.z));
        p0 = (1.0f - a) * p0 + a * p1;
    }
    
    // clip against far plane
    if (p0.w - p0.z < 0.0f) {
        float a = (p0.w - p0.z) / ((p0.w - p0.z) - (p1.w - p1.z));
        p0 = (1.0f - a) * p0 + a * p1;
    }
    
    return p0;
}

vec4 ClipSinglePointLineLeftPlane(vec4 outside, vec4 inside) {
    float a = (outside.w + outside.x) / ((outside.w + outside.x) - (inside.w + inside.x));
    return (1.0f - a) * outside + a * inside;
}

vec4 ClipSinglePointLineRightPlane(vec4 outside, vec4 inside) {
    float a = (outside.w - outside.x) / ((outside.w - outside.x) - (inside.w - inside.x));
    return (1.0f - a) * outside + a * inside;
}

vec4 ClipSinglePointLineBottomPlane(vec4 outside, vec4 inside) {
    float a = (outside.w + outside.y) / ((outside.w + outside.y) - (inside.w + inside.y));
    return (1.0f - a) * outside + a * inside;
}

vec4 ClipSinglePointLineTopPlane(vec4 outside, vec4 inside) {
    float a = (outside.w - outside.y) / ((outside.w - outside.y) - (inside.w - inside.y));
    return (1.0f - a) * outside + a * inside;
}

vec4 ClipSinglePointLineNearPlane(vec4 outside, vec4 inside) {
    float a = (outside.w + outside.z) / ((outside.w + outside.z) - (inside.w + inside.z));
    return (1.0f - a) * outside + a * inside;
}

vec4 ClipSinglePointLineFarPlane(vec4 outside, vec4 inside) {
    float a = (outside.w - outside.z) / ((outside.w - outside.z) - (inside.w - inside.z));
    return (1.0f - a) * outside + a * inside;
}

void PerspectiveDivision(vec4& p) {
    if (p.w != 0.0f) {
        p.x /= p.w;
        p.y /= p.w;
        p.z /= p.w;
    }
}

void PerspectiveDivision(vec4& p0, vec4& p1) {
    if (p0.w != 0.0f) {
        p0.x /= p0.w;
        p0.y /= p0.w;
    }
    
    if (p1.w != 0.0f) {
        p1.x /= p1.w;
        p1.y /= p1.w;
    }
}

std::pair<int32_t, int32_t> ClipSpaceToScreenSpace(const vec4 p) {
    int32_t px = 2 + (p.x + 1.0f) * 0.5f * (screen_width - 4);
    int32_t py = 2 + (1.0f - p.y) * 0.5f * (screen_height - 4);
    return {px, py};
}

bool PointVisible(vec4 p) {    
    // point outside left plane
    if (p.w + p.x < 0.0f) {
        return false;
    }

    // point outside right plane
    if (p.w - p.x < 0.0f) {
        return false;
    }

    // point outside bottom plane
    if (p.w + p.y < 0.0f) {
        return false;
    }

    // point outside top plane
    if (p.w - p.y < 0.0f) {
        return false;
    }

    // point outside near plane
    if (p.w + p.z < 0.0f) {
        return false;
    }
    
    // point outside far plane
    if (p.w - p.z < 0.0f) {
        return false;
    }
    
    return true;
}

bool PointOutsideLeftPlane(vec4 p) {    
    return p.w + p.x < 0.0f;
}

bool PointOutsideRightPlane(vec4 p) {    
    return p.w - p.x < 0.0f;
}

bool PointOutsideBottomPlane(vec4 p) {    
    return p.w + p.y < 0.0f;
}

bool PointOutsideTopPlane(vec4 p) {    
    return p.w - p.y < 0.0f;
}

bool PointOutsideNearPlane(vec4 p) {    
    return p.w + p.z < 0.0f;
}

bool PointOutsideFarPlane(vec4 p) {    
    return p.w - p.z < 0.0f;
}

bool LineVisible(vec4 p0, vec4 p1) {    
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

void ClipRenderLine(vec4 p0, vec4 p1, uint16_t color) {
    if (!LineVisible(p0, p1)) return;
    
    ClipLineInClipSpace(p0, p1);
    PerspectiveDivision(p0, p1);
    
    auto [px0, py0] = ClipSpaceToScreenSpace(p0);
    auto [px1, py1] = ClipSpaceToScreenSpace(p1);
    
    BlitLine(px0, py0, px1, py1, color);
}

struct ClipTriangle {
    vec4 points[3];
};

size_t ClipTriangleList(ClipTriangle* triangles) {
    ClipTriangle tri_list_1[12] = {*triangles};
    ClipTriangle tri_list_2[12];
    size_t tri_count_working = 1;
    size_t tri_count_buffered = 0;
    ClipTriangle* working_list = tri_list_1;
    ClipTriangle* buffer_list = tri_list_2;
    
    auto clip_lambda = [&](auto outside_func, auto clip_func) {
        for (size_t i = 0; i < tri_count_working; i++) {
            vec4* in_points[3] = {nullptr, nullptr, nullptr};
            vec4* out_points[3] = {nullptr, nullptr, nullptr};
            size_t in_point_count = 0;
            size_t out_point_count = 0;
            
            for (size_t p = 0; p < 3; p++) {
                if (outside_func(working_list[i].points[p])) {
                    out_points[out_point_count++] = &working_list[i].points[p];
                } else {
                    in_points[in_point_count++] = &working_list[i].points[p];
                }
            }
            
            switch (out_point_count) {
                case 0:
                    // triangle doesn't intersect plane, don't clip
                    buffer_list[tri_count_buffered++] = working_list[i];
                    break;
                case 1: {
                    vec4 clipped0 = clip_func(*out_points[0], *in_points[0]);
                    vec4 clipped1 = clip_func(*out_points[0], *in_points[1]);
                    
                    buffer_list[tri_count_buffered++] = {{*in_points[1], *in_points[0], clipped0}};
                    buffer_list[tri_count_buffered++] = {{*in_points[1], clipped0, clipped1}};
                    
                    } break;
                case 2: {
                    vec4 clipped0 = clip_func(*out_points[0], *in_points[0]);
                    vec4 clipped1 = clip_func(*out_points[1], *in_points[0]);
                    
                    buffer_list[tri_count_buffered++] = {{*in_points[0], clipped0, clipped1}};
                    } break;
                case 3:
                    // triangle completely outside of plane, discard
                    break;
            }
        }
        
        assert(tri_count_buffered != 12);
        
        tri_count_working = tri_count_buffered;
        tri_count_buffered = 0;
        std::swap(working_list, buffer_list);
    };
    
    clip_lambda(PointOutsideNearPlane, ClipSinglePointLineNearPlane);
    clip_lambda(PointOutsideFarPlane, ClipSinglePointLineFarPlane);
    clip_lambda(PointOutsideLeftPlane, ClipSinglePointLineLeftPlane);
    clip_lambda(PointOutsideRightPlane, ClipSinglePointLineRightPlane);
    clip_lambda(PointOutsideBottomPlane, ClipSinglePointLineBottomPlane);
    clip_lambda(PointOutsideTopPlane, ClipSinglePointLineTopPlane);
    
    for (size_t i = 0; i < tri_count_working; i++) {
        triangles[i] = working_list[i];
    }
    
    return tri_count_working;
}

void RenderFrame() {
    memset(depth_buffer, 0, 800 * 600 * sizeof(float));

    if (clear_screen) {
        uint16_t clear_color = IntColor(screen_clear_color);
        for (int i = 0; i < 800 * 600; i++) {
            screen_buffer[i] = clear_color;
        }
    }
    
    /*for (int i = 0; i < 500; i++) {
        uint16_t color = IntColor(COLOR_PINK);
        uint16_t x = sinf(i + GetTickTime()) * 100.0f + 200.0f;
        uint16_t y = cosf(i + GetTickTime()) * 100.0f + 200.0f;
        BlitDot(x, y, color);
    }
    
    float turn = 3.14f / 2.5f;
    for (float r = 0.0f; r < 3.14f * 2.0f; r += turn) {
        uint16_t x0 = sinf(r + GetTickTime()) * 50.0f + 420.0f;
        uint16_t y0 = cosf(r + GetTickTime()) * 50.0f + 200.0f;
        uint16_t x1 = sinf(r + turn + GetTickTime()) * 50.0f + 420.0f;
        uint16_t y1 = cosf(r + turn + GetTickTime()) * 50.0f + 200.0f;
        BlitLine(x0, y0, x1, y1, IntColor(COLOR_YELLOW));
    }
    
    float spin = GetTickTime() * 2.0f;
    
    BlitLine(420, 420, sinf(spin) * 100 + 420, cosf(spin) * 100 + 420, IntColor(COLOR_CYAN));
    
    float spin1 = GetTickTime() * 2.0f + 0.0f;
    float spin2 = GetTickTime() * 2.0f + 2.1f;
    float spin3 = GetTickTime() * 2.0f + 2.1f + 2.1f;
    int x0 = 200 * sinf(spin1) + 300;
    int y0 = 200 * cosf(spin1) + 300;
    int x1 = 200 * sinf(spin2) + 300;
    int y1 = 200 * cosf(spin2) + 300;
    int x2 = fabsf(sinf(GetTickTime() * 1.76f)) * 200 * sinf(spin3) + 300;
    int y2 = fabsf(sinf(GetTickTime() * 1.76f)) * 200 * cosf(spin3) + 300;
    int x3 = 200 * sinf(spin3 + 0.4f) + 300;
    int y3 = 200 * cosf(spin3+ 0.4f) + 300;
    
    //BlitLine(x0, y0, x1, y1, IntColor(COLOR_CYAN));
    //BlitLine(x1, y1, x2, y2, IntColor(COLOR_CYAN));
    //BlitLine(x2, y2, x0, y0, IntColor(COLOR_CYAN));
    
    Point2D plist[4] = {{x0, y0}, {x1, y1}, {x2, y2}, {x3, y3}};
    
    //BlitLine(0, 0, x0, y0, IntColor(COLOR_RED));
    //BlitLine(0, 0, x1, y1, IntColor(COLOR_GREEN));
    //BlitLine(0, 0, x2, y2, IntColor(COLOR_BLUE));
    
    RasterizeTriangle(plist, IntColor(COLOR_GREEN));
    
    
    Point2D plist2[3] = {{0, 0}, {0, 100}, {100, 0}};
    
    RasterizeTriangle(plist2, IntColor(COLOR_BLUE));*/
    
    
    std::vector<std::pair<uint32_t, SWDrawListEntry*>> draw_list_sorted;
    for (auto& entry : draw_list) {
        uint32_t sort_key = entry.layer;
        draw_list_sorted.push_back({sort_key, &entry});
    }
    sort(draw_list_sorted.begin(), draw_list_sorted.end());
    
    uint32_t current_layer = 0;
    for (auto [_, entry] : draw_list_sorted) {
        if (!(entry->flags & FLAG_RENDER)) continue;
        if (!entry->vertex_array) continue;

        if (current_layer != entry->layer) {
            // TODO: zero out zbuffer?
        }
        current_layer = entry->layer;
        
        // ...
        
        
        switch (entry->vertex_array->type) {
            case SW_STATIC_LIGHTMAPPED:
                
                if (!entry->index_array) continue;

                for (size_t i = 0; i < entry->index_length; i++) {
                    uint32_t triangle = (entry->index_offset + i) * 3;
                    
                    StaticVertex& p0 = ((StaticVertex*)entry->vertex_array->vertices)[entry->index_array->indices[triangle + 0]];
                    StaticVertex& p1 = ((StaticVertex*)entry->vertex_array->vertices)[entry->index_array->indices[triangle + 1]];
                    StaticVertex& p2 = ((StaticVertex*)entry->vertex_array->vertices)[entry->index_array->indices[triangle + 2]];
                    
                    vec4 pr0 = layers[0].projection_matrix * layers[0].view_matrix * entry->matrix * vec4(p0.pos, 1.0f);
                    vec4 pr1 = layers[0].projection_matrix * layers[0].view_matrix * entry->matrix * vec4(p1.pos, 1.0f);
                    vec4 pr2 = layers[0].projection_matrix * layers[0].view_matrix * entry->matrix * vec4(p2.pos, 1.0f);
                    
                    // backface culling
                    vec2 ab = {pr1.x/pr1.w - pr0.x/pr0.w, pr1.y/pr1.w - pr0.y/pr0.w};
                    vec2 ac = {pr2.x/pr2.w - pr0.x/pr0.w, pr2.y/pr2.w - pr0.y/pr0.w};
                    if (ab.x * ac.y - ac.x * ab.y < 0.0f) continue;
                    
                    if (PointVisible(pr0) && PointVisible(pr1) && PointVisible(pr2)) {
                        
                        PerspectiveDivision(pr0);
                        PerspectiveDivision(pr1);
                        PerspectiveDivision(pr2);
    
                        auto [px0, py0] = ClipSpaceToScreenSpace(pr0);
                        auto [px1, py1] = ClipSpaceToScreenSpace(pr1);
                        auto [px2, py2] = ClipSpaceToScreenSpace(pr2);
                        
                        uint32_t pz0 = (float)0xFFFF * (pr0.z/pr0.w);
                        uint32_t pz1 = (float)0xFFFF * (pr1.z/pr1.w);
                        uint32_t pz2 = (float)0xFFFF * (pr2.z/pr2.w);
                        
                        Point2D ps[3] = {{px0, py0, pz0}, {px1, py1, pz1}, {px2, py2, pz2}};
                        
                        //std::cout << 0xFFFF << " " << pz2 << std::endl; 
                        
                        RasterizeTriangle(ps , IntColor(COLOR_WHITE));
                    } else {
                        ClipTriangle clipped[12] = {{{pr0, pr1, pr2}}};
                        size_t tri_count = ClipTriangleList(clipped);
                        
                        for (size_t i = 0; i < tri_count; i++) {
                            PerspectiveDivision(clipped[i].points[0]);
                            PerspectiveDivision(clipped[i].points[1]);
                            PerspectiveDivision(clipped[i].points[2]);
        
                            auto [px0, py0] = ClipSpaceToScreenSpace(clipped[i].points[0]);
                            auto [px1, py1] = ClipSpaceToScreenSpace(clipped[i].points[1]);
                            auto [px2, py2] = ClipSpaceToScreenSpace(clipped[i].points[2]);
                            
                            uint32_t pz0 = (float)0xFFFF * (clipped[i].points[0].z/clipped[i].points[0].w);
                            uint32_t pz1 = (float)0xFFFF * (clipped[i].points[1].z/clipped[i].points[1].w);
                            uint32_t pz2 = (float)0xFFFF * (clipped[i].points[2].z/clipped[i].points[2].w);
                            
                            Point2D ps[3] = {{px0, py0, pz0}, {px1, py1, pz1}, {px2, py2, pz2}};
                            RasterizeTriangle(ps , IntColor(COLOR_WHITE));
                        }
                        
                        //ClipRenderLine(pr0, pr1, IntColor(COLOR_WHITE));
                        //ClipRenderLine(pr1, pr2, IntColor(COLOR_WHITE));
                        //ClipRenderLine(pr2, pr0, IntColor(COLOR_WHITE));
                    }
                    
                    
                }
                
            default:
                break;
        }
    }

}

drawlistentry_t InsertDrawListEntry() {
    return drawlistentry_t {.sw = draw_list.AddNew()};
}

void RemoveDrawListEntry(drawlistentry_t entry) {
    draw_list.Remove(entry.sw);
}

uint32_t GetFlags(drawlistentry_t entry) {
    return entry.sw->flags;
}

void SetFlags(drawlistentry_t entry, uint32_t flags) {
    entry.sw->flags = flags;
}

void SetLayer(drawlistentry_t entry, uint32_t layer) {
    entry.sw->layer = layer;
}

void SetPose(drawlistentry_t entry, Pose* pose) {
    entry.sw->pose = pose;
}

void SetLightmap(drawlistentry_t entry, texturehandle_t lightmap) {
    entry.sw->lightmap = lightmap.sw_texture;
}

void SetDrawListColors(drawlistentry_t entry, size_t count, vec4* color) {
    entry.sw->color = *color;
}

void SetDrawListSpecularities(drawlistentry_t entry, size_t count, float* weight, float* exponent, float* transparency) {
    entry.sw->specular_weight = *weight;
    entry.sw->specular_exponent = *exponent;
    entry.sw->specular_transparency = *transparency;
}

void SetLights(drawlistentry_t entry, uint32_t* lights) {
    for (size_t i = 0; i < 4; i++) {
        entry.sw->lights[i] = lights[i];
    }
}

void SetMatrix(drawlistentry_t entry, const mat4& matrix) {
    vec4 origin = matrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    
    entry.sw->matrix = matrix;
    
    light_tree.FindNearest(entry.sw->lights, origin.x, origin.y, origin.z);
}

void SetDrawListVertexArray(drawlistentry_t entry, vertexarray_t vertex_array_handle) {
    entry.sw->vertex_array = vertex_array_handle.sw_vertex_array;
}

void SetDrawListIndexArray(drawlistentry_t entry, indexarray_t index_array_handle) {
    entry.sw->index_array = index_array_handle.sw_index_array;
}

void SetDrawListIndexRange(drawlistentry_t entry, uint32_t index_offset, uint32_t index_length) {
    entry.sw->index_offset = index_offset;
    entry.sw->index_length = index_length;
}

void SetDrawListShader(drawlistentry_t entry, vertexformat_t vertex_format, materialtype_t material_type) {
    // TODO: check if material_type has transparency and save that fact
}

void SetDrawListTextures(drawlistentry_t entry, size_t texture_count, texturehandle_t* texture) {
    std::cout << "drawlist TEXRTUREs: " << texture_count << std::endl;
    entry.sw->texture = texture->sw_texture;
}

light_t MakeLight() {
    SWLight* light = light_list.AddNew();
    uint32_t light_id = light - light_list.GetFirst();
    uint32_t leaf_id = light_tree.AddLeaf(light_id, 0.0f, 0.0f, 0.0f);
    
    light_tree_ids [light_id] = leaf_id;
        
    return light_t { .sw = light };
}

void DeleteLight(light_t light) {
    SWLight* light_ptr = light.sw;
    uint32_t light_id = light_ptr - light_list.GetFirst();
    uint32_t leaf_id = light_tree_ids [light_id];

    light_list.Remove(light_ptr);
    light_tree.RemoveLeaf(leaf_id);
}

void SetLightParameters(light_t light, vec3 location, vec3 color, float distance, vec3 direction, float exponent) {
    SWLight* light_ptr = light.sw;
    uint32_t light_id = light_ptr - light_list.GetFirst();
    uint32_t leaf_id = light_tree_ids [light_id];
    
    light_ptr->location = location;
    light_ptr->color = color;
    light_ptr->distance = distance;
    light_ptr->direction = direction;
    light_ptr->exponent = exponent;
    
    light_tree.RemoveLeaf(leaf_id);
    leaf_id = light_tree.AddLeaf(light_id, light_ptr->location.x, light_ptr->location.y, light_ptr->location.z);
    light_tree_ids [light_id] = leaf_id;
}


texturehandle_t CreateTexture(ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data) {
    SWTexture* texture = texture_list.AddNew();
    
    texture->width = width;
    texture->height = height;
    
    switch (color_mode) {
        case COLORMODE_R:       texture->channels = 1;  break;
        case COLORMODE_RG:      texture->channels = 2;  break;
        case COLORMODE_RGB:     texture->channels = 3;  break;
        case COLORMODE_RGBA:    texture->channels = 4;  break;
    }
    
    switch (texture_filter) {
        case TEXTUREFILTER_NEAREST:             texture->mode = SW_NEAREST; break;
        case TEXTUREFILTER_LINEAR:              texture->mode = SW_BLENDED; break;
        case TEXTUREFILTER_LINEAR_MIPMAPPED:    texture->mode = SW_BLENDED; break;
    }
    
    int texture_size = texture->width * texture->height * texture->channels;
    texture->pixels = (char*)malloc(texture_size);
    memcpy(texture->pixels, data, texture_size);
    
    return texturehandle_t {.sw_texture = texture};
}

void ParseFormat(vertexarray_t& vertex_array, VertexDefinition vertex_format) {
    auto& helper = vertex_array.sw_vertex_array->helper;
 
     for (int i = 0; i < (int)vertex_format.attribute_count; i++) {
        switch (vertex_format.attributes[i].ffp_type) {
            case VertexAttribute::FFP_POSITION:     helper.position = i;    break;
            case VertexAttribute::FFP_NORMAL:       helper.normal = i;      break;
            case VertexAttribute::FFP_COLOR:        helper.color = i;      break;
            case VertexAttribute::FFP_TEXTURE:      helper.texture = i;     break;
            case VertexAttribute::FFP_LIGHTMAP:     helper.lightmap = i;    break;
            case VertexAttribute::FFP_BONE_INDEX:   helper.bone_index = i;  break;
            case VertexAttribute::FFP_BONE_WEIGHT:  helper.bone_weight = i; break;
            case VertexAttribute::FFP_IGNORE:                               break;
        }
    }
    
    if (helper.color != -1) {
         vertex_array.sw_vertex_array->type = SW_STATIC_COLORED;
    }
 
    if (helper.lightmap != -1) {
        vertex_array.sw_vertex_array->type = SW_STATIC_LIGHTMAPPED;
    }
    
    if (helper.bone_index != -1 && helper.bone_weight != -1) {
        vertex_array.sw_vertex_array->type = SW_DYNAMIC_BLENDED;
    }
    
    vertex_array.sw_vertex_array->format = vertex_format;
}

void PackVertices(vertexarray_t& vertex_array, void* data, size_t count) {
    switch (vertex_array.sw_vertex_array->type) {
        case SW_STATIC_LIGHTMAPPED: {
            StaticVertex* verts = (StaticVertex*)malloc(sizeof(StaticVertex) * count);
            
            VertexAttribute pos_attrib = vertex_array.sw_vertex_array->format.attributes[vertex_array.sw_vertex_array->helper.position];
            for (size_t i = 0; i < count; i++) {
                vec3* pos = (vec3*)((char*)data + pos_attrib.offset + pos_attrib.stride * i);
                verts[i].pos = *pos;
            }
            
            VertexAttribute nrm_attrib = vertex_array.sw_vertex_array->format.attributes[vertex_array.sw_vertex_array->helper.normal];
            for (size_t i = 0; i < count; i++) {
                vec3* nrm = (vec3*)((char*)data + nrm_attrib.offset + nrm_attrib.stride * i);
                verts[i].nrm = *nrm;
            }
            
            VertexAttribute tex_attrib = vertex_array.sw_vertex_array->format.attributes[vertex_array.sw_vertex_array->helper.texture];
            for (size_t i = 0; i < count; i++) {
                vec2* tex = (vec2*)((char*)data + tex_attrib.offset + tex_attrib.stride * i);
                verts[i].tex = *tex;
            }
            
            VertexAttribute lit_attrib = vertex_array.sw_vertex_array->format.attributes[vertex_array.sw_vertex_array->helper.lightmap];
            for (size_t i = 0; i < count; i++) {
                vec2* lit = (vec2*)((char*)data + lit_attrib.offset + lit_attrib.stride * i);
                verts[i].lit = *lit;
            }
            
            vertex_array.sw_vertex_array->vertices = verts;
            vertex_array.sw_vertex_array->vertex_count = count;
        } break;
        case SW_DYNAMIC_BLENDED: {
            DynamicVertex* verts = (DynamicVertex*)malloc(sizeof(DynamicVertex) * count);
            
            VertexAttribute pos_attrib = vertex_array.sw_vertex_array->format.attributes[vertex_array.sw_vertex_array->helper.position];
            for (size_t i = 0; i < count; i++) {
                vec3* pos = (vec3*)((char*)data + pos_attrib.offset + pos_attrib.stride * i);
                verts[i].pos = *pos;
            }
            
            VertexAttribute nrm_attrib = vertex_array.sw_vertex_array->format.attributes[vertex_array.sw_vertex_array->helper.normal];
            for (size_t i = 0; i < count; i++) {
                vec3* nrm = (vec3*)((char*)data + nrm_attrib.offset + nrm_attrib.stride * i);
                verts[i].nrm = *nrm;
            }
            
            VertexAttribute tex_attrib = vertex_array.sw_vertex_array->format.attributes[vertex_array.sw_vertex_array->helper.texture];
            for (size_t i = 0; i < count; i++) {
                vec2* tex = (vec2*)((char*)data + tex_attrib.offset + tex_attrib.stride * i);
                verts[i].tex = *tex;
            }
            
            VertexAttribute wgt_attrib = vertex_array.sw_vertex_array->format.attributes[vertex_array.sw_vertex_array->helper.bone_weight];
            for (size_t i = 0; i < count; i++) {
                vec4* wgt = (vec4*)((char*)data + wgt_attrib.offset + wgt_attrib.stride * i);
                verts[i].wgt = *wgt;
            }
            
            VertexAttribute ind_attrib = vertex_array.sw_vertex_array->format.attributes[vertex_array.sw_vertex_array->helper.bone_index];
            for (size_t i = 0; i < count; i++) {
                ivec4* ind = (ivec4*)((char*)data + ind_attrib.offset + ind_attrib.stride * i);
                verts[i].ind = *ind;
            }
        
            vertex_array.sw_vertex_array->vertices = verts;
            vertex_array.sw_vertex_array->vertex_count = count;
        } break;
        case SW_STATIC_COLORED: {
            ColorVertex* verts = (ColorVertex*)malloc(sizeof(ColorVertex) * count);
            
            VertexAttribute pos_attrib = vertex_array.sw_vertex_array->format.attributes[vertex_array.sw_vertex_array->helper.position];
            for (size_t i = 0; i < count; i++) {
                vec3* pos = (vec3*)((char*)data + pos_attrib.offset + pos_attrib.stride * i);
                verts[i].pos = *pos;
            }
            
            VertexAttribute col_attrib = vertex_array.sw_vertex_array->format.attributes[vertex_array.sw_vertex_array->helper.color];
            for (size_t i = 0; i < count; i++) {
                vec3* col = (vec3*)((char*)data + col_attrib.offset + col_attrib.stride * i);
                verts[i].col = *col;
            }
            
            vertex_array.sw_vertex_array->vertices = verts;
            vertex_array.sw_vertex_array->vertex_count = count;
        } break;
        case SW_SPRITE:
            assert(false);
    }
}

void CreateIndexedVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array, indexarray_t& index_array, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data) {
    vertex_array.sw_vertex_array = vertex_arrays.AddNew();
    ParseFormat(vertex_array, vertex_format);
    PackVertices(vertex_array, vertex_data, vertex_size / vertex_format.attributes[0].stride);
    
    index_array.sw_index_array = index_arrays.AddNew();
    index_array.sw_index_array->indices = (uint32_t*)malloc(index_size);
    index_array.sw_index_array->index_count = index_size / sizeof(uint32_t);
    memcpy(index_array.sw_index_array->indices, index_data, index_size);
    /*glGenBuffers(1, &vertex_array.gl_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array.gl_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_size, vertex_data, GL_STATIC_DRAW);

    glGenBuffers(1, &index_array.gl_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_array.gl_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, index_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vertex_array.gl_vertex_array);

    glBindVertexArray(vertex_array.gl_vertex_array);

    for (size_t i = 0; i < vertex_format.attribute_count; i++) {
        uint32_t opengl_type = vertex_format.attributes[i].type == VertexAttribute::FLOAT32 ? GL_FLOAT : GL_UNSIGNED_INT;
        
        if (opengl_type == GL_FLOAT) {
            glVertexAttribPointer(i, vertex_format.attributes[i].size, opengl_type, GL_FALSE, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        } else {
            glVertexAttribIPointer(i, vertex_format.attributes[i].size, opengl_type, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        }
        
        glEnableVertexAttribArray(i);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_array.gl_index_buffer);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);*/
}

void CreateVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array) {
    vertex_array.sw_vertex_array = vertex_arrays.AddNew();
    ParseFormat(vertex_array, vertex_format);
    
    /*glGenBuffers(1, &vertex_array.gl_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array.gl_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &vertex_array.gl_vertex_array);

    glBindVertexArray(vertex_array.gl_vertex_array);

    for (size_t i = 0; i < vertex_format.attribute_count; i++) {
        uint32_t opengl_type = vertex_format.attributes[i].type == VertexAttribute::FLOAT32 ? GL_FLOAT : GL_UNSIGNED_INT;
        
        if (opengl_type == GL_FLOAT) {
            glVertexAttribPointer(i, vertex_format.attributes[i].size, opengl_type, GL_FALSE, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        } else {
            glVertexAttribIPointer(i, vertex_format.attributes[i].size, opengl_type, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        }
        
        glEnableVertexAttribArray(i);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);*/
}

void UpdateVertexArray(vertexarray_t& vertex_array, size_t data_size, void* data) {
    if (vertex_array.sw_vertex_array->vertices) {
        free(vertex_array.sw_vertex_array->vertices);
        vertex_array.sw_vertex_array->vertices = nullptr;
    }
    
    PackVertices(vertex_array, data, data_size / vertex_array.sw_vertex_array->format.attributes[0].stride);
    //glBindBuffer(GL_ARRAY_BUFFER, vertex_array.gl_vertex_buffer);
    //glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_DYNAMIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SetViewMatrix(const mat4& matrix, layer_t layer) {
    layers[layer].view_matrix = matrix;
    layers[layer].view_position = glm::inverse(matrix) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void SetProjectionMatrix(const mat4& matrix, layer_t layer) {
    layers[layer].projection_matrix = matrix;
}

void GetScreen(char* buffer, int w, int h) {
    //glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer);
}

void Init() {
    
    depth_buffer = (uint16_t*)malloc(800 * 600 * sizeof(float));
    
    
    // initialize the default pose
    BLANK_POSE = PoolProxy<Render::Pose>::New();
    for (size_t i = 0; i < BONE_COUNT; i++) {
        BLANK_POSE->pose[i] = mat4(1.0f);
    }
    
    // initialize the default light
    //new (light_list.begin().ptr) LightListEntry;
    light_list.AddNew();
}

ContextType GetContext() {
    return CONTEXT_SOFTWARE;
}

uint32_t GetMaxIndexRangeLength() {
    return 1;
}

void SetDevice(void* buffer) {
    screen_buffer = (uint16_t*)buffer;
}

}
