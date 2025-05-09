// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

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

/**
 * @namespace tram::Render
 * 
 * High-level Render system API.
 */

using namespace tram;
using namespace tram::Render;

namespace tram::Render {

struct {
    vec3 view_position = {0.0f, 0.0f, 0.0f};
    quat view_rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    float view_fov = 60.0f;
    float near_plane = 0.01f;
    float far_plane = 750.0f;
    float ortho_ratio = 1.0f;
    
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

// TODO: make these static???

vertexarray_t colorlines_vertex_array = {};
drawlistentry_t colorlines_entry;

vertexarray_t debugtext_vertex_array = {};
drawlistentry_t debugtext_entry;
Sprite* font_debug = nullptr;

std::vector<LineVertex> colorlines;
std::vector<SpriteVertex> textvertices;

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
        matrix = glm::ortho((-ratio) / view_properties[layer].ortho_ratio,
                            (+ratio) / view_properties[layer].ortho_ratio,
                            (-(1.0f/ratio)) / view_properties[layer].ortho_ratio,
                            (+(1.0f/ratio)) / view_properties[layer].ortho_ratio,
                            //view_properties[layer].near_plane,
                            -view_properties[layer].far_plane,
                            view_properties[layer].far_plane);
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
    assert(System::IsInitialized(System::CORE));
    assert(System::IsInitialized(System::UI));
    
    API::Init();
    
    // this will initialize the projection matrices
    Render::SetScreenSize(screen_width, screen_height);
    
    // this is for rendering lines
    CreateVertexArray (GetVertexDefinition(VERTEX_LINE), colorlines_vertex_array);
    colorlines_entry = InsertDrawListEntry ();
    SetDrawListVertexArray(colorlines_entry, colorlines_vertex_array);
    SetDrawListShader(colorlines_entry, VERTEX_LINE, MATERIAL_FLAT_COLOR);
    SetFlags(colorlines_entry, FLAG_RENDER /*| FLAG_NO_DEPTH_TEST*/ | FLAG_DRAW_LINES | FLAG_DISABLE_LIGHTING);
    SetLayer(colorlines_entry, LAYER_DEBUG);
    
    // generating fullbright lightmap
    Material* fullbright = Material::Make("fullbright", MATERIAL_LIGHTMAP);
    fullbright->MakePattern({1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f});
    fullbright->LoadFromMemory();
    
    // generating fulldark environmentmap
    Material* fulldark = Material::Make("fulldark", MATERIAL_LIGHTMAP);
    fulldark->MakePattern({1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f});
    fulldark->LoadFromMemory();
    
    // generating default texture
    Material* defaulttexture = Material::Make("defaulttexture", MATERIAL_TEXTURE);
    defaulttexture->MakePattern({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f});
    defaulttexture->LoadFromMemory();
    
    // this is for rendering debug text
    Material::Make(UID("ui/font_debug"), MATERIAL_GLYPH)->Load();
    font_debug = Sprite::Find(UID("font_debug"));
    font_debug->AddReference();
    font_debug->Load();
    
    CreateVertexArray(GetVertexDefinition(VERTEX_SPRITE), debugtext_vertex_array);
    debugtext_entry = InsertDrawListEntry();
    SetDrawListVertexArray(debugtext_entry, debugtext_vertex_array);
    SetDrawListShader(debugtext_entry, VERTEX_SPRITE, MATERIAL_GLYPH);
    SetFlags(debugtext_entry, FLAG_RENDER /*| FLAG_NO_DEPTH_TEST*/);
    SetLayer(debugtext_entry, LAYER_GUI);
    
    material_t debugtext_texture = font_debug->GetMaterial()->GetMaterial();
    SetDrawListMaterials(debugtext_entry, 1, &debugtext_texture);
    
