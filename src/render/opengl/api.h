#ifndef TRAM_SDK_RENDER_OPENGL_API_H
#define TRAM_SDK_RENDER_OPENGL_API_H

#include <render/render.h>
#include <render/renderer.h>

namespace tram::Render::OpenGL {

void Init();

void Render();

void SetScreenSize (float width, float height);

drawlistentry_t InsertDrawListEntry ();

uint32_t GetFlags (drawlistentry_t entry);
void SetFlags (drawlistentry_t entry, uint32_t flags);
void SetPose (drawlistentry_t entry, Pose* pose);
void SetLightmap (drawlistentry_t entry, uint32_t lightmap);
void SetLights (drawlistentry_t entry, uint32_t* lights);
void SetMatrix (drawlistentry_t entry, const mat4& matrix);
void SetDrawListVertexArray (drawlistentry_t entry, uint32_t vertex_array_handle);
void SetDrawListIndexRange (drawlistentry_t entry, uint32_t index_offset, uint32_t index_length);
void SetDrawListShader (drawlistentry_t entry, vertexformat_t vertex_format, materialtype_t material_type);
void SetDrawListTextures (drawlistentry_t entry, size_t texture_count, uint32_t* textures);
void SetScreenClear (vec3 clear_color, bool clear);

void SetLightingParameters (vec3 sun_direction, vec3 sun_color, vec3 ambient_color, uint32_t layer);
void SetCameraParameters (vec3 position, quat rotation, uint32_t layer);

light_t MakeLight ();
void DeleteLight (light_t light);

void SetLightParameters (light_t light, vec3 location, vec3 color, float distance);

void RemoveDrawListEntry (drawlistentry_t entry);

uint32_t CreateTexture (ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data);

// this bit of the API is a bit leaky
// might change later!
void CreateIndexedVertexArray (VertexDefinition vertex_format, vertexhandle_t& vertex_buffer_handle, vertexhandle_t& index_buffer_handle,  vertexhandle_t& vertex_array_handle, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data);
void CreateVertexArray (VertexDefinition vertex_format, vertexhandle_t& vertex_buffer_handle,  vertexhandle_t& vertex_array_handle);
void UpdateVertexArray (vertexhandle_t vertex_buffer_handle, size_t data_size, void* data);

void RenderFrame ();

}

#endif // TRAM_SDK_RENDER_OPENGL_API_H