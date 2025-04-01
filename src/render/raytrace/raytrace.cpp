// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/raytrace/raytrace.h>
#include <render/api.h>

#include <templates/octree.h>

#include <config.h>

#include <cstdlib>
#include <cstring>

namespace tram::Render::API {

Pool<RTDrawListEntry> draw_list("render list", 500);
Pool<RTLight> light_list("light list", 200);
Pool<RTTexture> texture_list("texture list", RESOURCE_LIMIT_MATERIAL);
Pool<RTMaterial> material_list("material list", RESOURCE_LIMIT_MATERIAL);
Pool<RTVertexArray> vertex_arrays("vertex_arrays list", 200);
Pool<RTIndexArray> index_arrays("index_arrays list", 200);
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

struct TreeVertex {
    vec3 pos;
    vec3 nrm;
    vec2 tex;
};

struct TreeTriangle {
    TreeVertex p0;
    TreeVertex p1;
    TreeVertex p2;
    
    vec3 tangent;
    vec3 bitangent;
    
    RTMaterial* material;
};

std::vector<TreeTriangle> tree_triangles;

int rendering_progress = 0;
bool is_rendering = false;

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
    int32_t px = (p.x + 1.0f) * 0.5f * screen_width;
    int32_t py = (1.0f - p.y) * 0.5f * screen_height;
    return {px, py};
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

vec3 InverseProject(vec3 point) {
    point.y =  screen_height - point.y;
    vec3 result = glm::unProject(point,
                                 layers[0].view_matrix,
                                 layers[0].projection_matrix,
                                 vec4(0.0f, 0.0f, screen_width, screen_height));
    return result;    
}

vec3 GetBarycentric(vec3 a, vec3 b, vec3 c, vec3 p) {
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

int FindNearestTriangle(vec3 pos, vec3 dir) {
    int nearest_intersect = -1;
    float nearest_dist = INFINITY;
    
    for (int i = 0; i < (int)tree_triangles.size(); i++) {
        vec3 v1 = tree_triangles[i].p1.pos - tree_triangles[i].p0.pos;
        vec3 v2 = tree_triangles[i].p2.pos - tree_triangles[i].p0.pos;
        
        if (glm::dot(glm::normalize(glm::cross(v1, v2)), dir) > 0.0f) continue; 
        
        vec3 intersection = RayTriangleIntersection(pos, dir, tree_triangles[i].p0.pos, tree_triangles[i].p1.pos, tree_triangles[i].p2.pos);
        
        if (intersection.x == INFINITY) {
            continue;
        }
        
        float intersection_distance = glm::distance(pos, intersection);
        
        if (intersection_distance < nearest_dist) {
            nearest_intersect = i;
            nearest_dist = intersection_distance;
        }
    }
    
    return nearest_intersect;
}

bool FindIfObstacle(vec3 pos, vec3 dir, vec3 target) {
    //int nearest_intersect = -1;
    //float nearest_dist = INFINITY;
    
    //return false;
    
    float target_distance = glm::distance(pos, target);
    
    for (int i = 0; i < (int)tree_triangles.size(); i++) {
        //if (glm::dot(tree_triangles[i].p0.nrm, dir) > 0.0f) continue; 
        
        vec3 intersection = RayTriangleIntersection(pos, dir, tree_triangles[i].p0.pos, tree_triangles[i].p1.pos, tree_triangles[i].p2.pos);
        
        if (intersection.x == INFINITY) {
            continue;
        }

        float intersection_distance = glm::distance(pos, intersection);
        
        if (intersection_distance < target_distance) {
            return true;
        }
    }
    
    return false;
}

const float epsilon = 1.0f/255.0f;

vec4 SampleTexture(RTTexture* texture, vec2 coords) {
    int tex_x = coords.x * (float)texture->width;
    int tex_y = coords.y * (float)texture->height;
    
    tex_x = tex_x % texture->width;
    tex_y = tex_y % texture->height;
    
    if (tex_x < 0) tex_x += texture->width;
    if (tex_y < 0) tex_y += texture->height;
    
    int offset = (tex_y * texture->width + tex_x);

    return texture->pixels[offset];
}

vec3 FindColorFromRay(vec3 pos, vec3 dir, int cap) {
    int nearest_intersect = FindNearestTriangle(pos, dir);

    if (nearest_intersect == -1) {
        return {0, 0, 0}; // or return clear color???
    }

    const auto& tri = tree_triangles[nearest_intersect];
    
    vec3 intersection = RayTriangleIntersection(pos, dir, tri.p0.pos, tri.p1.pos, tri.p2.pos);
    
    vec3 barycentric = GetBarycentric(tri.p0.pos, tri.p1.pos, tri.p2.pos, intersection);
    
    vec2 tex_coords = barycentric.x * tri.p0.tex + barycentric.y * tri.p1.tex + barycentric.z * tri.p2.tex;
    vec3 normal = barycentric.x * tri.p0.nrm + barycentric.y * tri.p1.nrm + barycentric.z * tri.p2.nrm;
    
    //vec4 texture_color = SampleTexture(tri.material->normal_map ? tri.material->normal_map : tri.material->texture, tex_coords);
    vec4 texture_color = SampleTexture(tri.material->texture, tex_coords);
    
    if (tri.material->normal_map) {
        mat3 normal_matrix = mat3(tri.tangent, tri.bitangent, normal);
        
        vec3 normal_sample = SampleTexture(tri.material->normal_map, tex_coords);
        normal = glm::normalize(normal_matrix * (normal_sample * 2.0f - 1.0f)); 
    }
    
    vec3 view_dir = -dir;

    vec3 diffuse_color = {0.0f, 0.0f, 0.0f};
    vec3 specular_color = {0.0f, 0.0f, 0.0f};
    vec3 reflection_color = {0.0f, 0.0f, 0.0f};
    
    vec3 away_pos = intersection + 0.01f * normal;
    
    if (cap > 0 && tri.material->reflectivity) {
        reflection_color = FindColorFromRay(away_pos, glm::reflect(dir, normal), 0);
    }

    for (const auto& light : light_list) {
        vec3 light_vec = light.location - vec3(intersection);
        
        float dist = glm::length(light_vec);
        float strn = glm::max(glm::dot(glm::normalize(light_vec), normal), 0.0f);
        float attn = strn * (1.0f / (1.0f + 0.09f * dist + 0.032f * (dist * dist)));

        if (attn < epsilon) continue;

        float drct = glm::clamp(glm::pow(glm::max(glm::dot(light.direction, -glm::normalize(light_vec)), 0.0f), light.exponent), 0.0f, 1.0f);

        if (drct < epsilon) continue;

        if (FindIfObstacle(away_pos, glm::normalize(light_vec), light.location)) {
            continue;
        }

        float spec = glm::pow(glm::max(glm::dot(view_dir, glm::reflect(-glm::normalize(light_vec), normal)), 0.0f), tri.material->specular_exponent);

        diffuse_color += light.color * attn * drct;
        specular_color += tri.material->specular_weight * spec * light.color * drct;
    }
    
    
    
    diffuse_color += (1.0f - tri.material->specular_transparency) * specular_color;

    return vec3(texture_color) * diffuse_color + tri.material->specular_transparency * specular_color + tri.material->reflectivity * reflection_color;// + tri.material->specular_transparency * specular_color;
}

void RenderFrame() {
    if (!is_rendering) goto interactive;
    {
        
    const int y = rendering_progress;
    
    if (rendering_progress++ < screen_height)
    for (int x = 0; x < screen_width; x++) {
        vec3 far_point = InverseProject({x, y, 0.0f});
        vec3 near_point = InverseProject({x, y, 1000.0f});
        
        vec3 look_direction = glm::normalize(far_point - near_point);
        vec3 look_position = near_point;

        vec3 pixel_color = FindColorFromRay(look_position, look_direction, 1);
        //vec3 pixel_color = FindColorFromRay(look_position, look_direction, 0);
        
        BlitDot(x, y, IntColor(glm::clamp(pixel_color, 0.0f, 1.0f)));
    }
    
    }
    
    return;
interactive:
    memset(depth_buffer, 0, screen_width * screen_height * sizeof(float));

    if (clear_screen) {
        uint16_t clear_color = IntColor(screen_clear_color);
        for (int i = 0; i < screen_width * screen_height; i++) {
            screen_buffer[i] = clear_color;
        }
    }
    
    std::vector<std::pair<uint32_t, RTDrawListEntry*>> draw_list_sorted;
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
            case RT_STATIC_LIGHTMAPPED:
                
                if (!entry->index_array) continue;

                for (size_t i = 0; i < entry->index_length; i++) {
                    uint32_t triangle = (entry->index_offset + i) * 3;
                    
                    StaticVertex& p0 = ((StaticVertex*)entry->vertex_array->vertices)[entry->index_array->indices[triangle + 0]];
                    StaticVertex& p1 = ((StaticVertex*)entry->vertex_array->vertices)[entry->index_array->indices[triangle + 1]];
                    StaticVertex& p2 = ((StaticVertex*)entry->vertex_array->vertices)[entry->index_array->indices[triangle + 2]];
                    
                    vec4 pr0 = layers[0].projection_matrix * layers[0].view_matrix * entry->matrix * vec4(p0.pos, 1.0f);
                    vec4 pr1 = layers[0].projection_matrix * layers[0].view_matrix * entry->matrix * vec4(p1.pos, 1.0f);
                    vec4 pr2 = layers[0].projection_matrix * layers[0].view_matrix * entry->matrix * vec4(p2.pos, 1.0f);
                    
                    ClipRenderLine(pr0, pr1, IntColor({1, 1, 1}));
                    ClipRenderLine(pr1, pr2, IntColor({1, 1, 1}));
                    ClipRenderLine(pr2, pr0, IntColor({1, 1, 1}));
                }
                
                break;
            case RT_DYNAMIC_BLENDED:
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
                    
                    ClipRenderLine(pos0, pos1, IntColor({1, 1, 1}));
                    ClipRenderLine(pos1, pos2, IntColor({1, 1, 1}));
                    ClipRenderLine(pos2, pos0, IntColor({1, 1, 1}));
                }
                
                break;
                
            case RT_STATIC_COLORED:
                
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

bool IsInteractiveMode() {
    return !is_rendering;
}

void SetInteractiveMode(bool is_interactive) {
    if (is_interactive) {
        std::cout << "Cancelling ray-trace!!" << std::endl;
        rendering_progress = 0;
        is_rendering = false;
        return;
    }
    
    rendering_progress = 0;
    is_rendering = true;
    
    std::cout << "Begiinning ray-trace!" << std::endl;
    
    tree_triangles.clear();
    
    std::vector<std::pair<uint32_t, RTDrawListEntry*>> draw_list_sorted;
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
            case RT_STATIC_LIGHTMAPPED:
                
                if (!entry->index_array) continue;

                for (size_t i = 0; i < entry->index_length; i++) {
                    uint32_t triangle = (entry->index_offset + i) * 3;
                    
                    StaticVertex& p0 = ((StaticVertex*)entry->vertex_array->vertices)[entry->index_array->indices[triangle + 0]];
                    StaticVertex& p1 = ((StaticVertex*)entry->vertex_array->vertices)[entry->index_array->indices[triangle + 1]];
                    StaticVertex& p2 = ((StaticVertex*)entry->vertex_array->vertices)[entry->index_array->indices[triangle + 2]];
                    
                    
                    TreeTriangle tree_triangle;
                    
                    tree_triangle.p0.pos = entry->matrix * vec4(p0.pos, 1.0f);
                    tree_triangle.p1.pos = entry->matrix * vec4(p1.pos, 1.0f);
                    tree_triangle.p2.pos = entry->matrix * vec4(p2.pos, 1.0f);
                    
                    tree_triangle.p0.nrm = glm::normalize(entry->matrix * vec4(p0.nrm, 0.0f));
                    tree_triangle.p1.nrm = glm::normalize(entry->matrix * vec4(p1.nrm, 0.0f));
                    tree_triangle.p2.nrm = glm::normalize(entry->matrix * vec4(p2.nrm, 0.0f));
                    
                    tree_triangle.p0.tex = p0.tex;
                    tree_triangle.p1.tex = p1.tex;
                    tree_triangle.p2.tex = p2.tex;
                    
                    vec3 v01 = tree_triangle.p1.pos - tree_triangle.p0.pos;
                    vec3 v02 = tree_triangle.p2.pos - tree_triangle.p0.pos;
                    vec2 delta01 = p1.tex - p0.tex;
                    vec2 delta02 = p2.tex - p0.tex; 
                    
                    float f = 1.0f / (delta01.x * delta02.y - delta02.x * delta01.y);

                    tree_triangle.tangent.x = f * (delta02.y * v01.x - delta01.y * v02.x);
                    tree_triangle.tangent.y = f * (delta02.y * v01.y - delta01.y * v02.y);
                    tree_triangle.tangent.z = f * (delta02.y * v01.z - delta01.y * v02.z);

                    tree_triangle.bitangent.x = f * (-delta02.x * v01.x + delta01.x * v02.x);
                    tree_triangle.bitangent.y = f * (-delta02.x * v01.y + delta01.x * v02.y);
                    tree_triangle.bitangent.z = f * (-delta02.x * v01.z + delta01.x * v02.z);
                    
                    tree_triangle.tangent = glm::normalize(tree_triangle.tangent);
                    tree_triangle.bitangent = glm::normalize(tree_triangle.bitangent);
                    
                    tree_triangle.material = entry->material;
                    
                    tree_triangles.push_back(tree_triangle);
                }
                
                break;
            case RT_DYNAMIC_BLENDED:
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

                    TreeTriangle tree_triangle;
                    
                    tree_triangle.p0.pos = world_pos0 / world_pos0.w;
                    tree_triangle.p1.pos = world_pos1 / world_pos1.w;
                    tree_triangle.p2.pos = world_pos2 / world_pos2.w;
                    
                    tree_triangle.p0.nrm = glm::normalize(vec3(entry->matrix * local_nrm0));
                    tree_triangle.p1.nrm = glm::normalize(vec3(entry->matrix * local_nrm1));
                    tree_triangle.p2.nrm = glm::normalize(vec3(entry->matrix * local_nrm2));
                    
                    tree_triangle.p0.tex = p0.tex;
                    tree_triangle.p1.tex = p1.tex;
                    tree_triangle.p2.tex = p2.tex;
                    
                    vec3 v01 = tree_triangle.p1.pos - tree_triangle.p0.pos;
                    vec3 v02 = tree_triangle.p2.pos - tree_triangle.p0.pos;
                    vec2 delta01 = p1.tex - p0.tex;
                    vec2 delta02 = p2.tex - p0.tex; 
                    
                    float f = 1.0f / (delta01.x * delta02.y - delta02.x * delta01.y);

                    tree_triangle.tangent.x = f * (delta02.y * v01.x - delta01.y * v02.x);
                    tree_triangle.tangent.y = f * (delta02.y * v01.y - delta01.y * v02.y);
                    tree_triangle.tangent.z = f * (delta02.y * v01.z - delta01.y * v02.z);

                    tree_triangle.bitangent.x = f * (-delta02.x * v01.x + delta01.x * v02.x);
                    tree_triangle.bitangent.y = f * (-delta02.x * v01.y + delta01.x * v02.y);
                    tree_triangle.bitangent.z = f * (-delta02.x * v01.z + delta01.x * v02.z);
                    
                    tree_triangle.tangent = glm::normalize(tree_triangle.tangent);
                    tree_triangle.bitangent = glm::normalize(tree_triangle.bitangent);
                    
                    tree_triangle.material = entry->material;
                    
                    tree_triangles.push_back(tree_triangle);
                }
                
                break;
                
            case RT_STATIC_COLORED:
                
                /*if (!(entry->flags & FLAG_DRAW_LINES)) continue;
                
                for (size_t i = 0; i < entry->index_length / 2; i++) {
                    ColorVertex& p0 = ((ColorVertex*)entry->vertex_array->vertices)[i * 2 + 0];
                    ColorVertex& p1 = ((ColorVertex*)entry->vertex_array->vertices)[i * 2 + 1];
                    
                    vec4 pr0 = layers[0].projection_matrix * layers[0].view_matrix * entry->matrix * vec4(p0.pos, 1.0f);
                    vec4 pr1 = layers[0].projection_matrix * layers[0].view_matrix * entry->matrix * vec4(p1.pos, 1.0f);
                    
                    ClipRenderLine(pr0, pr1, IntColor(p0.col));
                }*/
                
                break;
            default:
                break;
        }
    }

}

