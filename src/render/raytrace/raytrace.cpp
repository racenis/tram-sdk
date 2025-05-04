// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/raytrace/raytrace.h>
#include <render/api.h>

#include <render/raytrace/drawlist.h>
#include <render/raytrace/material.h>
#include <render/raytrace/light.h>
#include <render/raytrace/buffer.h>
#include <render/raytrace/math.h>

#include <templates/octree.h>
#include <templates/aabb.h>

#include <config.h>

#include <cstdlib>
#include <cstring>

namespace tram::Render::API {
    
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

static Render::Pose* null_pose = nullptr;

static RTTexture* texture_indices[16] = {nullptr};

const float epsilon = 1.0f/255.0f;

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

static std::vector<TreeTriangle> tree_triangles;
static AABBTree* tree = nullptr;

static int rendering_progress = 0;
static bool is_rendering = false;
static bool use_assembly_rendering = false;

static AssemblyLayers assembly;

std::vector<AssemblyLayer>& GetAssembly(int x, int y) {
    return assembly.pixels[y * screen_width + x];
}

void SetLightingParameters (vec3 sun_direction, vec3 sun_color, vec3 ambient_color, uint32_t layer) {
    layers[layer].sun_direction = sun_direction;
    layers[layer].sun_color = sun_color;
    layers[layer].ambient_color = ambient_color;
}

void SetScreenSize(float width, float height) {
    screen_width = width;
    screen_height = height;
}

void SetScreenClear (vec3 clear_color, bool clear) {
    clear_screen = clear;
    screen_clear_color = clear_color;
}

void BlitDot(uint32_t x, uint32_t y, uint16_t color) {
    if (x < 0 || x >= screen_width) return;
    if (y < 0 || y >= screen_height) return;
    screen_buffer[screen_width * y + x] = color;
}

std::pair<int32_t, int32_t> ClipSpaceToScreenSpace(const vec4 p) {
    int32_t px = (p.x + 1.0f) * 0.5f * screen_width;
    int32_t py = (1.0f - p.y) * 0.5f * screen_height;
    return {px, py};
}

void ClipRenderLine(vec4 p0, vec4 p1, auto blit_func) {
    if (!LineVisible(p0, p1)) return;
    
    ClipLineInClipSpace(p0, p1);
    PerspectiveDivision(p0, p1);
    
    auto [px0, py0] = ClipSpaceToScreenSpace(p0);
    auto [px1, py1] = ClipSpaceToScreenSpace(p1);
    
    BlitLine(px0, py0, px1, py1, blit_func);
}

vec3 InverseProject(vec3 point) {
    point.y =  screen_height - point.y;
    vec3 result = glm::unProject(point,
                                 layers[0].view_matrix,
                                 layers[0].projection_matrix,
                                 vec4(0.0f, 0.0f, screen_width, screen_height));
    return result;    
}

int FindNearestTriangle(vec3 pos, vec3 dir) {
    uint32_t nearest = tree->FindIntersection(pos, dir, 32.0f, [&](vec3 pos, vec3 dir, uint32_t index) {
        vec3 intersection = RayTriangleIntersection(pos,
                                                    dir,
                                                    tree_triangles[index].p0.pos,
                                                    tree_triangles[index].p1.pos,
                                                    tree_triangles[index].p2.pos);

        if (intersection.x == INFINITY) return INFINITY;

        return glm::distance(intersection, pos);
    });

    if (nearest == (uint32_t)-1) {
        return -1;
    }
    
    return nearest;
}

bool FindIfObstacle(vec3 pos, vec3 dir, vec3 target) {
    float target_distance = glm::distance(pos, target);
    
    uint32_t nearest = tree->FindIntersection(pos, dir, target_distance, [&](vec3 pos, vec3 dir, uint32_t index) {
        vec3 intersection = RayTriangleIntersection(pos,
                                                    dir,
                                                    tree_triangles[index].p0.pos,
                                                    tree_triangles[index].p1.pos,
                                                    tree_triangles[index].p2.pos);

        if (intersection.x == INFINITY) return INFINITY;

        return glm::distance(intersection, pos) < target_distance ? 0.0f : INFINITY;
    });

    if (nearest == (uint32_t)-1) {
        return false;
    }
    
    return true;
}

std::pair<vec3, vec3> GetLightContribution(RTMaterial* material, vec3 view_dir, vec3 point, vec3 normal) {
    vec3 diffuse_color = {0.0f, 0.0f, 0.0f};
    vec3 specular_color = {0.0f, 0.0f, 0.0f};
    
    const vec3 away_pos = point + 0.01f * normal;
    
    for (const auto& light : PoolProxy<RTLight>::GetPool()) {
        vec3 light_vec = light.location - point;
        
        float dist = glm::length(light_vec);
        float strn = glm::max(glm::dot(glm::normalize(light_vec), normal), 0.0f);
        float attn = strn * (1.0f / (1.0f + 0.09f * dist + 0.032f * (dist * dist)));

        if (attn < epsilon) continue;

        float drct = glm::clamp(glm::pow(glm::max(glm::dot(light.direction, -glm::normalize(light_vec)), 0.0f), light.exponent), 0.0f, 1.0f);

        if (drct < epsilon) continue;

        if (FindIfObstacle(away_pos, glm::normalize(light_vec), light.location)) {
            continue;
        }

        float spec = glm::pow(glm::max(glm::dot(view_dir, glm::reflect(-glm::normalize(light_vec), normal)), 0.0f), material->specular_exponent);

        diffuse_color += light.color * attn * drct;
        specular_color += material->specular_weight * spec * light.color * drct;
    }
    
    diffuse_color += (1.0f - material->specular_transparency) * specular_color;
    specular_color *= material->specular_transparency;
    
    return {diffuse_color, specular_color};
}

vec3 FindColorFromRay(vec3 pos, vec3 dir, int cap) {
    int nearest_intersect = FindNearestTriangle(pos, dir);

    if (nearest_intersect == -1) {
        if (clear_screen) {
            return screen_clear_color;
        } else {
            return {0, 0, 0};
        }
    }

    const auto& tri = tree_triangles[nearest_intersect];
    
    vec3 intersection = RayTriangleIntersection(pos, dir, tri.p0.pos, tri.p1.pos, tri.p2.pos);
    
    vec3 barycentric = GetBarycentric(tri.p0.pos, tri.p1.pos, tri.p2.pos, intersection);
    
    vec2 tex_coords = barycentric.x * tri.p0.tex + barycentric.y * tri.p1.tex + barycentric.z * tri.p2.tex;
    vec3 normal = barycentric.x * tri.p0.nrm + barycentric.y * tri.p1.nrm + barycentric.z * tri.p2.nrm;
    
    vec4 texture_color = SampleTexture(tri.material->texture, tex_coords);
    
    if (tri.material->normal_map) {
        mat3 normal_matrix = mat3(tri.tangent, tri.bitangent, normal);
        
        vec3 normal_sample = SampleTexture(tri.material->normal_map, tex_coords);
        normal = glm::normalize(normal_matrix * (normal_sample * 2.0f - 1.0f)); 
    }
    
    vec3 view_dir = -dir;
    vec3 away_pos = intersection + 0.01f * normal;

    vec3 reflection_color = {0.0f, 0.0f, 0.0f};
    
    if (cap > 0 && tri.material->reflectivity) {
        reflection_color = FindColorFromRay(away_pos, glm::reflect(dir, normal), 0);
    }

    auto [diffuse_color, specular_color] = GetLightContribution(tri.material, view_dir, intersection, normal);

    return vec3(texture_color) * diffuse_color + specular_color + tri.material->reflectivity * reflection_color;
}

std::vector<AssemblyLayer> FindAssemblyFromRay(vec3 pos, vec3 dir, int cap) {
    int nearest_intersect = FindNearestTriangle(pos, dir);

    if (nearest_intersect == -1) {
        return {{vec4(screen_clear_color, 1.0f),
                 -1,
                 0,
                 0,
                 1.0f,
                 {0.0f, 0.0f, 0.0f, 0.0f}}};        
    }

    const auto& tri = tree_triangles[nearest_intersect];
    
    vec3 intersection = RayTriangleIntersection(pos, dir, tri.p0.pos, tri.p1.pos, tri.p2.pos);
    
    vec3 barycentric = GetBarycentric(tri.p0.pos, tri.p1.pos, tri.p2.pos, intersection);
    
    vec2 tex_coords = barycentric.x * tri.p0.tex + barycentric.y * tri.p1.tex + barycentric.z * tri.p2.tex;
    vec3 normal = barycentric.x * tri.p0.nrm + barycentric.y * tri.p1.nrm + barycentric.z * tri.p2.nrm;
    
    vec4 texture_color = SampleTexture(tri.material->texture, tex_coords);
    
    if (tri.material->normal_map) {
        mat3 normal_matrix = mat3(tri.tangent, tri.bitangent, normal);
        
        vec3 normal_sample = SampleTexture(tri.material->normal_map, tex_coords);
        normal = glm::normalize(normal_matrix * (normal_sample * 2.0f - 1.0f)); 
    }
    
    vec3 view_dir = -dir;
    vec3 away_pos = intersection + 0.01f * normal;

    std::vector<AssemblyLayer> layers;

    if (cap > 0 && tri.material->reflectivity) {
        layers = FindAssemblyFromRay(away_pos, glm::reflect(dir, normal), 0);
    }

    auto [diffuse_color, specular_color] = GetLightContribution(tri.material, view_dir, intersection, normal);

    AssemblyLayer layer;
    
    if (tri.material->texture->assembly_index > 0) {
        // sample
        layer.diffuse = vec4(diffuse_color, 1.0f);
        layer.texture = tri.material->texture->assembly_index;
        layer.sample_x = tex_coords.x * (float)tri.material->texture->width;
        layer.sample_y = tex_coords.y * (float)tri.material->texture->height;
        layer.sublayer_opacity = tri.material->reflectivity;
        layer.specular = vec4(specular_color, 1.0f); // nuu
    } else {
        // merge
        layer.diffuse = texture_color * vec4(diffuse_color, 1.0f);
        layer.texture = -1;
        layer.sample_x = 0;
        layer.sample_y = 0;
        layer.sublayer_opacity = tri.material->reflectivity;
        layer.specular = vec4(specular_color, 1.0f); // nuu
    }
    
    layers.push_back(layer);
    
    return layers;
    
    //return vec3(texture_color) * diffuse_color + tri.material->specular_transparency * specular_color + tri.material->reflectivity * reflection_color;
    //return vec3(texture_color) * diffuse_color + tri.material->specular_transparency * specular_color + tri.material->reflectivity * reflection_color;
}

vec3 GetColorFromAssembly(std::vector<AssemblyLayer>& layers) {
    vec3 color = {0, 0, 0};
    
    for (auto& layer : layers) {
        color *= layer.sublayer_opacity;
        
        if (layer.texture > 0) {
            float u = (float)layer.sample_x / (float)texture_indices[layer.texture]->width;
            float v = (float)layer.sample_y / (float)texture_indices[layer.texture]->height;
            color += vec3(SampleTexture(texture_indices[layer.texture], {u, v})) * vec3(layer.diffuse);
        } else {
            color += vec3(layer.diffuse);
        }
        
        color += vec3(layer.specular);
    }
    
    return color;
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

        if (!use_assembly_rendering) {
            vec3 pixel_color = FindColorFromRay(look_position, look_direction, 1);
            //vec3 pixel_color = FindColorFromRay(look_position, look_direction, 0);
            
            BlitDot(x, y, IntColor(glm::clamp(pixel_color, 0.0f, 1.0f)));
        } else {
            auto layers = FindAssemblyFromRay(look_position, look_direction, 1);
            
            BlitDot(x, y, IntColor(glm::clamp(GetColorFromAssembly(layers), 0.0f, 1.0f)));
            
            GetAssembly(x, y) = layers;
        }
        
    }
    
    }
    
    return;
