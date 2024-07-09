// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/software/software.h>
#include <render/api.h>

#include <templates/octree.h>

#include <cstdlib>
#include <cstring>

namespace tram::Render::API {

Pool<SWDrawListEntry> draw_list ("render list", 500, false);
Pool<SWLight> light_list ("light list", 200, false);
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
static float* depth_buffer = nullptr;

static std::pair<uint16_t, uint16_t> spans[1000];

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
};

template <bool set_span_first>
void MakeSpans(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t skip_first, std::pair<uint16_t, uint16_t>** span_array) {
    std::pair<uint16_t, uint16_t>* span = *span_array;

    int32_t delta_x = x1 - x0;
    int32_t advance = delta_x > 0 ? 1 : -1;

    int32_t width = abs(delta_x);
    int32_t height = y1 - y0;

    if (height <= 0) return;

    if (width == 0) {
        for (int32_t i = height - skip_first; i-- > 0; span++) {
            if (set_span_first) {
                span->first = x0;
            } else {
                span->second = x0;
            }
        }
    } else if (width == height) {
        if (skip_first) {
            x0 += advance; 
        }

        for (int32_t i = height - skip_first; i-- > 0; span++) {
            if (set_span_first) {
                span->first = x0;
            } else {
                span->second = x0;
            }
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

        for (int32_t i = height - skip_first; i-- > 0; span++) {
            if (set_span_first) {
                span->first = x0;
            } else {
                span->second = x0;
            }

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

        for (int32_t i = height - skip_first; i-- > 0; span++) {
            if (set_span_first) {
                span->first = x0;
            } else {
                span->second = x0;
            }
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
    std::pair<uint16_t, uint16_t>* span_ptr;

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
                 skip_first,
                 &span_ptr);
                 
        prev_index = this_index;
        skip_first = 0; 
    } while (this_index != max_index);

    span_ptr = spans;
    for (int32_t y = start_y; y < (start_y + span_length); y++, span_ptr++) {
        for (int32_t x = span_ptr->first; x <= span_ptr->second; x++) {
            BlitDot(x, y, color);
        }
    }
}





void RenderFrame() {
    memset(depth_buffer, 0, 800 * 600 * sizeof(float));

    if (clear_screen) {
        uint16_t clear_color = IntColor(screen_clear_color);
        for (int i = 0; i < 800 * 600; i++) {
            screen_buffer[i] = clear_color;
        }
    }
    
    for (int i = 0; i < 500; i++) {
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
    
    RasterizeTriangle(plist, IntColor(COLOR_GREEN));
    
    
    Point2D plist2[3] = {{0, 0}, {0, 100}, {100, 0}};
    
    RasterizeTriangle(plist2, IntColor(COLOR_BLUE));
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
    
}

void SetDrawListTextures(drawlistentry_t entry, size_t texture_count, texturehandle_t* texture) {
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
    /*uint32_t texture;
    uint32_t opengl_tex_format;
    
    switch (color_mode) {
        case COLORMODE_R:
            opengl_tex_format = GL_RED;
            break;
        case COLORMODE_RG:
            opengl_tex_format = GL_RG;
            break;
        case COLORMODE_RGB:
            opengl_tex_format = GL_RGB;
            break;
        case COLORMODE_RGBA:
            opengl_tex_format = GL_RGBA;
    }
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    switch (texture_filter) {
        case TEXTUREFILTER_NEAREST:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case TEXTUREFILTER_LINEAR:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case TEXTUREFILTER_LINEAR_MIPMAPPED:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
    }

    assert(data);

    glTexImage2D(GL_TEXTURE_2D, 0, opengl_tex_format, width, height, 0, opengl_tex_format, GL_UNSIGNED_BYTE, data);
        
    glGenerateMipmap(GL_TEXTURE_2D);*/
    
    return texturehandle_t {.sw_texture = nullptr};
}

void CreateIndexedVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array, indexarray_t& index_array, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data) {
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
    
    depth_buffer = (float*)malloc(800 * 600 * sizeof(float));
    
    
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
