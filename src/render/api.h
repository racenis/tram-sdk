#ifndef TRAM_SDK_RENDER_API_H
#define TRAM_SDK_RENDER_API_H

#include <render/opengl/api.h>

namespace tram::Render::API {

void Init();
drawlistentry_t InsertDrawListEntry();
uint32_t GetFlags(drawlistentry_t entry);
void SetFlags(drawlistentry_t entry, uint32_t flags);
void SetLayer(drawlistentry_t entry, layer_t layer);
void SetPose(drawlistentry_t entry, Pose* pose);
void SetLightmap(drawlistentry_t entry, texturehandle_t lightmap);
void SetLights(drawlistentry_t entry, light_t* lights);
void SetMatrix(drawlistentry_t entry, const mat4& matrix);
void SetDrawListVertexArray(drawlistentry_t entry, vertexarray_t vertex_array_handle);
void SetDrawListIndexArray(drawlistentry_t entry, indexarray_t index_array_handle);
void SetDrawListIndexRange(drawlistentry_t entry, uint32_t index_offset, uint32_t index_length);
void SetDrawListShader(drawlistentry_t entry, vertexformat_t vertex_format, materialtype_t material_type);
void SetDrawListTextures(drawlistentry_t entry, size_t texture_count, texturehandle_t* textures);
void RemoveDrawListEntry(drawlistentry_t entry);
texturehandle_t CreateTexture(ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data);
void CreateIndexedVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array, indexarray_t& index_array, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data);
void CreateVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array);
void UpdateVertexArray(vertexarray_t& vertex_buffer, size_t data_size, void* data);
void RenderFrame();
void SetScreenSize(float width, float height);
void SetScreenClear(vec3 clear_color, bool clear);
void SetLightingParameters(vec3 sun_direction, vec3 sun_color, vec3 ambient_color, layer_t layer);
void SetViewParameters(vec3 position, quat rotation, layer_t layer);
light_t MakeLight();
void DeleteLight(light_t light);
void SetLightParameters(light_t light, vec3 location, vec3 color, float distance, vec3 direction, float exponent);
void RegisterShader(vertexformat_t format, materialtype_t type, const char* vertex_shader, const char* fragment_shader);
void SetDrawListColors(drawlistentry_t entry, size_t count, vec4* color);
void SetDrawListSpecularities(drawlistentry_t entry, size_t count, float* weights, float* exponents, float* transparencies);
void SetDrawListTextureOffsets(drawlistentry_t entry, size_t count, vec4* offset);
void SetViewMatrix(const mat4& matrix, layer_t layer);
void SetProjectionMatrix(const mat4& matrix, layer_t layer);
void GetScreen(char* buffer, int w, int h);

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

}

#endif // TRAM_SDK_RENDER_API_H