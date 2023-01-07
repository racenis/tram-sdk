// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/renderer.h>

namespace Core::Render {
    /// For more information, refer to the documentation section "Renderer API".
    // TODO: actually write up said documentation section
    
    /// Inserts an entry into the draw list and returns a handle.
    DrawListEntryHandle (*InsertDrawListEntry) () = nullptr;
    
    /// Inserts an entry into the draw list, initializes it to a model and returns a handle. 
    DrawListEntryHandle (*InsertDrawListEntryFromModel) (Model* model) = nullptr;
    
    /// Returns the flags of a draw list entry.
    uint32_t (*GetFlags) (DrawListEntryHandle entry) = nullptr;
    
    /// Sets the flags of a draw list entry.
    /// @param flags Bitmask constructed from Render::renderflags.
    void (*SetFlags) (DrawListEntryHandle entry, uint32_t flags) = nullptr;
    
    /// Sets a pose to a draw list entry.
    void (*SetPose) (DrawListEntryHandle entry, Pose* pose) = nullptr;
    
    /// Sets a lightmap to a draw list entry.
    void (*SetLightmap) (DrawListEntryHandle entry, uint32_t lightmap) = nullptr;
    
    /// Sets the lights for a draw list entry.
    /// @param lights Array of 4 indices into the light list.
    void (*SetLights) (DrawListEntryHandle entry, uint32_t* lights) = nullptr;
    
    /// Sets the location of a draw list entry.
    void (*SetLocation) (DrawListEntryHandle entry, glm::vec3& location) = nullptr;
    
    /// Sets the rotation of a draw list entry.
    void (*SetRotation) (DrawListEntryHandle entry, glm::quat& rotation) = nullptr;
    
    /// Sets the vertex array of a draw list entry.
    void (*SetDrawListVertexArray) (DrawListEntryHandle entry, uint32_t vertex_array_handle) = nullptr;
    
    /// Sets the element array of a draw list entry.
    void (*SetDrawListElements) (DrawListEntryHandle entry, uint32_t element_offset, uint32_t element_length) = nullptr;
    
    /// Sets the shader of a draw list entry.
    void (*SetDrawListShader) (DrawListEntryHandle entry, Model::VertexFormat vertex_format, Material::Type material_type) = nullptr;
    
    /// Sets the textures of a draw list entry.
    /// @param textures         Array of texture handles.
    /// @param texture_count    Length of the 'textures' array. Valid lengths are 0 to 16.
    void (*SetDrawListTextures) (DrawListEntryHandle entry, size_t texture_count, uint32_t* textures) = nullptr;
    
    /// Removes an entry from the draw list.
    void (*RemoveDrawListEntry) (DrawListEntryHandle entry) = nullptr;
    
    /// Creates a texture.
    uint32_t (*CreateTexture) (ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data) = nullptr;
    
    /// Creates an indexed vertex array.
    void (*CreateIndexedVertexArray) (const VertexDefinition& vertex_format, vertexhandle_t& vertex_buffer_handle, vertexhandle_t& element_buffer_handle,  vertexhandle_t& vertex_array_handle, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data) = nullptr;
    
    /// Creates a regular vertex array.
    void (*CreateVertexArray) (const VertexDefinition& vertex_format, vertexhandle_t& vertex_buffer_handle,  vertexhandle_t& vertex_array_handle) = nullptr;
    
    /// Changes the contents of a regular vertex array.
    void (*UpdateVertexArray) (vertexhandle_t vertex_buffer_handle, size_t data_size, void* data) = nullptr;
    
    /// Renders all of the entries in the draw list.
    void (*RenderFrame) () = nullptr;
    
    /// Sets the rendering image size.
    void (*SetScreenSize) (float width, float height) = nullptr;
}