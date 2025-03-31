// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/software/software.h>
#include <render/api.h>

#include <templates/octree.h>

#include <config.h>

#include <cstdlib>
#include <cstring>

namespace tram::Render::API {

Pool<SWDrawListEntry> draw_list("render list", 500);
Pool<SWLight> light_list("light list", 200);
Pool<SWTexture> texture_list("texture list", RESOURCE_LIMIT_MATERIAL);
Pool<SWMaterial> material_list("material list", RESOURCE_LIMIT_MATERIAL);
Pool<SWVertexArray> vertex_arrays("vertex_arrays list", 200);
Pool<SWIndexArray> index_arrays("index_arrays list", 200);
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

static Render::Pose* null_pose = nullptr;

struct Scanline {
    uint16_t begin;     // x coord on which the span starts
    uint16_t end;       // x coord on which the span ends
    uint16_t begin_p0;  // first point for begin
    uint16_t begin_p1;  // second point for begin
    int32_t begin_adv;  // how far from the first point in begin, mapped to [0x0000, 0xFFFF]
    uint16_t end_p0;    // same as for begin, but for end
    uint16_t end_p1;    // ditto
    int32_t end_adv;    // ditto
};

struct ScanlineBuffer {
    Scanline scanlines[1000];
    int32_t scanline_length = 0;
    int32_t start_y = 0;
};

ScanlineBuffer* scanlines = new ScanlineBuffer;

void SetLightingParameters (vec3 sun_direction, vec3 sun_color, vec3 ambient_color, uint32_t layer) {
    layers[layer].sun_direction = sun_direction;
    layers[layer].sun_color = sun_color;
    layers[layer].ambient_color = ambient_color;
}

void SetScreenSize(float width, float height) {
    depth_buffer = (uint16_t*)malloc(screen_width * screen_height * sizeof(float));
    
    screen_width = width;
    screen_height = height;
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
void MakeSpans(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t p0, int32_t p1, int32_t skip_first, Scanline** scanline_array) {
    Scanline* scanline = *scanline_array;

    int32_t delta_x = x1 - x0;
    int32_t advance = delta_x > 0 ? 1 : -1;

    int32_t width = abs(delta_x);
    int32_t height = y1 - y0;

    if (height <= 0) return;
    
    int32_t total_spans = height - skip_first;
    int32_t point_advance = -(0xFFFF / height);
    int32_t point_sum = 0xFFFF;

    if (width == 0) {
        for (int32_t i = total_spans; i-- > 0; scanline++) {
            if (set_span_first) {
                scanline->begin = x0;
                scanline->begin_p0 = p0;
                scanline->begin_p1 = p1;
                scanline->begin_adv = point_sum;
            } else {
                scanline->end = x0;
                scanline->end_p0 = p0;
                scanline->end_p1 = p1;
                scanline->end_adv = point_sum;
            }
            
            point_sum += point_advance;
        }
    } else if (width == height) {
        if (skip_first) {
            x0 += advance; 
        }

        for (int32_t i = total_spans; i-- > 0; scanline++) {
            if (set_span_first) {
                scanline->begin = x0;
                scanline->begin_p0 = p0;
                scanline->begin_p1 = p1;
                scanline->begin_adv = point_sum;
            } else {
                scanline->end = x0;
                scanline->end_p0 = p0;
                scanline->end_p1 = p1;
                scanline->end_adv = point_sum;
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

        for (int32_t i = total_spans; i-- > 0; scanline++) {
            if (set_span_first) {
                scanline->begin = x0;
                scanline->begin_p0 = p0;
                scanline->begin_p1 = p1;
                scanline->begin_adv = point_sum;
            } else {
                scanline->end = x0;
                scanline->end_p0 = p0;
                scanline->end_p1 = p1;
                scanline->end_adv = point_sum;
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

        for (int32_t i = total_spans; i-- > 0; scanline++) {
            if (set_span_first) {
                scanline->begin = x0;
                scanline->begin_p0 = p0;
                scanline->begin_p1 = p1;
                scanline->begin_adv = point_sum;
            } else {
                scanline->end = x0;
                scanline->end_p0 = p0;
                scanline->end_p1 = p1;
                scanline->end_adv = point_sum;
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

    *scanline_array = scanline;
}

void ScanlineConvertTriangle(ScanlineBuffer* scanlines, Point2D* vertices) {
    Scanline* span_ptr;

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

    /*int32_t*/ scanlines->scanline_length = max_point_y - min_point_y - 1 + top_is_flat;
    /*int32_t*/ scanlines->start_y = min_point_y + 1 - top_is_flat;

    if (scanlines->scanline_length <= 0) return;

    span_ptr = scanlines->scanlines;
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

    span_ptr = scanlines->scanlines;
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
}

void RasterizeTriangle(ScanlineBuffer* scanlines, Point2D* vertices) {
    Scanline* span_ptr = scanlines->scanlines;
    for (int32_t y = scanlines->start_y; y < (scanlines->start_y + scanlines->scanline_length); y++, span_ptr++) {
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

void RasterizeTriangleLightmapped(ScanlineBuffer* scanlines, Point2D* vertices, uint16_t color, vec2* texture_uvs, vec2* lightmap_uvs, SWTexture* texture, SWTexture* lightmap) {
    int32_t p0_tex_x = texture_uvs[0].x * (float)texture->width * (float)(0xFF + 1);
    int32_t p0_tex_y = texture_uvs[0].y * (float)texture->height * (float)(0xFF + 1);
    int32_t p1_tex_x = texture_uvs[1].x * (float)texture->width * (float)(0xFF + 1);
    int32_t p1_tex_y = texture_uvs[1].y * (float)texture->height * (float)(0xFF + 1);
    int32_t p2_tex_x = texture_uvs[2].x * (float)texture->width * (float)(0xFF + 1);
    int32_t p2_tex_y = texture_uvs[2].y * (float)texture->height * (float)(0xFF + 1);
    
    int32_t p0_lit_x = lightmap_uvs[0].x * (float)lightmap->width * (float)(0xFF + 1);
    int32_t p0_lit_y = lightmap_uvs[0].y * (float)lightmap->height * (float)(0xFF + 1);
    int32_t p1_lit_x = lightmap_uvs[1].x * (float)lightmap->width * (float)(0xFF + 1);
    int32_t p1_lit_y = lightmap_uvs[1].y * (float)lightmap->height * (float)(0xFF + 1);
    int32_t p2_lit_x = lightmap_uvs[2].x * (float)lightmap->width * (float)(0xFF + 1);
    int32_t p2_lit_y = lightmap_uvs[2].y * (float)lightmap->height * (float)(0xFF + 1);
    
    Scanline* span_ptr = scanlines->scanlines;
    for (int32_t y = scanlines->start_y; y < (scanlines->start_y + scanlines->scanline_length); y++, span_ptr++) {
        int32_t begin_mix[3] = {0, 0, 0};
        int32_t end_mix[3] = {0, 0, 0};
        
        begin_mix[span_ptr->begin_p0] = span_ptr->begin_adv;
        begin_mix[span_ptr->begin_p1] = 0xFFFF - span_ptr->begin_adv;
        
        end_mix[span_ptr->end_p0] = span_ptr->end_adv;
        end_mix[span_ptr->end_p1] = 0xFFFF - span_ptr->end_adv;
        
        int32_t span_length = span_ptr->end - span_ptr->begin + 1;
        
        if (span_length <= 0) continue;
        
        int32_t depth_begin = ((begin_mix[0] * vertices[0].depth) >> 16) + ((begin_mix[1] * vertices[1].depth) >> 16) + ((begin_mix[2] * vertices[2].depth) >> 16);
        int32_t depth_end = ((end_mix[0] * vertices[0].depth) >> 16) + ((end_mix[1] * vertices[1].depth) >> 16) + ((end_mix[2] * vertices[2].depth) >> 16);
        
        int32_t depth_adv = (depth_end - depth_begin) / span_length;
        int32_t depth_mix = depth_begin;
        
        int32_t tex_x_begin = (((begin_mix[0] >> 8) * p0_tex_x) >> 8) + (((begin_mix[1] >> 8) * p1_tex_x) >> 8) + (((begin_mix[2] >> 8) * p2_tex_x) >> 8);
        int32_t tex_y_begin = (((begin_mix[0] >> 8) * p0_tex_y) >> 8) + (((begin_mix[1] >> 8) * p1_tex_y) >> 8) + (((begin_mix[2] >> 8) * p2_tex_y) >> 8);
        
        int32_t tex_x_end = (((end_mix[0] >> 8) * p0_tex_x) >> 8) + (((end_mix[1] >> 8) * p1_tex_x) >> 8) + (((end_mix[2] >> 8) * p2_tex_x) >> 8);
        int32_t tex_y_end = (((end_mix[0] >> 8) * p0_tex_y) >> 8) + (((end_mix[1] >> 8) * p1_tex_y) >> 8) + (((end_mix[2] >> 8) * p2_tex_y) >> 8);
        
        int32_t tex_x_adv = (tex_x_end - tex_x_begin) / span_length;
        int32_t tex_y_adv = (tex_y_end - tex_y_begin) / span_length;
        
        int32_t tex_x_mix = tex_x_begin;
        int32_t tex_y_mix = tex_y_begin;
        
        int32_t lit_x_begin = (((begin_mix[0] >> 8) * p0_lit_x) >> 8) + (((begin_mix[1] >> 8) * p1_lit_x) >> 8) + (((begin_mix[2] >> 8) * p2_lit_x) >> 8);
        int32_t lit_y_begin = (((begin_mix[0] >> 8) * p0_lit_y) >> 8) + (((begin_mix[1] >> 8) * p1_lit_y) >> 8) + (((begin_mix[2] >> 8) * p2_lit_y) >> 8);
        
        int32_t lit_x_end = (((end_mix[0] >> 8) * p0_lit_x) >> 8) + (((end_mix[1] >> 8) * p1_lit_x) >> 8) + (((end_mix[2] >> 8) * p2_lit_x) >> 8);
        int32_t lit_y_end = (((end_mix[0] >> 8) * p0_lit_y) >> 8) + (((end_mix[1] >> 8) * p1_lit_y) >> 8) + (((end_mix[2] >> 8) * p2_lit_y) >> 8);
        
        int32_t lit_x_adv = (lit_x_end - lit_x_begin) / span_length;
        int32_t lit_y_adv = (lit_y_end - lit_y_begin) / span_length;
        
        int32_t lit_x_mix = lit_x_begin;
        int32_t lit_y_mix = lit_y_begin;
        
        for (int32_t x = span_ptr->begin; x <= span_ptr->end; x++) {
            depth_mix += depth_adv;
            
            tex_x_mix += tex_x_adv;
            tex_y_mix += tex_y_adv;
            
            lit_x_mix += lit_x_adv;
            lit_y_mix += lit_y_adv;
            
            if (depth_buffer[y * screen_width + x] > depth_mix) continue;
            
            int32_t tex_x = (tex_x_mix >> 8) & (texture->width - 1);
            int32_t tex_y = (tex_y_mix >> 8) & (texture->height - 1);
            int32_t tex_offset = (texture->width * tex_y + tex_x) * texture->channels;
            
            int32_t lit_x = (lit_x_mix >> 8) & (lightmap->width - 1);
            int32_t lit_y = (lit_y_mix >> 8) & (lightmap->height - 1);
            int32_t lit_offset = (lightmap->width * lit_y + lit_x) * lightmap->channels;
            
            uint16_t r = (((int32_t)texture->pixels[tex_offset + 0] * (int32_t)lightmap->pixels[lit_offset + 0]) & 0xFFFF) >> (16 - 5);
            uint16_t g = (((int32_t)texture->pixels[tex_offset + 1] * (int32_t)lightmap->pixels[lit_offset + 1]) & 0xFFFF) >> (16 - 6);
            uint16_t b = (((int32_t)texture->pixels[tex_offset + 2] * (int32_t)lightmap->pixels[lit_offset + 2]) & 0xFFFF) >> (16 - 5);
            
            // overwrite with depth
            //r = depth_mix  >> (16 - 5);
            //g = depth_mix  >> (16 - 6);
            //b = depth_mix  >> (16 - 5);
            
            uint16_t c = (r << (5 + 6)) | (g << (5)) | b;
            
            depth_buffer[y * screen_width + x] = depth_mix;
            BlitDot(x, y, c);
        }
    }
}

void RasterizeTriangleShadedTextured(ScanlineBuffer* scanlines, Point2D* vertices, vec3* colors, vec3* speculars, vec2* texture_uvs, SWTexture* texture) {
    int32_t p0_tex_x = texture_uvs[0].x * (float)texture->width * (float)(0xFF + 1);
    int32_t p0_tex_y = texture_uvs[0].y * (float)texture->height * (float)(0xFF + 1);
    int32_t p1_tex_x = texture_uvs[1].x * (float)texture->width * (float)(0xFF + 1);
    int32_t p1_tex_y = texture_uvs[1].y * (float)texture->height * (float)(0xFF + 1);
    int32_t p2_tex_x = texture_uvs[2].x * (float)texture->width * (float)(0xFF + 1);
    int32_t p2_tex_y = texture_uvs[2].y * (float)texture->height * (float)(0xFF + 1);
    
    int32_t p0_col_r = fabsf(colors[0].r) * (float)(0xFFFF + 1);
    int32_t p0_col_g = fabsf(colors[0].g) * (float)(0xFFFF + 1);
    int32_t p0_col_b = fabsf(colors[0].b) * (float)(0xFFFF + 1);
    int32_t p1_col_r = fabsf(colors[1].r) * (float)(0xFFFF + 1);
    int32_t p1_col_g = fabsf(colors[1].g) * (float)(0xFFFF + 1);
    int32_t p1_col_b = fabsf(colors[1].b) * (float)(0xFFFF + 1);
    int32_t p2_col_r = fabsf(colors[2].r) * (float)(0xFFFF + 1);
    int32_t p2_col_g = fabsf(colors[2].g) * (float)(0xFFFF + 1);
    int32_t p2_col_b = fabsf(colors[2].b) * (float)(0xFFFF + 1);
    
    int32_t p0_spc_r = fabsf(speculars[0].r) * (float)(0xFFFF + 1);
    int32_t p0_spc_g = fabsf(speculars[0].g) * (float)(0xFFFF + 1);
    int32_t p0_spc_b = fabsf(speculars[0].b) * (float)(0xFFFF + 1);
    int32_t p1_spc_r = fabsf(speculars[1].r) * (float)(0xFFFF + 1);
    int32_t p1_spc_g = fabsf(speculars[1].g) * (float)(0xFFFF + 1);
    int32_t p1_spc_b = fabsf(speculars[1].b) * (float)(0xFFFF + 1);
    int32_t p2_spc_r = fabsf(speculars[2].r) * (float)(0xFFFF + 1);
    int32_t p2_spc_g = fabsf(speculars[2].g) * (float)(0xFFFF + 1);
    int32_t p2_spc_b = fabsf(speculars[2].b) * (float)(0xFFFF + 1);
    
    Scanline* span_ptr = scanlines->scanlines;
    for (int32_t y = scanlines->start_y; y < (scanlines->start_y + scanlines->scanline_length); y++, span_ptr++) {
        int32_t begin_mix[3] = {0, 0, 0};
        int32_t end_mix[3] = {0, 0, 0};
        
        begin_mix[span_ptr->begin_p0] = span_ptr->begin_adv;
        begin_mix[span_ptr->begin_p1] = 0xFFFF - span_ptr->begin_adv;
        
        end_mix[span_ptr->end_p0] = span_ptr->end_adv;
        end_mix[span_ptr->end_p1] = 0xFFFF - span_ptr->end_adv;
        
        int32_t span_length = span_ptr->end - span_ptr->begin + 1;
        
        if (span_length <= 0) continue;
        
        int32_t depth_begin = ((begin_mix[0] * vertices[0].depth) >> 16) + ((begin_mix[1] * vertices[1].depth) >> 16) + ((begin_mix[2] * vertices[2].depth) >> 16);
        int32_t depth_end = ((end_mix[0] * vertices[0].depth) >> 16) + ((end_mix[1] * vertices[1].depth) >> 16) + ((end_mix[2] * vertices[2].depth) >> 16);
        
        int32_t depth_adv = (depth_end - depth_begin) / span_length;
        int32_t depth_mix = depth_begin;
        
        int32_t tex_x_begin = (((begin_mix[0] >> 8) * p0_tex_x) >> 8) + (((begin_mix[1] >> 8) * p1_tex_x) >> 8) + (((begin_mix[2] >> 8) * p2_tex_x) >> 8);
        int32_t tex_y_begin = (((begin_mix[0] >> 8) * p0_tex_y) >> 8) + (((begin_mix[1] >> 8) * p1_tex_y) >> 8) + (((begin_mix[2] >> 8) * p2_tex_y) >> 8);
        
        int32_t tex_x_end = (((end_mix[0] >> 8) * p0_tex_x) >> 8) + (((end_mix[1] >> 8) * p1_tex_x) >> 8) + (((end_mix[2] >> 8) * p2_tex_x) >> 8);
        int32_t tex_y_end = (((end_mix[0] >> 8) * p0_tex_y) >> 8) + (((end_mix[1] >> 8) * p1_tex_y) >> 8) + (((end_mix[2] >> 8) * p2_tex_y) >> 8);
        
        int32_t tex_x_adv = (tex_x_end - tex_x_begin) / span_length;
        int32_t tex_y_adv = (tex_y_end - tex_y_begin) / span_length;
        
        int32_t tex_x_mix = tex_x_begin;
        int32_t tex_y_mix = tex_y_begin;
        
        int32_t col_r_begin = (((begin_mix[0] >> 8) * p0_col_r) >> 8) + (((begin_mix[1] >> 8) * p1_col_r) >> 8) + (((begin_mix[2] >> 8) * p2_col_r) >> 8);
        int32_t col_g_begin = (((begin_mix[0] >> 8) * p0_col_g) >> 8) + (((begin_mix[1] >> 8) * p1_col_g) >> 8) + (((begin_mix[2] >> 8) * p2_col_g) >> 8);
        int32_t col_b_begin = (((begin_mix[0] >> 8) * p0_col_b) >> 8) + (((begin_mix[1] >> 8) * p1_col_b) >> 8) + (((begin_mix[2] >> 8) * p2_col_b) >> 8);
        
        int32_t col_r_end = (((end_mix[0] >> 8) * p0_col_r) >> 8) + (((end_mix[1] >> 8) * p1_col_r) >> 8) + (((end_mix[2] >> 8) * p2_col_r) >> 8);
        int32_t col_g_end = (((end_mix[0] >> 8) * p0_col_g) >> 8) + (((end_mix[1] >> 8) * p1_col_g) >> 8) + (((end_mix[2] >> 8) * p2_col_g) >> 8);
        int32_t col_b_end = (((end_mix[0] >> 8) * p0_col_b) >> 8) + (((end_mix[1] >> 8) * p1_col_b) >> 8) + (((end_mix[2] >> 8) * p2_col_b) >> 8);
        
        int32_t col_r_adv = (col_r_end - col_r_begin) / span_length;
        int32_t col_g_adv = (col_g_end - col_g_begin) / span_length;
        int32_t col_b_adv = (col_b_end - col_b_begin) / span_length;
        
        int32_t col_r_mix = col_r_begin;
        int32_t col_g_mix = col_g_begin;
        int32_t col_b_mix = col_b_begin;
        
        int32_t spc_r_begin = (((begin_mix[0] >> 8) * p0_spc_r) >> 8) + (((begin_mix[1] >> 8) * p1_spc_r) >> 8) + (((begin_mix[2] >> 8) * p2_spc_r) >> 8);
        int32_t spc_g_begin = (((begin_mix[0] >> 8) * p0_spc_g) >> 8) + (((begin_mix[1] >> 8) * p1_spc_g) >> 8) + (((begin_mix[2] >> 8) * p2_spc_g) >> 8);
        int32_t spc_b_begin = (((begin_mix[0] >> 8) * p0_spc_b) >> 8) + (((begin_mix[1] >> 8) * p1_spc_b) >> 8) + (((begin_mix[2] >> 8) * p2_spc_b) >> 8);
        
        int32_t spc_r_end = (((end_mix[0] >> 8) * p0_spc_r) >> 8) + (((end_mix[1] >> 8) * p1_spc_r) >> 8) + (((end_mix[2] >> 8) * p2_spc_r) >> 8);
        int32_t spc_g_end = (((end_mix[0] >> 8) * p0_spc_g) >> 8) + (((end_mix[1] >> 8) * p1_spc_g) >> 8) + (((end_mix[2] >> 8) * p2_spc_g) >> 8);
        int32_t spc_b_end = (((end_mix[0] >> 8) * p0_spc_b) >> 8) + (((end_mix[1] >> 8) * p1_spc_b) >> 8) + (((end_mix[2] >> 8) * p2_spc_b) >> 8);
        
        int32_t spc_r_adv = (spc_r_end - spc_r_begin) / span_length;
        int32_t spc_g_adv = (spc_g_end - spc_g_begin) / span_length;
        int32_t spc_b_adv = (spc_b_end - spc_b_begin) / span_length;
        
        int32_t spc_r_mix = spc_r_begin;
        int32_t spc_g_mix = spc_g_begin;
        int32_t spc_b_mix = spc_b_begin;
        
        for (int32_t x = span_ptr->begin; x <= span_ptr->end; x++) {
            depth_mix += depth_adv;
            
            tex_x_mix += tex_x_adv;
            tex_y_mix += tex_y_adv;
            
            col_r_mix += col_r_adv;
            col_g_mix += col_g_adv;
            col_b_mix += col_b_adv;
            
            spc_r_mix += spc_r_adv;
            spc_g_mix += spc_g_adv;
            spc_b_mix += spc_b_adv;
            
            if (depth_buffer[y * screen_width + x] > depth_mix) continue;
            
            int32_t tex_x = (tex_x_mix >> 8) & (texture->width - 1);
            int32_t tex_y = (tex_y_mix >> 8) & (texture->height - 1);
            int32_t tex_offset = (texture->width * tex_y + tex_x) * texture->channels;
            
            int32_t r = (((int32_t)texture->pixels[tex_offset + 0] * (col_r_mix >> 8) + spc_r_mix)) >> (16 - 5);
            int32_t g = (((int32_t)texture->pixels[tex_offset + 1] * (col_g_mix >> 8) + spc_g_mix)) >> (16 - 6);
            int32_t b = (((int32_t)texture->pixels[tex_offset + 2] * (col_b_mix >> 8) + spc_b_mix)) >> (16 - 5);
            
            if (r & ~0x1F) r = 0x1F;
            if (g & ~0x3F) g = 0x3F;
            if (b & ~0x1F) b = 0x1F;
            
            uint16_t c = (r << (5 + 6)) | (g << (5)) | b;
            
            depth_buffer[y * screen_width + x] = depth_mix;
            BlitDot(x, y, c);
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

struct ClipPoint {
    vec4 coordinate;
    vec3 barycentric;
};

struct ClipTriangle {
    ClipPoint points[3];
};

ClipPoint ClipSingleClipPointLineLeftPlane(ClipPoint outside, ClipPoint inside) {
    float a = (outside.coordinate.w + outside.coordinate.x) / ((outside.coordinate.w + outside.coordinate.x) - (inside.coordinate.w + inside.coordinate.x));
    return {(1.0f - a) * outside.coordinate + a * inside.coordinate, (1.0f - a) * outside.barycentric + a * inside.barycentric};
}

ClipPoint ClipSingleClipPointLineRightPlane(ClipPoint outside, ClipPoint inside) {
    float a = (outside.coordinate.w - outside.coordinate.x) / ((outside.coordinate.w - outside.coordinate.x) - (inside.coordinate.w - inside.coordinate.x));
    return {(1.0f - a) * outside.coordinate + a * inside.coordinate, (1.0f - a) * outside.barycentric + a * inside.barycentric};
}

ClipPoint ClipSingleClipPointLineBottomPlane(ClipPoint outside, ClipPoint inside) {
    float a = (outside.coordinate.w + outside.coordinate.y) / ((outside.coordinate.w + outside.coordinate.y) - (inside.coordinate.w + inside.coordinate.y));
    return {(1.0f - a) * outside.coordinate + a * inside.coordinate, (1.0f - a) * outside.barycentric + a * inside.barycentric};
}

ClipPoint ClipSingleClipPointLineTopPlane(ClipPoint outside, ClipPoint inside) {
    float a = (outside.coordinate.w - outside.coordinate.y) / ((outside.coordinate.w - outside.coordinate.y) - (inside.coordinate.w - inside.coordinate.y));
    return {(1.0f - a) * outside.coordinate + a * inside.coordinate, (1.0f - a) * outside.barycentric + a * inside.barycentric};
}

ClipPoint ClipSingleClipPointLineNearPlane(ClipPoint outside, ClipPoint inside) {
    float a = (outside.coordinate.w + outside.coordinate.z) / ((outside.coordinate.w + outside.coordinate.z) - (inside.coordinate.w + inside.coordinate.z));
    return {(1.0f - a) * outside.coordinate + a * inside.coordinate, (1.0f - a) * outside.barycentric + a * inside.barycentric};
}

ClipPoint ClipSingleClipPointLineFarPlane(ClipPoint outside, ClipPoint inside) {
    float a = (outside.coordinate.w - outside.coordinate.z) / ((outside.coordinate.w - outside.coordinate.z) - (inside.coordinate.w - inside.coordinate.z));
    return {(1.0f - a) * outside.coordinate + a * inside.coordinate, (1.0f - a) * outside.barycentric + a * inside.barycentric};
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

bool TriangleVisible(vec4 p0, vec4 p1, vec4 p2) {    
    // line outside left plane
    if (p0.w + p0.x < 0.0f && p1.w + p1.x < 0.0f && p2.w + p2.x < 0.0f) {
        return false;
    }

    // line outside right plane
    if (p0.w - p0.x < 0.0f && p1.w - p1.x < 0.0f && p2.w - p2.x < 0.0f) {
        return false;
    }

    // line outside bottom plane
    if (p0.w + p0.y < 0.0f && p1.w + p1.y < 0.0f && p2.w + p2.y < 0.0f) {
        return false;
    }

    // line outside top plane
    if (p0.w - p0.y < 0.0f && p1.w - p1.y < 0.0f && p2.w - p2.y < 0.0f) {
        return false;
    }

    // line outside near plane
    if (p0.w + p0.z < 0.0f && p1.w + p1.z < 0.0f && p2.w + p2.z < 0.0f) {
        return false;
    }
    
    // line outside far plane
    if (p0.w - p0.z < 0.0f && p1.w - p1.z < 0.0f && p2.w - p2.z < 0.0f) {
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

size_t ClipTriangleList(ClipTriangle* triangles) {
    // accidentally messed up calculations, this array should actually be 2^6? large?
    // we could also fold in tri_list_1 into triangles
    ClipTriangle tri_list_1[12] = {*triangles};
    ClipTriangle tri_list_2[12];
    size_t tri_count_working = 1;
    size_t tri_count_buffered = 0;
    ClipTriangle* working_list = tri_list_1;
    ClipTriangle* buffer_list = tri_list_2;
    
    auto clip_lambda = [&](auto outside_func, auto clip_func) {
        for (size_t i = 0; i < tri_count_working; i++) {
            ClipPoint* in_points[3] = {nullptr, nullptr, nullptr};
            ClipPoint* out_points[3] = {nullptr, nullptr, nullptr};
            size_t in_point_count = 0;
            size_t out_point_count = 0;
            
            for (size_t p = 0; p < 3; p++) {
                if (outside_func(working_list[i].points[p].coordinate)) {
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
                    ClipPoint clipped0 = clip_func(*out_points[0], *in_points[0]);
                    ClipPoint clipped1 = clip_func(*out_points[0], *in_points[1]);
                    
                    buffer_list[tri_count_buffered++] = {{*in_points[1], *in_points[0], clipped0}};
                    buffer_list[tri_count_buffered++] = {{*in_points[1], clipped0, clipped1}};
                    
                    } break;
                case 2: {
                    ClipPoint clipped0 = clip_func(*out_points[0], *in_points[0]);
                    ClipPoint clipped1 = clip_func(*out_points[1], *in_points[0]);
                    
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
    
    clip_lambda(PointOutsideLeftPlane, ClipSingleClipPointLineLeftPlane);
    clip_lambda(PointOutsideRightPlane, ClipSingleClipPointLineRightPlane);
    clip_lambda(PointOutsideBottomPlane, ClipSingleClipPointLineBottomPlane);
    clip_lambda(PointOutsideTopPlane, ClipSingleClipPointLineTopPlane);
    clip_lambda(PointOutsideNearPlane, ClipSingleClipPointLineNearPlane);
    clip_lambda(PointOutsideFarPlane, ClipSingleClipPointLineFarPlane);
    
    for (size_t i = 0; i < tri_count_working; i++) {
        triangles[i] = working_list[i];
    }
    
    return tri_count_working;
}

void RenderFrame() {
    memset(depth_buffer, 0, screen_width * screen_height * sizeof(float));

    if (clear_screen) {
        uint16_t clear_color = IntColor(screen_clear_color);
        for (int i = 0; i < screen_width * screen_height; i++) {
            screen_buffer[i] = clear_color;
        }
    }
    
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
                    
                    if (!TriangleVisible(pr0, pr1, pr2)) continue;
                    
                    // backface culling
                    //vec2 ab = {pr1.x/pr1.w - pr0.x/pr0.w, pr1.y/pr1.w - pr0.y/pr0.w};
                    //vec2 ac = {pr2.x/pr2.w - pr0.x/pr0.w, pr2.y/pr2.w - pr0.y/pr0.w};
                    //if (ab.x * ac.y - ac.x * ab.y < 0.0f) continue;
                    
                    if (PointVisible(pr0) && PointVisible(pr1) && PointVisible(pr2)) {
                        PerspectiveDivision(pr0);
                        PerspectiveDivision(pr1);
                        PerspectiveDivision(pr2);
        
                        // backface culling
                        vec2 ab = {pr1.x - pr0.x, pr1.y - pr0.y};
                        vec2 ac = {pr2.x - pr0.x, pr2.y - pr0.y};
                        if (ab.x * ac.y - ac.x * ab.y < 0.0f) continue;
    
                        auto [px0, py0] = ClipSpaceToScreenSpace(pr0);
                        auto [px1, py1] = ClipSpaceToScreenSpace(pr1);
                        auto [px2, py2] = ClipSpaceToScreenSpace(pr2);
                        
                        float depth0 = (pr0.z/pr0.w);
                        float depth1 = (pr1.z/pr1.w);
                        float depth2 = (pr2.z/pr2.w);
                        
                        if (depth0 < 0.0f) depth0 = 0.0f;
                        if (depth1 < 0.0f) depth1 = 0.0f;
                        if (depth2 < 0.0f) depth2 = 0.0f;
                        
                        if (depth0 > 1.0f) depth0 = 1.0f;
                        if (depth1 > 1.0f) depth1 = 1.0f;
                        if (depth2 > 1.0f) depth2 = 1.0f;
                        
                        uint32_t pz0 = (float)0xFFFF * depth0;
                        uint32_t pz1 = (float)0xFFFF * depth1;
                        uint32_t pz2 = (float)0xFFFF * depth2;
                        
                        Point2D ps[3] = {{px0, py0, pz0}, {px1, py1, pz1}, {px2, py2, pz2}};
                        
                        vec2 texture_uvs[3] = {p0.tex, p1.tex, p2.tex};
                        vec2 lightmap_uvs[3] = {p0.lit, p1.lit, p2.lit};
                        
                        ScanlineConvertTriangle(scanlines, ps);
                        RasterizeTriangleLightmapped(scanlines, ps, IntColor(COLOR_WHITE), texture_uvs, lightmap_uvs, entry->material->texture, entry->lightmap);
                    } else {
                        ClipTriangle clipped[12] = {{{{pr0, {1, 0, 0}}, {pr1, {0, 1, 0}}, {pr2, {0, 0, 1}}}}};
                        size_t tri_count = ClipTriangleList(clipped);
                        
                        for (size_t i = 0; i < tri_count; i++) {
                            PerspectiveDivision(clipped[i].points[0].coordinate);
                            PerspectiveDivision(clipped[i].points[1].coordinate);
                            PerspectiveDivision(clipped[i].points[2].coordinate);
        
                            // backface culling doesn't work, since the triangle
                            // triangle windigs get messed up? or something like
                            // that.
                            // TODO: fix
                            //vec2 ab = {clipped[i].points[1].coordinate.x - clipped[i].points[0].coordinate.x, clipped[i].points[1].coordinate.y - clipped[i].points[0].coordinate.y};
                            //vec2 ac = {clipped[i].points[2].coordinate.x - clipped[i].points[0].coordinate.x, clipped[i].points[2].coordinate.y - clipped[i].points[0].coordinate.y};
                            //if (ab.x * ac.y - ac.x * ab.y < 0.0f) continue;
        
                            auto [px0, py0] = ClipSpaceToScreenSpace(clipped[i].points[0].coordinate);
                            auto [px1, py1] = ClipSpaceToScreenSpace(clipped[i].points[1].coordinate);
                            auto [px2, py2] = ClipSpaceToScreenSpace(clipped[i].points[2].coordinate);
                            
                            float depth0 = (clipped[i].points[0].coordinate.z/clipped[i].points[0].coordinate.w);
                            float depth1 = (clipped[i].points[1].coordinate.z/clipped[i].points[1].coordinate.w);
                            float depth2 = (clipped[i].points[2].coordinate.z/clipped[i].points[2].coordinate.w);
                            
                            if (depth0 < 0.0f) depth0 = 0.0f;
                            if (depth1 < 0.0f) depth1 = 0.0f;
                            if (depth2 < 0.0f) depth2 = 0.0f;
                            
                            if (depth0 > 1.0f) depth0 = 1.0f;
                            if (depth1 > 1.0f) depth1 = 1.0f;
                            if (depth2 > 1.0f) depth2 = 1.0f;
                            
                            uint32_t pz0 = (float)0xFFFF * depth0;
                            uint32_t pz1 = (float)0xFFFF * depth1;
                            uint32_t pz2 = (float)0xFFFF * depth2;
   
                            const float bary00 = clipped[i].points[0].barycentric.x;
                            const float bary01 = clipped[i].points[0].barycentric.y;
                            const float bary02 = clipped[i].points[0].barycentric.z;
                            const float bary10 = clipped[i].points[1].barycentric.x;
                            const float bary11 = clipped[i].points[1].barycentric.y;
                            const float bary12 = clipped[i].points[1].barycentric.z;
                            const float bary20 = clipped[i].points[2].barycentric.x;
                            const float bary21 = clipped[i].points[2].barycentric.y;
                            const float bary22 = clipped[i].points[2].barycentric.z;
                            
                            vec2 texture_uvs[3] = {p0.tex * bary00 + p1.tex * bary01 + p2.tex * bary02, 
                                                   p0.tex * bary10 + p1.tex * bary11 + p2.tex * bary12,
                                                   p0.tex * bary20 + p1.tex * bary21 + p2.tex * bary22};
                            vec2 lightmap_uvs[3] = {p0.lit * bary00 + p1.lit * bary01 + p2.lit * bary02, 
                                                    p0.lit * bary10 + p1.lit * bary11 + p2.lit * bary12,
                                                    p0.lit * bary20 + p1.lit * bary21 + p2.lit * bary22};
                            
                            Point2D ps[3] = {{px0, py0, pz0}, {px1, py1, pz1}, {px2, py2, pz2}};
                            ScanlineConvertTriangle(scanlines, ps);
                            RasterizeTriangleLightmapped(scanlines, ps, IntColor(COLOR_WHITE), texture_uvs, lightmap_uvs, entry->material->texture, entry->lightmap);
                        }
                    }
                    
                    
                }
                
                break;
            case SW_DYNAMIC_BLENDED:
                if (!entry->index_array) continue;

                for (size_t i = 0; i < entry->index_length; i++) {
                    uint32_t triangle = (entry->index_offset + i) * 3;
                    
                    DynamicVertex& p0 = ((DynamicVertex*)entry->vertex_array->vertices)[entry->index_array->indices[triangle + 0]];
                    DynamicVertex& p1 = ((DynamicVertex*)entry->vertex_array->vertices)[entry->index_array->indices[triangle + 1]];
                    DynamicVertex& p2 = ((DynamicVertex*)entry->vertex_array->vertices)[entry->index_array->indices[triangle + 2]];
                    
                    vec4 local_pos0 = vec4(p0.pos, 1.0f);
                    vec4 local_pos1 = vec4(p1.pos, 1.0f);
                    vec4 local_pos2 = vec4(p2.pos, 1.0f);
                    
                    if (entry->pose) {
                        vec4 local_pos00 = entry->pose->pose[p0.ind.x] * local_pos0;
                        vec4 local_pos01 = entry->pose->pose[p0.ind.y] * local_pos0;
                        vec4 local_pos02 = entry->pose->pose[p0.ind.z] * local_pos0;
                        vec4 local_pos03 = entry->pose->pose[p0.ind.w] * local_pos0;
                        
                        vec4 local_pos10 = entry->pose->pose[p1.ind.x] * local_pos1;
                        vec4 local_pos11 = entry->pose->pose[p1.ind.y] * local_pos1;
                        vec4 local_pos12 = entry->pose->pose[p1.ind.z] * local_pos1;
                        vec4 local_pos13 = entry->pose->pose[p1.ind.w] * local_pos1;
                        
                        vec4 local_pos20 = entry->pose->pose[p2.ind.x] * local_pos2;
                        vec4 local_pos21 = entry->pose->pose[p2.ind.y] * local_pos2;
                        vec4 local_pos22 = entry->pose->pose[p2.ind.z] * local_pos2;
                        vec4 local_pos23 = entry->pose->pose[p2.ind.w] * local_pos2;
                        
                        local_pos0 = local_pos00 * p0.wgt.x + local_pos01 * p0.wgt.y + local_pos02 * p0.wgt.z + local_pos03 * p0.wgt.w;
                        local_pos1 = local_pos10 * p1.wgt.x + local_pos11 * p1.wgt.y + local_pos12 * p1.wgt.z + local_pos13 * p1.wgt.w;
                        local_pos2 = local_pos20 * p2.wgt.x + local_pos21 * p2.wgt.y + local_pos22 * p2.wgt.z + local_pos23 * p2.wgt.w;
                    }
                    
                    const vec4 world_pos0 = entry->matrix * local_pos0;
                    const vec4 world_pos1 = entry->matrix * local_pos1;
                    const vec4 world_pos2 = entry->matrix * local_pos2;
                    
                    vec4 pos0 = layers[0].projection_matrix * layers[0].view_matrix * world_pos0;
                    vec4 pos1 = layers[0].projection_matrix * layers[0].view_matrix * world_pos1;
                    vec4 pos2 = layers[0].projection_matrix * layers[0].view_matrix * world_pos2;
                    
                    if (!TriangleVisible(pos0, pos1, pos2)) continue;
                    
                    // backface culling
                    vec2 ab = {pos1.x/pos1.w - pos0.x/pos0.w, pos1.y/pos1.w - pos0.y/pos0.w};
                    vec2 ac = {pos2.x/pos2.w - pos0.x/pos0.w, pos2.y/pos2.w - pos0.y/pos0.w};
                    if (ab.x * ac.y - ac.x * ab.y < 0.0f) continue;
                    
                    vec4 local_nrm0 = vec4(p0.nrm, 0.0f);
                    vec4 local_nrm1 = vec4(p1.nrm, 0.0f);
                    vec4 local_nrm2 = vec4(p2.nrm, 0.0f);
                    
                    if (entry->pose) {
                        vec4 local_nrm00 = entry->pose->pose[p0.ind.x] * local_nrm0;
                        vec4 local_nrm01 = entry->pose->pose[p0.ind.y] * local_nrm0;
                        vec4 local_nrm02 = entry->pose->pose[p0.ind.z] * local_nrm0;
                        vec4 local_nrm03 = entry->pose->pose[p0.ind.w] * local_nrm0;
                        
                        vec4 local_nrm10 = entry->pose->pose[p1.ind.x] * local_nrm1;
                        vec4 local_nrm11 = entry->pose->pose[p1.ind.y] * local_nrm1;
                        vec4 local_nrm12 = entry->pose->pose[p1.ind.z] * local_nrm1;
                        vec4 local_nrm13 = entry->pose->pose[p1.ind.w] * local_nrm1;
                        
                        vec4 local_nrm20 = entry->pose->pose[p2.ind.x] * local_nrm2;
                        vec4 local_nrm21 = entry->pose->pose[p2.ind.y] * local_nrm2;
                        vec4 local_nrm22 = entry->pose->pose[p2.ind.z] * local_nrm2;
                        vec4 local_nrm23 = entry->pose->pose[p2.ind.w] * local_nrm2;
                        
                        local_nrm0 = local_nrm00 * p0.wgt.x + local_nrm01 * p0.wgt.y + local_nrm02 * p0.wgt.z + local_nrm03 * p0.wgt.w;
                        local_nrm1 = local_nrm10 * p1.wgt.x + local_nrm11 * p1.wgt.y + local_nrm12 * p1.wgt.z + local_nrm13 * p1.wgt.w;
                        local_nrm2 = local_nrm20 * p2.wgt.x + local_nrm21 * p2.wgt.y + local_nrm22 * p2.wgt.z + local_nrm23 * p2.wgt.w;
                    }
                    
                    const vec3 world_nrm0 = glm::normalize(vec3(entry->matrix * local_nrm0));
                    const vec3 world_nrm1 = glm::normalize(vec3(entry->matrix * local_nrm1));
                    const vec3 world_nrm2 = glm::normalize(vec3(entry->matrix * local_nrm2));
                    
                    const vec3 view_dir0 = normalize(layers[0].view_position - vec3(world_pos0));
                    const vec3 view_dir1 = normalize(layers[0].view_position - vec3(world_pos1));
                    const vec3 view_dir2 = normalize(layers[0].view_position - vec3(world_pos2));
                    
                    vec3 color0, color1, color2;
                    if (entry->flags & FLAG_NO_DIRECTIONAL) {
                        color0 = {0.0f, 0.0f, 0.0f};
                        color1 = {0.0f, 0.0f, 0.0f};
                        color2 = {0.0f, 0.0f, 0.0f};
                    } else {
                        color0 = layers[0].ambient_color + layers[0].sun_color * glm::max(glm::dot(world_nrm0, normalize(layers[0].sun_direction)), 0.0f);
                        color1 = layers[0].ambient_color + layers[0].sun_color * glm::max(glm::dot(world_nrm1, normalize(layers[0].sun_direction)), 0.0f);
                        color2 = layers[0].ambient_color + layers[0].sun_color * glm::max(glm::dot(world_nrm2, normalize(layers[0].sun_direction)), 0.0f);
                    }
                    
                    vec3 spec_color0 = {0.0f, 0.0f, 0.0f};
                    vec3 spec_color1 = {0.0f, 0.0f, 0.0f};
                    vec3 spec_color2 = {0.0f, 0.0f, 0.0f};
                    
                    for (size_t i = 0; i < 4; i++) {
                        SWLight* light = entry->lights[i];

                        if (!light) continue;
                        
                        vec3 light_vec0 = light->location - vec3(world_pos0);
                        vec3 light_vec1 = light->location - vec3(world_pos1);
                        vec3 light_vec2 = light->location - vec3(world_pos2);
                        float dist0 = glm::length(light_vec0);
                        float dist1 = glm::length(light_vec1);
                        float dist2 = glm::length(light_vec2);
                        float strn0 = glm::max(glm::dot(glm::normalize(light_vec0), world_nrm0), 0.0f);
                        float strn1 = glm::max(glm::dot(glm::normalize(light_vec1), world_nrm1), 0.0f);
                        float strn2 = glm::max(glm::dot(glm::normalize(light_vec2), world_nrm2), 0.0f);
                        float attn0 = strn0 * (1.0f / (1.0f + 0.09f * dist0 + 0.032f * (dist0 * dist0)));
                        float attn1 = strn1 * (1.0f / (1.0f + 0.09f * dist0 + 0.032f * (dist1 * dist1)));
                        float attn2 = strn2 * (1.0f / (1.0f + 0.09f * dist0 + 0.032f * (dist2 * dist2)));
                        float drct0 = glm::clamp(glm::pow(glm::max(glm::dot(light->direction, -glm::normalize(light_vec0)), 0.0f), light->exponent), 0.0f, 1.0f);
                        float drct1 = glm::clamp(glm::pow(glm::max(glm::dot(light->direction, -glm::normalize(light_vec1)), 0.0f), light->exponent), 0.0f, 1.0f);
                        float drct2 = glm::clamp(glm::pow(glm::max(glm::dot(light->direction, -glm::normalize(light_vec2)), 0.0f), light->exponent), 0.0f, 1.0f);
                        
                        float spec0 = glm::pow(glm::max(glm::dot(view_dir0, glm::reflect(-glm::normalize(light_vec0), world_nrm0)), 0.0f), entry->specular_exponent);
                        float spec1 = glm::pow(glm::max(glm::dot(view_dir1, glm::reflect(-glm::normalize(light_vec1), world_nrm1)), 0.0f), entry->specular_exponent);
                        float spec2 = glm::pow(glm::max(glm::dot(view_dir2, glm::reflect(-glm::normalize(light_vec2), world_nrm2)), 0.0f), entry->specular_exponent);
                        
                        color0 += light->color * attn0 * drct0;
                        color1 += light->color * attn1 * drct1;
                        color2 += light->color * attn2 * drct2;
                        
                        spec_color0 += entry->specular_weight * spec0 * light->color * drct0;
                        spec_color1 += entry->specular_weight * spec1 * light->color * drct1;
                        spec_color2 += entry->specular_weight * spec2 * light->color * drct2;
                    }
                    
                    color0 += (1.0f - entry->specular_transparency) * spec_color0;
                    color1 += (1.0f - entry->specular_transparency) * spec_color1;
                    color2 += (1.0f - entry->specular_transparency) * spec_color2;
                    
                    if (PointVisible(pos0) && PointVisible(pos1) && PointVisible(pos2)) {
                        PerspectiveDivision(pos0);
                        PerspectiveDivision(pos1);
                        PerspectiveDivision(pos2);
    
                        auto [px0, py0] = ClipSpaceToScreenSpace(pos0);
                        auto [px1, py1] = ClipSpaceToScreenSpace(pos1);
                        auto [px2, py2] = ClipSpaceToScreenSpace(pos2);
                        
                        uint32_t pz0 = (float)0xFFFF * (pos0.z/pos0.w);
                        uint32_t pz1 = (float)0xFFFF * (pos1.z/pos1.w);
                        uint32_t pz2 = (float)0xFFFF * (pos2.z/pos2.w);
                        
                        Point2D ps[3] = {{px0, py0, pz0}, {px1, py1, pz1}, {px2, py2, pz2}};
                        
                        vec2 texture_uvs[3] = {p0.tex, p1.tex, p2.tex};
                        vec3 colors[3] = {color0, color1, color2};
                        vec3 speculars[3] = {spec_color0, spec_color1, spec_color2};

                        ScanlineConvertTriangle(scanlines, ps);
                        RasterizeTriangleShadedTextured(scanlines, ps, colors, speculars, texture_uvs, entry->material->texture);
                    } else {
                        ClipTriangle clipped[12] = {{{{pos0, {1, 0, 0}}, {pos1, {0, 1, 0}}, {pos2, {0, 0, 1}}}}};
                        size_t tri_count = ClipTriangleList(clipped);
                        
                        for (size_t i = 0; i < tri_count; i++) {
                            PerspectiveDivision(clipped[i].points[0].coordinate);
                            PerspectiveDivision(clipped[i].points[1].coordinate);
                            PerspectiveDivision(clipped[i].points[2].coordinate);
        
                            auto [px0, py0] = ClipSpaceToScreenSpace(clipped[i].points[0].coordinate);
                            auto [px1, py1] = ClipSpaceToScreenSpace(clipped[i].points[1].coordinate);
                            auto [px2, py2] = ClipSpaceToScreenSpace(clipped[i].points[2].coordinate);
                            
                            uint32_t pz0 = (float)0xFFFF * (clipped[i].points[0].coordinate.z/clipped[i].points[0].coordinate.w);
                            uint32_t pz1 = (float)0xFFFF * (clipped[i].points[1].coordinate.z/clipped[i].points[1].coordinate.w);
                            uint32_t pz2 = (float)0xFFFF * (clipped[i].points[2].coordinate.z/clipped[i].points[2].coordinate.w);
   
                            const float bary00 = clipped[i].points[0].barycentric.x;
                            const float bary01 = clipped[i].points[0].barycentric.y;
                            const float bary02 = clipped[i].points[0].barycentric.z;
                            const float bary10 = clipped[i].points[1].barycentric.x;
                            const float bary11 = clipped[i].points[1].barycentric.y;
                            const float bary12 = clipped[i].points[1].barycentric.z;
                            const float bary20 = clipped[i].points[2].barycentric.x;
                            const float bary21 = clipped[i].points[2].barycentric.y;
                            const float bary22 = clipped[i].points[2].barycentric.z;
                            
                            vec2 texture_uvs[3] = {p0.tex * bary00 + p1.tex * bary01 + p2.tex * bary02, 
                                                   p0.tex * bary10 + p1.tex * bary11 + p2.tex * bary12,
                                                   p0.tex * bary20 + p1.tex * bary21 + p2.tex * bary22};
                            vec3 colors[3] = {color0 * bary00 + color1 * bary01 + color2 * bary02, 
                                              color0 * bary10 + color1 * bary11 + color2 * bary12,
                                              color0 * bary20 + color1 * bary21 + color2 * bary22};
                            vec3 speculars[3] = {spec_color0 * bary00 + spec_color1 * bary01 + spec_color2 * bary02, 
                                                 spec_color0 * bary10 + spec_color1 * bary11 + spec_color2 * bary12,
                                                 spec_color0 * bary20 + spec_color1 * bary21 + spec_color2 * bary22};
                            
                            Point2D ps[3] = {{px0, py0, pz0}, {px1, py1, pz1}, {px2, py2, pz2}};
                            ScanlineConvertTriangle(scanlines, ps);
                            RasterizeTriangleShadedTextured(scanlines, ps, colors, speculars, texture_uvs, entry->material->texture);
                        }
                    }
                    
                    
                }
                
                break;
                
            case SW_STATIC_COLORED:
                
                if (!(entry->flags & FLAG_DRAW_LINES)) continue;
                
                for (size_t i = 0; i < entry->index_length / 2; i++) {
                    ColorVertex& p0 = ((ColorVertex*)entry->vertex_array->vertices)[i * 2 + 0];
                    ColorVertex& p1 = ((ColorVertex*)entry->vertex_array->vertices)[i * 2 + 1];
                    
                    vec4 pr0 = layers[0].projection_matrix * layers[0].view_matrix * entry->matrix * vec4(p0.pos, 1.0f);
                    vec4 pr1 = layers[0].projection_matrix * layers[0].view_matrix * entry->matrix * vec4(p1.pos, 1.0f);
                    
                    ClipRenderLine(pr0, pr1, IntColor(p0.col));
                }
                
                break;
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

void SetEnvironmentMap(drawlistentry_t entry, texturehandle_t environmentmap) {
    entry.sw->environmentmap = environmentmap.sw_texture;
}

void SetSphericalHarmonic(drawlistentry_t entry, sphericalharmonic_t harmonic) {
    entry.sw->harmonic = harmonic;
}

void SetDrawListAABB(drawlistentry_t entry, vec3 min, vec3 max) {
    //entry.sw->aabb_min = min;
    //entry.sw->aabb_max = max;
}

void SetDrawListColors(drawlistentry_t entry, size_t count, vec4* color) {
    entry.sw->color = *color;
}

void SetDrawListTextureOffsets(drawlistentry_t entry, size_t count, vec4* offset) {
    /*for (size_t i = 0; i < count; i++) {
        entry.sw->texture_transforms[i] = offset[i];
    }*/
}


void SetDrawListSpecularities(drawlistentry_t entry, size_t count, float* weight, float* exponent, float* transparency) {
    entry.sw->specular_weight = *weight;
    entry.sw->specular_exponent = *exponent;
    entry.sw->specular_transparency = *transparency;
}

void SetLights(drawlistentry_t entry, light_t* lights) {
    for (size_t i = 0; i < 4; i++) {
        entry.sw->lights[i] = lights[i].sw;
    }
}

void SetMatrix(drawlistentry_t entry, const mat4& matrix) {
    entry.sw->matrix = matrix;
}

void SetFadeDistance(drawlistentry_t entry, float near, float far) {
    //entry.sw->fade_near = near;
    //entry.sw->fade_far = far;
}

void SetDrawListVertexArray(drawlistentry_t entry, vertexarray_t vertex_array_handle) {
    entry.sw->vertex_array = vertex_array_handle.sw_vertex_array;
}

void SetDrawListSpriteArray(drawlistentry_t entry, spritearray_t sprite_array_handle) {
    
}

void SetDrawListIndexArray(drawlistentry_t entry, indexarray_t index_array_handle) {
    entry.sw->index_array = index_array_handle.sw_index_array;
}

void SetDrawListIndexRange(drawlistentry_t entry, uint32_t index_offset, uint32_t index_length) {
    entry.sw->index_offset = index_offset;
    entry.sw->index_length = index_length;
}

bool SetDrawListShader(drawlistentry_t entry, vertexformat_t vertex_format, materialtype_t material_type) {
    // TODO: check if material_type has transparency and save that fact
    return true;
}

/*void SetDrawListTextures(drawlistentry_t entry, size_t texture_count, texturehandle_t* texture) {
    entry.sw->texture = texture->sw_texture;
}*/

void SetDrawListMaterials(drawlistentry_t entry, size_t texture_count, material_t* material) {
    entry.sw->material = material->sw;
}

material_t MakeMaterial() {
    return material_t{.sw = material_list.AddNew()};
}

void DeleteMaterial(material_t material) {
    material_list.Remove(material.sw);
}

void SetMaterialTexture(material_t material, texturehandle_t texture) {
    material.sw->texture = texture.sw_texture;
}

void SetMaterialColor(material_t material, vec4 color) {
    material.sw->color = color;
}

void SetMaterialSpecularWeight(material_t material, float weight) {
    material.sw->specular_weight = weight;
}

void SetMaterialSpecularExponent(material_t material, float exponent) {
    material.sw->specular_exponent = exponent;
}

void SetMaterialSpecularTransparency(material_t material, float transparency) {
    material.sw->specular_transparency = transparency;
}

void SetMaterialReflectivity(material_t material, float reflectivity) {
    material.sw->reflectivity = reflectivity;
}

light_t MakeLight() {
    return light_t {.sw = light_list.AddNew()};
}

void DeleteLight(light_t light) {
    light_list.Remove(light.sw);
}

void SetLightParameters(light_t light, vec3 location, vec3 color, float distance, vec3 direction, float exponent) {
    SWLight* light_ptr = light.sw;
    
    light_ptr->location = location;
    light_ptr->color = color;
    light_ptr->distance = distance;
    light_ptr->direction = direction;
    light_ptr->exponent = exponent;
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
    texture->pixels = (uint8_t*)malloc(texture_size);
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
                verts[i].wgt = glm::normalize(*wgt);
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
}

void CreateVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array) {
    vertex_array.sw_vertex_array = vertex_arrays.AddNew();
    ParseFormat(vertex_array, vertex_format);
}

void UpdateVertexArray(vertexarray_t& vertex_array, size_t data_size, void* data) {
    if (vertex_array.sw_vertex_array->vertices) {
        free(vertex_array.sw_vertex_array->vertices);
        vertex_array.sw_vertex_array->vertices = nullptr;
    }
    
    PackVertices(vertex_array, data, data_size / vertex_array.sw_vertex_array->format.attributes[0].stride);
}

spritearray_t CreateSpriteArray() {
    return spritearray_t {.generic = nullptr};
}

void UpdateSpriteArray(spritearray_t array, size_t data_size, void* data) {
    
}

void SetViewMatrix(const mat4& matrix, layer_t layer) {
    layers[layer].view_matrix = matrix;
    layers[layer].view_position = glm::inverse(matrix) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void SetProjectionMatrix(const mat4& matrix, layer_t layer) {
    layers[layer].projection_matrix = matrix;
}

void GetScreen(char* buffer, int w, int h) {
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            uint16_t color = screen_buffer[y * screen_width + x];
            
            uint8_t r = 0xFF & ((color >> 11) << 3);
            uint8_t g = 0xFF & ((color >> 5) << 2);
            uint8_t b = 0xFF & (color << 3); 
            
            int offset = 3 * ((h - y - 1) * w + x);
            
            buffer[offset + 0] = r;
            buffer[offset + 1] = g;
            buffer[offset + 2] = b;
        }
    }
}

bool IsDebugMode() {
    return false;
}

void SetDebugMode(bool) {
    
}

void Init() {
    depth_buffer = (uint16_t*)malloc(screen_width * screen_height * sizeof(float));
    
    // initialize the default pose
    /*BLANK_POSE = PoolProxy<Render::Pose>::New();
    for (size_t i = 0; i < BONE_COUNT; i++) {
        BLANK_POSE->pose[i] = mat4(1.0f);
    }*/
    
    null_pose = PoolProxy<Render::Pose>::New();
    for (size_t i = 0; i < BONE_COUNT; i++) {
        null_pose->pose[i] = mat4(1.0f);
    }
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
