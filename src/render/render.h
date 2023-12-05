// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_RENDER_H
#define TRAM_SDK_RENDER_H

#include <vector>

#include <render/animation.h>

namespace tram::Render {

class Model;

extern bool DRAW_RENDER_DEBUG;

extern bool THIRD_PERSON;

// these are for the OpenGL renderer only.
// when more renderers will be added, we'll
// probably switch these from int to void*
typedef uint32_t vertexhandle_t;
typedef uint32_t texturehandle_t;
typedef void* drawlistentry_t;

typedef vec3 color_t;
typedef uint32_t layer_t;

typedef uint32_t vertexformat_t;
typedef uint32_t materialtype_t;

typedef void* light_t;

void Init();
void Render();

void Project(const glm::vec3& point, glm::vec3& result);
vec3 ProjectInverse(vec3 point);

void SetSunDirection(vec3 direction, layer_t layer = 0);
void SetSunColor(color_t color, layer_t layer = 0);
void SetAmbientColor(color_t color, layer_t layer = 0);

void SetCameraPosition(vec3 position, layer_t layer = 0);
void SetCameraRotation(quat rotation, layer_t layer = 0);

vec3 GetCameraPosition(layer_t layer = 0);
quat GetCameraRotation(layer_t layer = 0);

void SetSun(float timeOfDay);

void AddLine(vec3 from, vec3 to, color_t color);
void AddLineMarker(vec3 location, color_t color);
void AddLineAABB(vec3 min, vec3 max, vec3 center, quat rotation, vec3 color);
void AddSphere(vec3 pos, float radius, color_t color);
void AddCylinder(vec3 pos, float height, float radius, color_t color);
void AddCube(vec3 pos, float height, float radius, color_t color);

const color_t COLOR_WHITE  (1.0f, 1.0f, 1.0f);
const color_t COLOR_RED    (1.0f, 0.0f, 0.0f);
const color_t COLOR_GREEN  (0.0f, 1.0f, 0.0f);
const color_t COLOR_BLUE   (0.0f, 0.0f, 1.0f);
const color_t COLOR_YELLOW (1.0f, 1.0f, 0.0f);
const color_t COLOR_PINK   (1.0f, 0.0f, 1.0f);
const color_t COLOR_CYAN   (0.0f, 1.0f, 1.0f);
const color_t COLOR_BLACK  (0.0f, 0.0f, 0.0f);
const color_t COLOR_GRAY   (0.3f, 0.3f, 0.3f);

enum VertexFormats : vertexformat_t {
    VERTEX_STATIC,
    VERTEX_DYNAMIC,
    VERTEX_SPRITE,
    VERTEX_LINE,
    VERTEX_LAST
};

enum MaterialTypes : materialtype_t {
    MATERIAL_TEXTURE,
    MATERIAL_TEXTURE_ALPHA,
    MATERIAL_LIGHTMAP,
    MATERIAL_MSDF,
    MATERIAL_GLYPH,
    MATERIAL_WATER,
    MATERIAL_FLAT_COLOR,
    MATERIAL_LAST
};

vertexformat_t RegisterVertexFormat(const char* name);
materialtype_t RegisterMaterialType(const char* name);

const char* GetVertexFormatName(vertexformat_t type);
const char* GetMaterialTypeName(materialtype_t type);

}

#endif // TRAM_SDK_RENDER_H