drawlistentry_t InsertDrawListEntry() {
    return drawlistentry_t {.rt = draw_list.AddNew()};
}

void RemoveDrawListEntry(drawlistentry_t entry) {
    draw_list.Remove(entry.rt);
}

uint32_t GetFlags(drawlistentry_t entry) {
    return entry.rt->flags;
}

void SetFlags(drawlistentry_t entry, uint32_t flags) {
    entry.rt->flags = flags;
}

void SetLayer(drawlistentry_t entry, uint32_t layer) {
    entry.rt->layer = layer;
}

void SetPose(drawlistentry_t entry, Pose* pose) {
    entry.rt->pose = pose;
}

void SetLightmap(drawlistentry_t entry, texturehandle_t lightmap) {
    entry.rt->lightmap = lightmap.rt_texture;
}

void SetEnvironmentMap(drawlistentry_t entry, texturehandle_t environmentmap) {
    entry.rt->environmentmap = environmentmap.rt_texture;
}

void SetSphericalHarmonic(drawlistentry_t entry, sphericalharmonic_t harmonic) {
    entry.rt->harmonic = harmonic;
}

void SetDrawListAABB(drawlistentry_t entry, vec3 min, vec3 max) {
    //entry.sw->aabb_min = min;
    //entry.sw->aabb_max = max;
}

