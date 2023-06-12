#ifndef TRAM_SDK_RENDER_API_H
#define TRAM_SDK_RENDER_API_H

#include <render/opengl/api.h>

namespace tram::Render {

/// Inserts an entry into the draw list and returns a handle.
inline drawlistentry_t InsertDrawListEntry () {
    return OpenGL::InsertDrawListEntry();
}

/// Returns the flags of a draw list entry.
inline uint32_t GetFlags (drawlistentry_t entry) {
    return OpenGL::GetFlags(entry);
}

/// Sets the flags of a draw list entry.
/// @param flags Bitmask constructed from Render::renderflags.
inline void SetFlags (drawlistentry_t entry, uint32_t flags) {
    return OpenGL::SetFlags(entry, flags);
}

/// Sets a pose to a draw list entry.
inline void SetPose (drawlistentry_t entry, Pose* pose) {
    return OpenGL::SetPose(entry, pose);
}

/// Sets a lightmap to a draw list entry.
inline void SetLightmap (drawlistentry_t entry, uint32_t lightmap) {
    return OpenGL::SetLightmap(entry, lightmap);
}

/// Sets the lights for a draw list entry.
/// @param lights Array of 4 indices into the light list.
inline void SetLights (drawlistentry_t entry, uint32_t* lights) {
    return OpenGL::SetLights(entry, lights);
}

/// Sets the location of a draw list entry.
inline void SetLocation (drawlistentry_t entry, glm::vec3& location) {
    return OpenGL::SetLocation(entry, location);
}

/// Sets the rotation of a draw list entry.
inline void SetRotation (drawlistentry_t entry, glm::quat& rotation) {
    return OpenGL::SetRotation(entry, rotation);
}

/// Sets the vertex array of a draw list entry.
inline void SetDrawListVertexArray (drawlistentry_t entry, uint32_t vertex_array_handle) {
    return OpenGL::SetDrawListVertexArray(entry, vertex_array_handle);
}

/// Sets the index array of a draw list entry.
inline void SetDrawListIndexRange (drawlistentry_t entry, uint32_t index_offset, uint32_t index_length) {
    return OpenGL::SetDrawListIndexRange(entry, index_offset, index_length);
}

/// Sets the shader of a draw list entry.
inline void SetDrawListShader (drawlistentry_t entry, vertexformat_t vertex_format, materialtype_t material_type) {
    return OpenGL::SetDrawListShader(entry, vertex_format, material_type);
}

/// Sets the textures of a draw list entry.
/// @param textures         Array of texture handles.
/// @param texture_count    Length of the 'textures' array. Valid lengths are 0 to 16.
inline void SetDrawListTextures (drawlistentry_t entry, size_t texture_count, uint32_t* textures) {
    return OpenGL::SetDrawListTextures(entry, texture_count, textures);
}

/// Removes an entry from the draw list.
inline void RemoveDrawListEntry (drawlistentry_t entry) {
    return OpenGL::RemoveDrawListEntry(entry);
}

/// Creates a texture.
inline uint32_t CreateTexture (ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data) {
    return OpenGL::CreateTexture(color_mode, texture_filter, width, height, data);
}

/// Creates an indexed vertex array.
inline void CreateIndexedVertexArray (const std::vector<VertexAttribute>& vertex_format, vertexhandle_t& vertex_buffer_handle, vertexhandle_t& index_buffer_handle,  vertexhandle_t& vertex_array_handle, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data) {
    return OpenGL::CreateIndexedVertexArray(vertex_format, vertex_buffer_handle, index_buffer_handle, vertex_array_handle, vertex_size, vertex_data, index_size, index_data);
}

/// Creates a regular vertex array.
inline void CreateVertexArray (const std::vector<VertexAttribute>& vertex_format, vertexhandle_t& vertex_buffer_handle,  vertexhandle_t& vertex_array_handle) {
    return OpenGL::CreateVertexArray(vertex_format, vertex_buffer_handle, vertex_array_handle);
}

/// Changes the contents of a regular vertex array.
inline void UpdateVertexArray (vertexhandle_t vertex_buffer_handle, size_t data_size, void* data) {
    return OpenGL::UpdateVertexArray(vertex_buffer_handle, data_size, data);
}

/// Renders all of the entries in the draw list.
inline void RenderFrame () {
    return OpenGL::RenderFrame();
}

/// Sets the rendering image size.
inline void SetScreenSize (float width, float height) {
    return OpenGL::SetScreenSize(width, height);
}

/// Sets the screen clearing behavior.
/// If clear is set to true, then screen will be cleared with clear_color
/// before rendering each frame.
inline void SetScreenClear (vec3 clear_color, bool clear) {
    return OpenGL::SetScreenClear(clear_color, clear);
}

inline void SetLightingParameters (vec3 sun_direction, vec3 sun_color, vec3 ambient_color, uint32_t layer) {
    OpenGL::SetLightingParameters (sun_direction, sun_color, ambient_color, layer);
}

inline void SetCameraParameters (vec3 position, quat rotation, uint32_t layer) {
    OpenGL::SetCameraParameters (position, rotation, layer);
}

inline light_t MakeLight () {
    return OpenGL::MakeLight ();
}

inline void DeleteLight (light_t light) {
    OpenGL::DeleteLight (light);
}

inline void SetLightParameters (light_t light, vec3 location, vec3 color, float distance) {
    OpenGL::SetLightParameters (light, location, color, distance);
}

}

#endif // TRAM_SDK_RENDER_API_H