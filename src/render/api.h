// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_API_H
#define TRAM_SDK_RENDER_API_H

#include <render/render.h>
#include <render/renderer.h>

/**
 * @namespace tram::Render::API
 * 
 * Rendering backend API.
 */

namespace tram::Render::API {

// meta functions
void Init();
void RenderFrame();

void SetScreenSize(float width, float height);
void SetScreenClear(vec3 clear_color, bool clear);

void SetLightingParameters(vec3 sun_direction, vec3 sun_color, vec3 ambient_color, layer_t layer);
void SetFogParameters(vec3 color, float near, float far, layer_t layer);

void SetViewMatrix(const mat4& matrix, layer_t layer);
void SetProjectionMatrix(const mat4& matrix, layer_t layer);

bool IsInteractiveMode();
void SetInteractiveMode(bool is_interactive);

void GetScreen(char* buffer, int w, int h);

void RegisterShader(vertexformat_t format, materialtype_t type, const char* vertex_shader, const char* fragment_shader);

// drawlist functions
drawlistentry_t InsertDrawListEntry();
void RemoveDrawListEntry(drawlistentry_t entry);

uint32_t GetFlags(drawlistentry_t entry);
void SetFlags(drawlistentry_t entry, uint32_t flags);
void SetLayer(drawlistentry_t entry, layer_t layer);
void SetPose(drawlistentry_t entry, Pose* pose);
void SetLightmap(drawlistentry_t entry, texturehandle_t lightmap);
void SetEnvironmentMap(drawlistentry_t entry, texturehandle_t environmentmap);
void SetSphericalHarmonic(drawlistentry_t entry, sphericalharmonic_t harmonic);
void SetLights(drawlistentry_t entry, light_t* lights);
void SetMatrix(drawlistentry_t entry, const mat4& matrix);
void SetFadeDistance(drawlistentry_t entry, float near, float far);

void SetDrawListVertexArray(drawlistentry_t entry, vertexarray_t vertex_array_handle);
void SetDrawListIndexArray(drawlistentry_t entry, indexarray_t index_array_handle);
void SetDrawListIndexRange(drawlistentry_t entry, uint32_t index_offset, uint32_t index_length);
void SetDrawListSpriteArray(drawlistentry_t entry, spritearray_t sprite_array_handle);

bool SetDrawListShader(drawlistentry_t entry, vertexformat_t vertex_format, materialtype_t material_type);

void SetDrawListMaterials(drawlistentry_t entry, size_t material_count, material_t* materials);

void SetDrawListColors(drawlistentry_t entry, size_t count, vec4* color);

void SetDrawListTextureOffsets(drawlistentry_t entry, size_t count, vec4* offset);
void SetDrawListAABB(drawlistentry_t entry, vec3 min, vec3 max);

// vertex buffer functions
void CreateIndexedVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array, indexarray_t& index_array, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data);
void CreateVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array);
void UpdateVertexArray(vertexarray_t& vertex_buffer, size_t data_size, void* data);
// TODO: add deletion functions

// texture functions
texturehandle_t CreateTexture(ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data);
void SetTextureFilter(texturehandle_t texture, TextureFilter texture_filter);
void SetTextureImage(texturehandle_t texture, ColorMode color_mode, uint32_t width, uint32_t height, void* data);
void YeetTexture(texturehandle_t texture);

// sprite functions
spritearray_t CreateSpriteArray();
void UpdateSpriteArray(spritearray_t array, size_t data_size, void* data);


// light functions
light_t MakeLight();
void YeetLight(light_t light);
void SetLightParameters(light_t light, vec3 location, vec3 color, float distance, vec3 direction, float exponent);
// TODO: decompose big function?

// material functions
material_t MakeMaterial();
void YeetMaterial(material_t material);
void SetMaterialTexture(material_t material, texturehandle_t texture);
void SetMaterialNormalMap(material_t material, texturehandle_t texture);
void SetMaterialColor(material_t material, vec4 color);
void SetMaterialSpecularWeight(material_t material, float weight);
void SetMaterialSpecularExponent(material_t material, float exponent);
void SetMaterialSpecularTransparency(material_t material, float transparency);
void SetMaterialReflectivity(material_t material, float reflectivity);

// default layers
enum LayerType {
    LAYER_DEFAULT =     0,
    LAYER_DEBUG =       1,
    LAYER_OVERLAY =     1,
    LAYER_REFLECTION =  3,
    LAYER_GUI =         2,
};

// rendering contexts
enum ContextType {
    CONTEXT_OPENGL,
    CONTEXT_WEBGL,
    CONTEXT_DIRECT3D,
    CONTEXT_SOFTWARE,
    CONTEXT_NONE
};

ContextType GetContext();
uint32_t GetMaxIndexRangeLength();
void SetDevice(void*);

bool IsDebugMode();
void SetDebugMode(bool);

}

#endif // TRAM_SDK_RENDER_API_H