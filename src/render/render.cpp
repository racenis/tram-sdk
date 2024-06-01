// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <framework/system.h>
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

// cached values for the renderer
// TODO: maybe put all of these in a struct?
static vec3 CAMERA_POSITION [7] = {{0.0f, 0.0f, 0.0f}};
static quat CAMERA_ROTATION [7] = {{1.0f, 0.0f, 0.0f, 0.0f}};

static vec3 SUN_DIRECTION [7] = {{0.0f, 1.0f, 0.0f}};
static vec3 SUN_COLOR [7] = {{1.0f, 1.0f, 1.0f}};
static vec3 AMBIENT_COLOR [7] = {{0.0f, 0.0f, 0.0f}};

float SCREEN_WIDTH = 800.0f;
float SCREEN_HEIGHT = 600.0f;

bool THIRD_PERSON = false;
bool DRAW_RENDER_DEBUG = false;

//Pool<LightListObject> lightPool("lightpool", 100, true);
//Octree<uint32_t> lightTree;

//uint32_t colorlines_vertex_array = 0;
//uint32_t colorlines_vertex_buffer = 0;
vertexarray_t colorlines_vertex_array = {.generic = 0};
drawlistentry_t colorlines_entry;

std::vector<LineVertex> colorlines;

using namespace API;

/// Initializes the rendering system.
/// @note Core and UI systems need to be initialized before initializing the render system.
void Init () {
    assert(System::IsInitialized(System::SYSTEM_CORE));
    assert(System::IsInitialized(System::SYSTEM_UI));
    
    API::Init();
    
    
    
    // this is for rendering lines
    CreateVertexArray (GetVertexDefinition(VERTEX_LINE), colorlines_vertex_array);
    colorlines_entry = InsertDrawListEntry ();
    SetDrawListVertexArray(colorlines_entry, colorlines_vertex_array);
    SetDrawListShader(colorlines_entry, VERTEX_LINE, MATERIAL_FLAT_COLOR);
    SetFlags(colorlines_entry, FLAG_RENDER | FLAG_NO_DEPTH_TEST | FLAG_DRAW_LINES);
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
#ifndef ENGINE_EDITOR_MODE
    // idk if these need to be here -> after all, the armatures aren't updated in here?
    for (auto& it : PoolProxy<SpriteComponent>::GetPool()) it.Update();
    for (auto& it : PoolProxy<ParticleComponent>::GetPool()) it.Update();
#endif // ENGINE_EDITOR_MODE
    
    UpdateVertexArray(colorlines_vertex_array, colorlines.size() * sizeof(LineVertex), &colorlines[0]);
    SetDrawListIndexRange(colorlines_entry, 0, colorlines.size());
    colorlines.clear();
    
    RenderFrame();
}

/// Sets the sun direction.
/// @param direction    Normal vector pointing towards the sun.
/// @param layer        Rendering layer to which the sun direction will be applied.
void SetSunDirection (color_t direction, layer_t layer) {
    SUN_DIRECTION [layer] = direction;
    API::SetLightingParameters(SUN_DIRECTION[layer], SUN_COLOR[layer], AMBIENT_COLOR[layer], layer);
}

/// Sets the sun color.
/// @param color    Color of the sun.
/// @param layer    Rendering layer to which the sun color will be applied.
void SetSunColor (color_t color, layer_t layer) {
    SUN_COLOR [layer] = color;
    API::SetLightingParameters(SUN_DIRECTION[layer], SUN_COLOR[layer], AMBIENT_COLOR[layer], layer);
}

/// Sets the ambient color.
/// @param color    Ambient color.
/// @param layer    Rendering layer to which the ambient color will be applied.
void SetAmbientColor (color_t color, layer_t layer) {
    AMBIENT_COLOR [layer] = color;
    API::SetLightingParameters(SUN_DIRECTION[layer], SUN_COLOR[layer], AMBIENT_COLOR[layer], layer);
}

/// Sets the camera position.
/// @param position Camera position.
/// @param layer    Rendering layer to which the camera position will be applied.
void SetCameraPosition (vec3 position, layer_t layer) {
    CAMERA_POSITION [layer] = position;
    API::SetCameraParameters(CAMERA_POSITION[layer], CAMERA_ROTATION[layer], layer);
}

/// Sets the camera rotation.
/// @param rotation Camera rotation.
/// @param layer    Rendering layer to which the camera rotation will be applied.
void SetCameraRotation (quat rotation, layer_t layer) {
    CAMERA_ROTATION [layer] = rotation;
    API::SetCameraParameters(CAMERA_POSITION[layer], CAMERA_ROTATION[layer], layer);
}

/// Returns the camera position for a given layer.
vec3 GetCameraPosition (layer_t layer) {
    return CAMERA_POSITION [layer];
}

/// Returns the camera rotation for a given layer.
quat GetCameraRotation (layer_t layer) {
    return CAMERA_ROTATION [layer];
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