void SetDrawListColors(drawlistentry_t entry, size_t count, vec4* color) {
    entry.rt->color = *color;
}

void SetDrawListTextureOffsets(drawlistentry_t entry, size_t count, vec4* offset) {
    /*for (size_t i = 0; i < count; i++) {
        entry.sw->texture_transforms[i] = offset[i];
    }*/
}


void SetDrawListSpecularities(drawlistentry_t entry, size_t count, float* weight, float* exponent, float* transparency) {
    entry.rt->specular_weight = *weight;
    entry.rt->specular_exponent = *exponent;
    entry.rt->specular_transparency = *transparency;
}

void SetLights(drawlistentry_t entry, light_t* lights) {
    for (size_t i = 0; i < 4; i++) {
        entry.rt->lights[i] = lights[i].rt;
    }
}

void SetMatrix(drawlistentry_t entry, const mat4& matrix) {
    entry.rt->matrix = matrix;
}

void SetFadeDistance(drawlistentry_t entry, float near, float far) {
    //entry.sw->fade_near = near;
    //entry.sw->fade_far = far;
}

void SetDrawListVertexArray(drawlistentry_t entry, vertexarray_t vertex_array_handle) {
    entry.rt->vertex_array = vertex_array_handle.rt_vertex_array;
}

void SetDrawListSpriteArray(drawlistentry_t entry, spritearray_t sprite_array_handle) {
    
}