interactive:
    /*if (clear_screen) {
        uint16_t clear_color = IntColor(screen_clear_color);
        for (int i = 0; i < screen_width * screen_height; i++) {
            screen_buffer[i] = clear_color;
        }
    }*/
    
    for (int i = 0; i < screen_width * screen_height; i++) {
        screen_buffer[i] = 0x0000;
    }
    
    std::vector<std::pair<uint32_t, RTDrawListEntry*>> draw_list_sorted;
    for (auto& entry : PoolProxy<RTDrawListEntry>::GetPool()) {
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
                    
                    ClipRenderLine(pr0, pr1, [=](uint32_t x, uint32_t y) {BlitDot(x, y, IntColor({1, 1, 1}));});
                    ClipRenderLine(pr1, pr2, [=](uint32_t x, uint32_t y) {BlitDot(x, y, IntColor({1, 1, 1}));});
                    ClipRenderLine(pr2, pr0, [=](uint32_t x, uint32_t y) {BlitDot(x, y, IntColor({1, 1, 1}));});
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
                    
                    ClipRenderLine(pos0, pos1, [=](uint32_t x, uint32_t y) {BlitDot(x, y, IntColor({1, 1, 1}));});
                    ClipRenderLine(pos1, pos2, [=](uint32_t x, uint32_t y) {BlitDot(x, y, IntColor({1, 1, 1}));});
                    ClipRenderLine(pos2, pos0, [=](uint32_t x, uint32_t y) {BlitDot(x, y, IntColor({1, 1, 1}));});
                }
                
                break;
                
            case RT_STATIC_COLORED:
                
                if (!(entry->flags & FLAG_DRAW_LINES)) continue;
                
                for (size_t i = 0; i < entry->index_length / 2; i++) {
                    ColorVertex& p0 = ((ColorVertex*)entry->vertex_array->vertices)[i * 2 + 0];
                    ColorVertex& p1 = ((ColorVertex*)entry->vertex_array->vertices)[i * 2 + 1];
                    
                    vec4 pr0 = layers[0].projection_matrix * layers[0].view_matrix * entry->matrix * vec4(p0.pos, 1.0f);
                    vec4 pr1 = layers[0].projection_matrix * layers[0].view_matrix * entry->matrix * vec4(p1.pos, 1.0f);
                    
                    ClipRenderLine(pr0, pr1, [=](uint32_t x, uint32_t y) {BlitDot(x, y, IntColor(p0.col));});
                }
                
                break;
            default:
                break;
        }
    }

}