    System::SetInitialized(System::RENDER, true);
}

/// Renders a single frame.
void Render () {
    Stats::Start(System::RENDER);
#ifndef ENGINE_EDITOR_MODE
    // idk if these need to be here -> after all, the armatures aren't updated in here?
    for (auto& it : PoolProxy<SpriteComponent>::GetPool()) it.Update();
    for (auto& it : PoolProxy<ParticleComponent>::GetPool()) it.Update();
#endif // ENGINE_EDITOR_MODE
    
    // upload debug lines
    UpdateVertexArray(colorlines_vertex_array, colorlines.size() * sizeof(LineVertex), &colorlines[0]);
    SetDrawListVertexArray(colorlines_entry, colorlines_vertex_array);
    SetDrawListIndexRange(colorlines_entry, 0, colorlines.size());
    colorlines.clear();
    
    // upload debug texts
    UpdateVertexArray(debugtext_vertex_array, textvertices.size() * sizeof(SpriteVertex), &textvertices[0]);
    SetDrawListIndexRange(debugtext_entry, 0, textvertices.size());
    textvertices.clear();
    
    RenderFrame();
    Stats::Stop(System::RENDER);
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

void SetOrthoRatio(float ratio, layer_t layer) {
    view_properties[layer].ortho_ratio = ratio;
    update_projection(layer);
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

/// Draws a line for a single frame.
void AddLine(vec3 from, vec3 to, vec3 color) {
    colorlines.push_back({from, color});
    colorlines.push_back({to, color});
}

/// Draws a cross marker for a single frame.
void AddLineMarker(vec3 location, vec3 color) {
    AddLine(location + vec3(-1.0f, 0.0f, 0.0f), location + vec3(1.0f, 0.0f, 0.0f), color);
    AddLine(location + vec3(0.0f, -1.0f, 0.0f), location + vec3(0.0f, 1.0f, 0.0f), color);
    AddLine(location + vec3(0.0f, 0.0f, -1.0f), location + vec3(0.0f, 0.0f, 1.0f), color);
}

/// Draws an AABB box for a single frame.
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

/// Draws a sphere for a single frame.
void AddSphere(vec3 pos, float radius, color_t color) {
    for (int i = 0; i < SPHERE_HEIGHT; i++) {
        for (int j = 0; j < SPHERE_WIDTH; j++) {
            AddLine(pos + (sphere_p(i, j) * radius), pos + (sphere_p(i+1, j+1) * radius), color);
        }
    }
}

/// Draws a cylinder for a single frame.
/// @note Not implmented.
void AddCylinder(vec3 pos, float height, float radius, color_t color) {
    // TODO: implement
}

/// Draws a cube for a single frame.
/// @note Not implmented.
void AddCube(vec3 pos, float height, float radius, color_t color) {
    // TODO: implement
}

/// Draws text, in 3D space, for a single frame.
void AddText(vec3 pos, const char* text, color_t color) {
    Project(pos, pos);
    if (pos.z > 1.0f) return;
    
    uint16_t w = 0;
    uint16_t h = 16;
    
    uint16_t line = 0;
    for (const char* c = text; *c != '\0'; c++) {
        if (*c=='\n') {
            h += 16;
            if (line > w) w = line;
            line = 0;
        } else {
            //line += 16;
            line += font_debug->GetFrames()[*c].width;
        }
    }
    
    AddText(pos.x - w / 2.0f, pos.y - h / 2.0f, text, color);
}

/// Draws text, on the screen, for a single frame.
void AddText(float x, float y, const char* text, color_t color) {
    float cur_x = x;
    float cur_y = y;
    
    for (const char* c = text; *c != '\0'; c++) {
        if (*c=='\n'){ cur_y += 16.0f;
            cur_x = x;
            continue;}
            
        const auto& info = font_debug->GetFrames()[*c];
        
        float x = cur_x;
        float y = cur_y;
        float w = info.width;
        float h = info.height;
        float tex_x = info.offset_x;
        float tex_y = info.offset_y;
        float tex_w = info.width;
        float tex_h = info.height;
        
        SpriteVertex tleft;
        SpriteVertex tright;
        SpriteVertex bleft;
        SpriteVertex bright;

        tleft.co.x = x;
        tleft.co.y = y;
        tleft.co.z = 1;
        tleft.texco.x = tex_x;
        tleft.texco.y = tex_y;
        
        tright.co.x = x + w;
        tright.co.y = y;
        tright.co.z = 1;
        tright.texco.x = tex_x + tex_w;
        tright.texco.y = tex_y;
        
        bleft.co.x = x;
        bleft.co.y = y + h;
        bleft.co.z = 1;
        bleft.texco.x = tex_x;
        bleft.texco.y = tex_y + tex_h;
        
        bright.co.x = x + w;
        bright.co.y = y + h;
        bright.co.z = 1;
        bright.texco.x = tex_x + tex_w;
        bright.texco.y = tex_y + tex_h;
        
        tleft.color = color;
        tleft.texture = 0;
        tright.color = color;
        tright.texture = 0;
        bleft.color = color;
        bleft.texture = 0;
        bright.color = color;
        bright.texture = 0;

        textvertices.push_back(bleft);
        textvertices.push_back(bright);
        textvertices.push_back(tleft);
        textvertices.push_back(bright);
        textvertices.push_back(tright);
        textvertices.push_back(tleft);
        
        //DrawGlyph(font, (unsigned char)*c, cursor_x, cursor_y);
        
        
        //cursor_x += GlyphWidth(font, (unsigned char)*c);]
        cur_x += info.width;
    }
}

/// Projects a point into screen coordinates.
void Project(const vec3& point, vec3& result, layer_t layer) {
    result = glm::project(point, 
                          view_properties[layer].view, 
                          view_properties[layer].projection,
                          vec4(0.0f, 0.0f, screen_width, screen_height));
    result.y = screen_height - result.y;
}

/// Reverses screen coordinates into world coordinates.
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
    "MATERIAL_TEXTURE_BLEND",
    "MATERIAL_LIGHTMAP",
    "MATERIAL_ENVIRONMENTMAP",
    "MATERIAL_MSDF",
    "MATERIAL_GLYPH",
    "MATERIAL_WATER",
    "MATERIAL_FLAT_COLOR"
};

/// Allocates a new vertex format.
vertexformat_t RegisterVertexFormat(const char* name) {
    assert(last_vertex_format < MAX_MATERIAL_TYPES);
    
    vertex_format_names[last_vertex_format] = name;
    
    return last_vertex_format++;
}

/// Allocates a new material type. 
materialtype_t RegisterMaterialType(const char* name) {
    assert(last_material_type < MAX_MATERIAL_TYPES);
    
    material_type_names[last_material_type] = name;
    
    return last_material_type++;
}

/// Finds an already allocated vertex format by its name.
vertexformat_t FindVertexFormat(const char* name) {
    for (uint32_t i = 0; i < last_vertex_format; i++) {
        if (strcmp(vertex_format_names[i], name) == 0) return i;
    }
    
    return -1;
}

/// Finds an already material type format by its name.
materialtype_t FindMaterialType(const char* name) {
    for (uint32_t i = 0; i < last_material_type; i++) {
        if (strcmp(material_type_names[i], name) == 0) return i;
    }
    
    return -1;
}

/// Finds an allocated vertex format's name.
const char* GetVertexFormatName(vertexformat_t type) {
    assert(type < MAX_VERTEX_FORMATS);
    
    return vertex_format_names[type];
}

/// Finds an allocated material type's name.
const char* GetMaterialTypeName(materialtype_t type) {
    assert(type < MAX_MATERIAL_TYPES);
    
    return material_type_names[type];
}


}