void SetDrawListIndexArray(drawlistentry_t entry, indexarray_t index_array_handle) {
    entry.rt->index_array = index_array_handle.rt_index_array;
}

void SetDrawListIndexRange(drawlistentry_t entry, uint32_t index_offset, uint32_t index_length) {
    entry.rt->index_offset = index_offset;
    entry.rt->index_length = index_length;
}

bool SetDrawListShader(drawlistentry_t entry, vertexformat_t vertex_format, materialtype_t material_type) {
    // TODO: check if material_type has transparency and save that fact
    return true;
}

void SetDrawListMaterials(drawlistentry_t entry, size_t texture_count, material_t* material) {
    entry.rt->material = material->rt;
}

material_t MakeMaterial() {
    return material_t{.rt = material_list.AddNew()};
}

void DeleteMaterial(material_t material) {
    material_list.Remove(material.rt);
}

void SetMaterialTexture(material_t material, texturehandle_t texture) {
    material.rt->texture = texture.rt_texture;
}

void SetMaterialNormalMap(material_t material, texturehandle_t texture) {
    material.rt->normal_map = texture.rt_texture;
}

void SetMaterialColor(material_t material, vec4 color) {
    material.rt->color = color;
}

void SetMaterialSpecularWeight(material_t material, float weight) {
    material.rt->specular_weight = weight;
}