bool IsFinishedRendering() {
    return is_rendering && rendering_progress > screen_height;
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
    
    // horrible spathetti code to render a wireframe befor estarting to raytrace
    //is_rendering = false;
    //RenderFrame();
    //is_rendering = true;
    
    /*for (int i = 0; i < screen_width * screen_height; i++) {
        screen_buffer[i] = 0x0000;
    }*/
    
    if (use_assembly_rendering) {
        assembly.width = screen_width;
        assembly.height = screen_height;
        
        assembly.pixels.clear();
        assembly.pixels.resize(screen_width * screen_height);
    }
    
    
    tree_triangles.clear();
    if (tree) delete tree;
    tree = new AABBTree;
    
    std::vector<std::pair<uint32_t, RTDrawListEntry*>> draw_list_sorted;
    for (auto& entry : PoolProxy<RTDrawListEntry>::GetPool()) {
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
                    
                    tree->InsertLeaf(tree_triangles.size(),
                                     MergeAABBMin(MergeAABBMin(tree_triangle.p0.pos, tree_triangle.p1.pos), tree_triangle.p2.pos),
                                     MergeAABBMax(MergeAABBMax(tree_triangle.p0.pos, tree_triangle.p1.pos), tree_triangle.p2.pos));
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
                    
                    tree->InsertLeaf(tree_triangles.size(),
                                     MergeAABBMin(MergeAABBMin(tree_triangle.p0.pos, tree_triangle.p1.pos), tree_triangle.p2.pos),
                                     MergeAABBMax(MergeAABBMax(tree_triangle.p0.pos, tree_triangle.p1.pos), tree_triangle.p2.pos));
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

void SetMaterialAssemblyIndex(material_t material, int index) {
    material.rt->texture->assembly_index = index;
    texture_indices[index] = material.rt->texture;
    assembly.textures.push_back({index, material.rt->texture->width, material.rt->texture->height});
}

void SetUseAssembly(bool use) {
    use_assembly_rendering = use;
}

AssemblyLayers GetAssemblyLayers() {
    return assembly;
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
