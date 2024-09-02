// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <framework/system.h>
#include <framework/stats.h>
#include <render/render.h>
#include <render/renderer.h>
#include <render/vertices.h>
#include <components/render.h>
#include <components/sprite.h>
#include <components/particle.h>

#include <render/api.h>

#include <config.h>

#include <cstring>

using namespace tram;
using namespace tram::Render;

namespace tram::Render {

struct {
    vec3 view_position = {0.0f, 0.0f, 0.0f};
    quat view_rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    float view_fov = 60.0f;
    float near_plane = 0.01f;
    float far_plane = 250.0f;
    
    vec3 sun_direction = {0.0f, 1.0f, 0.0f};
    vec3 sun_color = {1.0f, 1.0f, 1.0f};
    vec3 ambient_color = {0.0f, 0.0f, 0.0f};
    
    mat4 projection = mat4(1.0f);
    mat4 view = mat4(1.0f);
} view_properties[7];

static float screen_width = 800.0f;
static float screen_height = 600.0f;

bool THIRD_PERSON = false;
bool DRAW_RENDER_DEBUG = false;

vertexarray_t colorlines_vertex_array = {.generic = 0};
drawlistentry_t colorlines_entry;

std::vector<LineVertex> colorlines;

using namespace API;

static void update_view(layer_t layer) {
    const vec3 pos = view_properties[layer].view_position;
    const quat rot = view_properties[layer].view_rotation;
    
    view_properties[layer].view = glm::inverse(glm::translate(mat4(1.0f), pos) * glm::toMat4(rot));
    
    API::SetViewMatrix(view_properties[layer].view, layer);
}

static void update_projection(layer_t layer) {
    const float fov = view_properties[layer].view_fov;
    const float ratio = screen_width / screen_height;
    
    mat4 matrix;
    if (fov == 0.0f) {
        matrix = glm::ortho(-ratio, +ratio, -(1.0f/ratio), +(1.0f/ratio), view_properties[layer].near_plane, view_properties[layer].far_plane);
    } else {
        matrix = glm::perspective(glm::radians(view_properties[layer].view_fov), ratio, view_properties[layer].near_plane, view_properties[layer].far_plane);
    }
    
    view_properties[layer].projection = matrix;
    API::SetProjectionMatrix(matrix, layer);
}

static void update_light(layer_t layer) {
    API::SetLightingParameters(view_properties[layer].sun_direction, 
                               view_properties[layer].sun_color, 
                               view_properties[layer].ambient_color, 
                               layer);
}

/// Initializes the rendering system.
/// @note Core and UI systems need to be initialized before initializing the render system.
void Init () {
    assert(System::IsInitialized(System::SYSTEM_CORE));
    assert(System::IsInitialized(System::SYSTEM_UI));
    
    API::Init();
    
    // this will initialize the projection matrices
    Render::SetScreenSize(screen_width, screen_height);
    
    
    // this is for rendering lines
    CreateVertexArray (GetVertexDefinition(VERTEX_LINE), colorlines_vertex_array);
    colorlines_entry = InsertDrawListEntry ();
    SetDrawListVertexArray(colorlines_entry, colorlines_vertex_array);
    SetDrawListShader(colorlines_entry, VERTEX_LINE, MATERIAL_FLAT_COLOR);
    SetFlags(colorlines_entry, FLAG_RENDER | FLAG_NO_DEPTH_TEST | FLAG_DRAW_LINES | FLAG_DISABLE_LIGHTING);
    SetLayer(colorlines_entry, 1);
    
    // generating fullbright lightmap
    Material* fullbright = Material::Make("fullbright", MATERIAL_LIGHTMAP);
    fullbright->MakePattern({1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f});
    fullbright->LoadFromMemory();
    
    // generating default texture
    Material* defaulttexture = Material::Make("defaulttexture", MATERIAL_TEXTURE);
    defaulttexture->MakePattern({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f});
    defaulttexture->LoadFromMemory();
    
    System::SetInitialized(System::SYSTEM_RENDER, true);
}

/// Renders a single frame.
void Render () {
    Stats::Start(System::SYSTEM_RENDER);
#ifndef ENGINE_EDITOR_MODE
    // idk if these need to be here -> after all, the armatures aren't updated in here?
    for (auto& it : PoolProxy<SpriteComponent>::GetPool()) it.Update();
    for (auto& it : PoolProxy<ParticleComponent>::GetPool()) it.Update();
#endif // ENGINE_EDITOR_MODE
    
    UpdateVertexArray(colorlines_vertex_array, colorlines.size() * sizeof(LineVertex), &colorlines[0]);
    SetDrawListVertexArray(colorlines_entry, colorlines_vertex_array);
    SetDrawListIndexRange(colorlines_entry, 0, colorlines.size());
    colorlines.clear();
    
    RenderFrame();
    Stats::Stop(System::SYSTEM_RENDER);
}

/// Sets the sun direction.
/// @param direction    Normal vector pointing towards the sun.
/// @param layer        Rendering layer to which the sun direction will be applied.
void SetSunDirection (color_t direction, layer_t layer) {
    view_properties[layer].sun_direction = direction;
    update_light(layer);
}

/// Sets the sun color.
/// @param color    Color of the sun.
/// @param layer    Rendering layer to which the sun color will be applied.
void SetSunColor (color_t color, layer_t layer) {
    view_properties[layer].sun_color = color;
    update_light(layer);
}

/// Sets the ambient color.
/// @param color    Ambient color.
/// @param layer    Rendering layer to which the ambient color will be applied.
void SetAmbientColor (color_t color, layer_t layer) {
    view_properties[layer].ambient_color = color;
    update_light(layer);
}

void SetScreenSize(float width, float height) {
    screen_width = width;
    screen_height = height;
    
    for (int i = 0; i < 7; i++) {
        update_projection(i);
    }
    
    API::SetScreenSize(width, height);
}

void SetViewFov(float fov, layer_t layer) {
    view_properties[layer].view_fov = fov;
    update_projection(layer);
}

float GetViewFov(layer_t layer) {
    return view_properties[layer].view_fov;
}

void SetViewDistance(float dist, layer_t layer) {
    view_properties[layer].far_plane = dist;
    update_projection(layer);
}
float GetViewDistance(layer_t layer) {
    return view_properties[layer].far_plane;
}



/// Sets the view position.
/// @param position View position.
/// @param layer    Rendering layer to which the view position will be applied.
void SetViewPosition (vec3 position, layer_t layer) {
    view_properties[layer].view_position = position;
    update_view(layer);
}

/// Sets the view rotation.
/// @param rotation View rotation.
/// @param layer    Rendering layer to which the view rotation will be applied.
void SetViewRotation (quat rotation, layer_t layer) {
    view_properties[layer].view_rotation = rotation;
    update_view(layer);
}

/// Returns the view position for a given layer.
vec3 GetViewPosition (layer_t layer) {
    return view_properties[layer].view_position;
}

/// Returns the view rotation for a given layer.
quat GetViewRotation (layer_t layer) {
    return view_properties[layer].view_rotation;
}

void AddLine(vec3 from, vec3 to, vec3 color) {
    colorlines.push_back({from, color});
    colorlines.push_back({to, color});
}

void AddLineMarker(vec3 location, vec3 color) {
    AddLine(location + vec3(-1.0f, 0.0f, 0.0f), location + vec3(1.0f, 0.0f, 0.0f), color);
    AddLine(location + vec3(0.0f, -1.0f, 0.0f), location + vec3(0.0f, 1.0f, 0.0f), color);
    AddLine(location + vec3(0.0f, 0.0f, -1.0f), location + vec3(0.0f, 0.0f, 1.0f), color);
}

void AddLineAABB(vec3 min, vec3 max, vec3 center, quat rotation, vec3 color) {
    vec3 point1 = center + (rotation * vec3 {min.x, min.y, min.z});
    vec3 point2 = center + (rotation * vec3 {max.x, max.y, max.z});
    vec3 point3 = center + (rotation * vec3 {min.x, max.y, max.z});
    vec3 point4 = center + (rotation * vec3 {max.x, min.y, max.z});
    vec3 point5 = center + (rotation * vec3 {max.x, max.y, min.z});
    vec3 point6 = center + (rotation * vec3 {max.x, min.y, min.z});
    vec3 point7 = center + (rotation * vec3 {min.x, max.y, min.z});
    vec3 point8 = center + (rotation * vec3 {min.x, min.y, max.z});
    
    AddLine(point1, point6, color);
    AddLine(point1, point7, color);
    AddLine(point1, point8, color);
    
    AddLine(point2, point3, color);
    AddLine(point2, point4, color);
    AddLine(point2, point5, color);
    
    AddLine(point3, point7, color);
    AddLine(point3, point8, color);
    
    AddLine(point4, point6, color);
    AddLine(point4, point8, color);
    
    AddLine(point5, point6, color);
    AddLine(point5, point7, color);
}

static const int SPHERE_HEIGHT = 8;
static const int SPHERE_WIDTH = 8;
static constexpr float pi = 3.14159f;

static constexpr vec3 sphere_p(int h, int w) {
    float x = sin(pi * h/SPHERE_HEIGHT) * cos(2*pi * w/SPHERE_WIDTH);
    float y = sin(pi * h/SPHERE_HEIGHT) * sin(2*pi * w/SPHERE_WIDTH);
    float z = cos(pi * h/SPHERE_HEIGHT);
    
    return {x, z, y};
}

void AddSphere(vec3 pos, float radius, color_t color) {
    for (int i = 0; i < SPHERE_HEIGHT; i++) {
        for (int j = 0; j < SPHERE_WIDTH; j++) {
            AddLine(pos + (sphere_p(i, j) * radius), pos + (sphere_p(i+1, j+1) * radius), color);
        }
    }
}
    
void AddCylinder(vec3 pos, float height, float radius, color_t color) {
    // TODO: implement
}

void AddCube(vec3 pos, float height, float radius, color_t color) {
    // TODO: implement
}

void Project(const vec3& point, vec3& result, layer_t layer) {
    result = glm::project(point, 
                          view_properties[layer].view, 
                          view_properties[layer].projection,
                          vec4(0.0f, 0.0f, screen_width, screen_height));
    result.y = screen_height - result.y;
}

vec3 ProjectInverse(vec3 point, layer_t layer) {
    point.y =  screen_height - point.y;
    vec3 result = glm::unProject(point,
                                 view_properties[layer].view,
                                 view_properties[layer].projection,
                                 vec4(0.0f, 0.0f, screen_width, screen_height));
    return result;
}

static uint32_t last_material_type = MATERIAL_LAST;
static uint32_t last_vertex_format = MATERIAL_LAST;

static const char* vertex_format_names[MAX_VERTEX_FORMATS] = {
    "VERTEX_STATIC",
    "VERTEX_DYNAMIC",
    "VERTEX_SPRITE",
    "VERTEX_LINE"
};

static const char* material_type_names[MAX_MATERIAL_TYPES] = {
    "MATERIAL_TEXTURE",
    "MATERIAL_TEXTURE_ALPHA",
    "MATERIAL_LIGHTMAP",
    "MATERIAL_MSDF",
    "MATERIAL_GLYPH",
    "MATERIAL_WATER",
    "MATERIAL_FLAT_COLOR"
};

vertexformat_t RegisterVertexFormat(const char* name) {
    assert(last_vertex_format < MAX_MATERIAL_TYPES);
    
    vertex_format_names[last_vertex_format] = name;
    
    return last_vertex_format++;
}

materialtype_t RegisterMaterialType(const char* name) {
    assert(last_material_type < MAX_MATERIAL_TYPES);
    
    material_type_names[last_material_type] = name;
    
    return last_material_type++;
}

vertexformat_t FindVertexFormat(const char* name) {
    for (uint32_t i = 0; i < last_vertex_format; i++) {
        if (strcmp(vertex_format_names[i], name) == 0) return i;
    }
    
    return -1;
}

materialtype_t FindMaterialType(const char* name) {
    for (uint32_t i = 0; i < last_material_type; i++) {
        if (strcmp(material_type_names[i], name) == 0) return i;
    }
    
    return -1;
}

const char* GetVertexFormatName(vertexformat_t type) {
    assert(type < MAX_VERTEX_FORMATS);
    
    return vertex_format_names[type];
}

const char* GetMaterialTypeName(materialtype_t type) {
    assert(type < MAX_MATERIAL_TYPES);
    
    return material_type_names[type];
}


}