void SetMaterialSpecularExponent(material_t material, float exponent) {
    material.rt->specular_exponent = exponent;
}

void SetMaterialSpecularTransparency(material_t material, float transparency) {
    material.rt->specular_transparency = transparency;
}

void SetMaterialReflectivity(material_t material, float reflectivity) {
    material.rt->reflectivity = reflectivity;
}

light_t MakeLight() {
    return light_t {.rt = light_list.AddNew()};
}

void DeleteLight(light_t light) {
    light_list.Remove(light.rt);
}

void SetLightParameters(light_t light, vec3 location, vec3 color, float distance, vec3 direction, float exponent) {
    RTLight* light_ptr = light.rt;
    
    light_ptr->location = location;
    light_ptr->color = color;
    light_ptr->distance = distance;
    light_ptr->direction = direction;
    light_ptr->exponent = exponent;
}


texturehandle_t CreateTexture(ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data) {
    RTTexture* texture = texture_list.AddNew();
    
    texture->width = width;
    texture->height = height;
    
    switch (color_mode) {
        case COLORMODE_R:       texture->channels = 1;  break;
        case COLORMODE_RG:      texture->channels = 2;  break;
        case COLORMODE_RGB:     texture->channels = 3;  break;
        case COLORMODE_RGBA:    texture->channels = 4;  break;
    }
    
    switch (texture_filter) {
        case TEXTUREFILTER_NEAREST:             texture->mode = RT_NEAREST; break;
        case TEXTUREFILTER_LINEAR:              texture->mode = RT_BLENDED; break;
        case TEXTUREFILTER_LINEAR_MIPMAPPED:    texture->mode = RT_BLENDED; break;
    }
    
    int texture_size = texture->width * texture->height * sizeof(vec4);
    texture->pixels = (vec4*)malloc(texture_size);
    uint8_t* pix = (uint8_t*)data;
    //memcpy(texture->pixels, data, texture_size);
    for (int i = 0; i < texture->width * texture->height; i++) {
        vec4 color = {0.0f, 0.0f, 0.0f, 255.0f};
        
        int offset = i * texture->channels;
        
        switch (color_mode) {
            case COLORMODE_R:       color.x = pix[offset + 0];      break;
            case COLORMODE_RG:      color.x = pix[offset + 0]; color.y = pix[offset + 1];    break;
            case COLORMODE_RGB:     color.x = pix[offset + 0]; color.y = pix[offset + 1]; color.z = pix[offset + 2];  break;
            case COLORMODE_RGBA:    color.x = pix[offset + 0]; color.y = pix[offset + 1]; color.z = pix[offset + 2]; color.w = pix[offset + 3];  break;
        }
        
        
        texture->pixels[i] = color / 255.0f;
    }
    
    return texturehandle_t {.rt_texture = texture};
}

