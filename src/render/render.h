// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_H
#define TRAM_SDK_RENDER_H

#include <vector>

#include <render/animation.h>

struct IDirect3DVertexBuffer9;
struct IDirect3DIndexBuffer9;
struct IDirect3DTexture9;

namespace tram::Render {

typedef vec3 color_t;
typedef uint32_t layer_t;

typedef uint32_t vertexformat_t;
typedef uint32_t materialtype_t;

void Init();
void Render();

void Project(const vec3& point, vec3& result, layer_t layer = 0);
vec3 ProjectInverse(vec3 point, layer_t layer = 0);

void SetSunDirection(vec3 direction, layer_t layer = 0);
void SetSunColor(color_t color, layer_t layer = 0);
void SetAmbientColor(color_t color, layer_t layer = 0);

void SetScreenSize(float width, float height);

void SetOrthoRatio(float ratio, layer_t layer = 0);

void SetViewFov(float fov, layer_t layer = 0);
float GetViewFov(layer_t layer = 0);

void SetViewDistance(float dist, layer_t layer = 0);
float GetViewDistance(layer_t layer = 0);

void SetViewPosition(vec3 position, layer_t layer = 0);
void SetViewRotation(quat rotation, layer_t layer = 0);

vec3 GetViewPosition(layer_t layer = 0);
quat GetViewRotation(layer_t layer = 0);

const color_t COLOR_WHITE  (1.0f, 1.0f, 1.0f);
const color_t COLOR_RED    (1.0f, 0.0f, 0.0f);
const color_t COLOR_GREEN  (0.0f, 1.0f, 0.0f);
const color_t COLOR_BLUE   (0.0f, 0.0f, 1.0f);
const color_t COLOR_YELLOW (1.0f, 1.0f, 0.0f);
const color_t COLOR_PINK   (1.0f, 0.0f, 1.0f);
const color_t COLOR_CYAN   (0.0f, 1.0f, 1.0f);
const color_t COLOR_BLACK  (0.0f, 0.0f, 0.0f);
const color_t COLOR_GRAY   (0.3f, 0.3f, 0.3f);

void AddLine(vec3 from, vec3 to, color_t color);
void AddLineMarker(vec3 location, color_t color);
void AddLineAABB(vec3 min, vec3 max, vec3 center, quat rotation, vec3 color);
void AddSphere(vec3 pos, float radius, color_t color);
void AddCylinder(vec3 pos, float height, float radius, color_t color);
void AddCube(vec3 pos, float height, float radius, color_t color);
void AddText(vec3 pos, const char* text, color_t color = COLOR_WHITE);
void AddText(float x, float y, const char* text, color_t color = COLOR_WHITE);

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
    MATERIAL_TEXTURE_BLEND,
    MATERIAL_LIGHTMAP,
    MATERIAL_ENVIRONMENTMAP,
    MATERIAL_MSDF,
    MATERIAL_GLYPH,
    MATERIAL_WATER,
    MATERIAL_FLAT_COLOR,
    MATERIAL_LAST
};

enum MaterialFilter {
    FILTER_NEAREST,
    FILTER_LINEAR
};

struct SphericalHarmonic {
    vec3 l00, l1m1, l10, l11, l2m2, l2m1, l20, l21, l22;
};

typedef SphericalHarmonic sphericalharmonic_t;

vertexformat_t RegisterVertexFormat(const char* name);
materialtype_t RegisterMaterialType(const char* name);

vertexformat_t FindVertexFormat(const char* name);
materialtype_t FindMaterialType(const char* name);

const char* GetVertexFormatName(vertexformat_t type);
const char* GetMaterialTypeName(materialtype_t type);

namespace API { struct SWVertexArray; }
namespace API { struct RTVertexArray; }

struct vertexarray_t {
    union {
        struct {
            uint32_t gl_vertex_array;
            uint32_t gl_vertex_buffer;
        };
        struct {
            IDirect3DVertexBuffer9* d3d_vertex_buffer;
            uint32_t d3d_metadata;
        };
        API::SWVertexArray* sw_vertex_array;
        API::RTVertexArray* rt_vertex_array;
        void* generic;
    };
};

namespace API { struct SWIndexArray; }
namespace API { struct RTIndexArray; }

struct indexarray_t {
    union {
        uint32_t gl_index_buffer;
        IDirect3DIndexBuffer9* d3d_index_buffer;
        API::SWIndexArray* sw_index_array;
        API::RTIndexArray* rt_index_array;
        void* generic;
    };
};

struct spritearray_t {
    union {
        vertexarray_t vertex_array;
        void* generic;
    };
    
    // TODO: add whatever you need for the D3D renderer
    // TODO: also add everything for the SW renderer
};

namespace API { struct GLDrawListEntry; }
namespace API { struct D3DDrawListEntry; }
namespace API { struct SWDrawListEntry; }
namespace API { struct RTDrawListEntry; }

struct drawlistentry_t {
    union {
        API::GLDrawListEntry* gl;
        API::D3DDrawListEntry* d3d;
        API::SWDrawListEntry* sw;
        API::RTDrawListEntry* rt;
        void* generic;
    };
};

namespace API { struct SWTexture; }
namespace API { struct RTTexture; }

struct texturehandle_t {
    union {
        uint32_t gl_texture_handle;
        IDirect3DTexture9* d3d_texture_handle;
        API::SWTexture* sw_texture;
        API::RTTexture* rt_texture;
        void* generic;
    };
};

namespace API { struct GLLight; }
namespace API { struct D3DLight; }
namespace API { struct SWLight; }
namespace API { struct RTLight; }

struct light_t {
    union {
        API::GLLight* gl;
        API::D3DLight* d3d;
        API::SWLight* sw;
        API::RTLight* rt;
        void* generic;
    };
};

namespace API { struct GLMaterial; }
namespace API { struct D3DMaterial; }
namespace API { struct SWMaterial; }
namespace API { struct RTMaterial; }

struct material_t {
    union {
        API::GLMaterial* gl;
        API::D3DMaterial* d3d;
        API::SWMaterial* sw;
        API::RTMaterial* rt;
        void* generic;
    };
};

}

#endif // TRAM_SDK_RENDER_H