void ParseFormat(vertexarray_t& vertex_array, VertexDefinition vertex_format) {
    auto& helper = vertex_array.rt_vertex_array->helper;
 
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
         vertex_array.rt_vertex_array->type = RT_STATIC_COLORED;
    }
 
    if (helper.lightmap != -1) {
        vertex_array.rt_vertex_array->type = RT_STATIC_LIGHTMAPPED;
    }
    
    if (helper.bone_index != -1 && helper.bone_weight != -1) {
        vertex_array.rt_vertex_array->type = RT_DYNAMIC_BLENDED;
    }
    
    vertex_array.rt_vertex_array->format = vertex_format;
}

void PackVertices(vertexarray_t& vertex_array, void* data, size_t count) {
    switch (vertex_array.rt_vertex_array->type) {
        case RT_STATIC_LIGHTMAPPED: {
            StaticVertex* verts = (StaticVertex*)malloc(sizeof(StaticVertex) * count);
            
            VertexAttribute pos_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.position];
            for (size_t i = 0; i < count; i++) {
                vec3* pos = (vec3*)((char*)data + pos_attrib.offset + pos_attrib.stride * i);
                verts[i].pos = *pos;
            }
            
            VertexAttribute nrm_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.normal];
            for (size_t i = 0; i < count; i++) {
                vec3* nrm = (vec3*)((char*)data + nrm_attrib.offset + nrm_attrib.stride * i);
                verts[i].nrm = *nrm;
            }
            
            VertexAttribute tex_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.texture];
            for (size_t i = 0; i < count; i++) {
                vec2* tex = (vec2*)((char*)data + tex_attrib.offset + tex_attrib.stride * i);
                verts[i].tex = *tex;
            }
            
            VertexAttribute lit_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.lightmap];
            for (size_t i = 0; i < count; i++) {
                vec2* lit = (vec2*)((char*)data + lit_attrib.offset + lit_attrib.stride * i);
                verts[i].lit = *lit;
            }
            
            vertex_array.rt_vertex_array->vertices = verts;
            vertex_array.rt_vertex_array->vertex_count = count;
        } break;
        case RT_DYNAMIC_BLENDED: {
            DynamicVertex* verts = (DynamicVertex*)malloc(sizeof(DynamicVertex) * count);
            
            VertexAttribute pos_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.position];
            for (size_t i = 0; i < count; i++) {
                vec3* pos = (vec3*)((char*)data + pos_attrib.offset + pos_attrib.stride * i);
                verts[i].pos = *pos;
            }
            
            VertexAttribute nrm_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.normal];
            for (size_t i = 0; i < count; i++) {
                vec3* nrm = (vec3*)((char*)data + nrm_attrib.offset + nrm_attrib.stride * i);
                verts[i].nrm = *nrm;
            }
            
            VertexAttribute tex_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.texture];
            for (size_t i = 0; i < count; i++) {
                vec2* tex = (vec2*)((char*)data + tex_attrib.offset + tex_attrib.stride * i);
                verts[i].tex = *tex;
            }
            
            VertexAttribute wgt_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.bone_weight];
            for (size_t i = 0; i < count; i++) {
                vec4* wgt = (vec4*)((char*)data + wgt_attrib.offset + wgt_attrib.stride * i);
                verts[i].wgt = glm::normalize(*wgt);
            }
            
            VertexAttribute ind_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.bone_index];
            for (size_t i = 0; i < count; i++) {
                ivec4* ind = (ivec4*)((char*)data + ind_attrib.offset + ind_attrib.stride * i);
                verts[i].ind = *ind;
            }
        
            vertex_array.rt_vertex_array->vertices = verts;
            vertex_array.rt_vertex_array->vertex_count = count;
        } break;
        case RT_STATIC_COLORED: {
            ColorVertex* verts = (ColorVertex*)malloc(sizeof(ColorVertex) * count);
            
            VertexAttribute pos_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.position];
            for (size_t i = 0; i < count; i++) {
                vec3* pos = (vec3*)((char*)data + pos_attrib.offset + pos_attrib.stride * i);
                verts[i].pos = *pos;
            }
            
            VertexAttribute col_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.color];
            for (size_t i = 0; i < count; i++) {
                vec3* col = (vec3*)((char*)data + col_attrib.offset + col_attrib.stride * i);
                verts[i].col = *col;
            }
            
            vertex_array.rt_vertex_array->vertices = verts;
            vertex_array.rt_vertex_array->vertex_count = count;
        } break;
        case RT_SPRITE:
            assert(false);
    }
}

void CreateIndexedVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array, indexarray_t& index_array, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data) {
    vertex_array.rt_vertex_array = vertex_arrays.AddNew();
    ParseFormat(vertex_array, vertex_format);
    PackVertices(vertex_array, vertex_data, vertex_size / vertex_format.attributes[0].stride);
    
    index_array.rt_index_array = index_arrays.AddNew();
    index_array.rt_index_array->indices = (uint32_t*)malloc(index_size);
    index_array.rt_index_array->index_count = index_size / sizeof(uint32_t);
    memcpy(index_array.rt_index_array->indices, index_data, index_size);
}

void CreateVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array) {
    vertex_array.rt_vertex_array = vertex_arrays.AddNew();
    ParseFormat(vertex_array, vertex_format);
}

void UpdateVertexArray(vertexarray_t& vertex_array, size_t data_size, void* data) {
    if (vertex_array.rt_vertex_array->vertices) {
        free(vertex_array.rt_vertex_array->vertices);
        vertex_array.rt_vertex_array->vertices = nullptr;
    }
    
    PackVertices(vertex_array, data, data_size / vertex_array.rt_vertex_array->format.attributes[0].stride);
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
