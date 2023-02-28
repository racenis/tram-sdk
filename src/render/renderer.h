// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_RENDERER_H
#define RENDER_RENDERER_H

#include <templates/octree.h>

#include <render/render.h>
#include <render/model.h>

namespace Core::Render {
    /// Flags for DrawListObjects.
    enum renderflags: uint32_t {
        FLAG_NO_RENDER = 0,         //< Object is not rendered.
        FLAG_RENDER = 1,            //< Object is rendered.

        FLAG_DRAW_INDEXED = 4,      //< Vertex buffer has indicies.
        FLAG_REVERSE_SORT = 8,      //< Furthest objects are rendered first.
        FLAG_TRANSPARENT = 16,      //< Object is transparent and is rendered last.
        FLAG_NO_DEPTH_TEST = 32,    //< Object is rendered on top of other objects.
        FLAG_INTERIOR_LIGHTING = 64,//< Object is rendered with interior lighting.
        FLAG_DRAW_LINES = 128       //< Object is rendered as lines. Otherwise rendered as triangles.
    };
    
    /// Texture color mode.
    enum ColorMode {
        COLORMODE_R,    //< Contains only red channel, pixel is 1 byte long.
        COLORMODE_RG,   //< Contains red and green channels, pixel is 2 bytes long.
        COLORMODE_RGB,  //< Contains red, green and blue channels, pixel is 3 bytes long.
        COLORMODE_RGBA  //< Contains red, green, blue and alpha channels, pixel is 4 bytes long.
    };
    
    /// Texture filters.
    enum TextureFilter {
        TEXTUREFILTER_LINEAR,           //< Textures use linear filtering.
        TEXTUREFILTER_LINEAR_MIPMAPPED  //< Textures use linear filtering and are mipmapped.
    };
    
    /// Atributes for vertex buffer vertices.
    struct VertexAttribute {
        // @note None of the current renderers use this field.
        enum {
            POSITION_VECTOR,
            NORMAL_VECTOR,
            COLOR_VECTOR,
            TEXTURE_COORDINATE,
            TEXTURE_INDEX,
            LIGHTMAP_COORDINATE,
            BONE_INDEX,
            BONE_WEIGHT,
            OTHER
        } function;
        enum {
            FLOAT32,
            INT32,
            UINT32
        } type;
        uint32_t size;      //< Size of the attribute, in bytes.
        uint32_t stride;    //< Distance between same attribute in different vertices, in bytes.
        uint64_t offset;    //< Offset of beginning of attribute in vertex buffer.
    };
    
    typedef std::vector<VertexAttribute> VertexDefinition;
    
    struct LightListObject {
        vec3 location = {0.0f, 0.0f, 0.0f};
        float padding;
        vec3 color = {0.0f, 0.0f, 0.0f};
        float distance = 0.0f;
        float padding2[8];
    };

    extern Pool<LightListObject> lightPool;
    extern Octree<uint32_t> lightTree;
    
    extern Pool<LightListObject> lightPool;
    extern Octree<uint32_t> lightTree;

    
    struct RendererAPI {
        DrawListEntryHandle (*InsertDrawListEntry) () = nullptr;
        DrawListEntryHandle (*InsertDrawListEntryFromModel) (Model* model) = nullptr;
        
        void (*SetScreenSize) (float width, float height) = nullptr;
        
        uint32_t (*GetFlags) (DrawListEntryHandle entry) = nullptr;
        void (*SetFlags) (DrawListEntryHandle entry, uint32_t flags) = nullptr;
        void (*SetPose) (DrawListEntryHandle entry, Pose* pose) = nullptr;
        void (*SetLightmap) (DrawListEntryHandle entry, uint32_t lightmap) = nullptr;
        void (*SetLights) (DrawListEntryHandle entry, uint32_t* lights) = nullptr;
        void (*SetLocation) (DrawListEntryHandle entry, glm::vec3& location) = nullptr;
        void (*SetRotation) (DrawListEntryHandle entry, glm::quat& rotation) = nullptr;
        void (*SetDrawListVertexArray) (DrawListEntryHandle entry, uint32_t vertex_array_handle) = nullptr;
        void (*SetDrawListElements) (DrawListEntryHandle entry, uint32_t element_offset, uint32_t element_length) = nullptr;
        void (*SetDrawListShader) (DrawListEntryHandle entry, Model::VertexFormat vertex_format, Material::Type material_type) = nullptr;
        void (*SetDrawListTextures) (DrawListEntryHandle entry, size_t texture_count, uint32_t* textures) = nullptr;

        void (*RemoveDrawListEntry) (DrawListEntryHandle entry) = nullptr;

        uint32_t (*CreateTexture) (ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data) = nullptr;

        void (*CreateIndexedVertexArray) (const VertexDefinition& vertex_format, vertexhandle_t& vertex_buffer_handle, vertexhandle_t& element_buffer_handle,  vertexhandle_t& vertex_array_handle, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data) = nullptr;
        void (*CreateVertexArray) (const VertexDefinition& vertex_format, vertexhandle_t& vertex_buffer_handle,  vertexhandle_t& vertex_array_handle) = nullptr;
        void (*UpdateVertexArray) (vertexhandle_t vertex_buffer_handle, size_t data_size, void* data) = nullptr;

        void (*RenderFrame) () = nullptr;
    };
    
    /// Currently initialized renderer function pointers.
    extern RendererAPI RENDERER;
    
    /// Inserts an entry into the draw list and returns a handle.
    inline DrawListEntryHandle InsertDrawListEntry () {
        return RENDERER.InsertDrawListEntry();
    }
    
    /// Inserts an entry into the draw list, initializes it to a model and returns a handle. 
    inline DrawListEntryHandle InsertDrawListEntryFromModel (Model* model) {
        return RENDERER.InsertDrawListEntryFromModel(model);
    }
    
    /// Returns the flags of a draw list entry.
    inline uint32_t GetFlags (DrawListEntryHandle entry) {
        return RENDERER.GetFlags(entry);
    }
    
    /// Sets the flags of a draw list entry.
    /// @param flags Bitmask constructed from Render::renderflags.
    inline void SetFlags (DrawListEntryHandle entry, uint32_t flags) {
        return RENDERER.SetFlags(entry, flags);
    }
    
    /// Sets a pose to a draw list entry.
    inline void SetPose (DrawListEntryHandle entry, Pose* pose) {
        return RENDERER.SetPose(entry, pose);
    }
    
    /// Sets a lightmap to a draw list entry.
    inline void SetLightmap (DrawListEntryHandle entry, uint32_t lightmap) {
        return RENDERER.SetLightmap(entry, lightmap);
    }
    
    /// Sets the lights for a draw list entry.
    /// @param lights Array of 4 indices into the light list.
    inline void SetLights (DrawListEntryHandle entry, uint32_t* lights) {
        return RENDERER.SetLights(entry, lights);
    }
    
    /// Sets the location of a draw list entry.
    inline void SetLocation (DrawListEntryHandle entry, glm::vec3& location) {
        return RENDERER.SetLocation(entry, location);
    }
    
    /// Sets the rotation of a draw list entry.
    inline void SetRotation (DrawListEntryHandle entry, glm::quat& rotation) {
        return RENDERER.SetRotation(entry, rotation);
    }
    
    /// Sets the vertex array of a draw list entry.
    inline void SetDrawListVertexArray (DrawListEntryHandle entry, uint32_t vertex_array_handle) {
        return RENDERER.SetDrawListVertexArray(entry, vertex_array_handle);
    }
    
    /// Sets the element array of a draw list entry.
    inline void SetDrawListElements (DrawListEntryHandle entry, uint32_t element_offset, uint32_t element_length) {
        return RENDERER.SetDrawListElements(entry, element_offset, element_length);
    }
    
    /// Sets the shader of a draw list entry.
    inline void SetDrawListShader (DrawListEntryHandle entry, Model::VertexFormat vertex_format, Material::Type material_type) {
        return RENDERER.SetDrawListShader(entry, vertex_format, material_type);
    }
    
    /// Sets the textures of a draw list entry.
    /// @param textures         Array of texture handles.
    /// @param texture_count    Length of the 'textures' array. Valid lengths are 0 to 16.
    inline void SetDrawListTextures (DrawListEntryHandle entry, size_t texture_count, uint32_t* textures) {
        return RENDERER.SetDrawListTextures(entry, texture_count, textures);
    }
    
    /// Removes an entry from the draw list.
    inline void RemoveDrawListEntry (DrawListEntryHandle entry) {
        return RENDERER.RemoveDrawListEntry(entry);
    }
    
    /// Creates a texture.
    inline uint32_t CreateTexture (ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data) {
        return RENDERER.CreateTexture(color_mode, texture_filter, width, height, data);
    }
    
    /// Creates an indexed vertex array.
    inline void CreateIndexedVertexArray (const VertexDefinition& vertex_format, vertexhandle_t& vertex_buffer_handle, vertexhandle_t& element_buffer_handle,  vertexhandle_t& vertex_array_handle, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data) {
        return RENDERER.CreateIndexedVertexArray(vertex_format, vertex_buffer_handle, element_buffer_handle, vertex_array_handle, vertex_size, vertex_data, index_size, index_data);
    }
    
    /// Creates a regular vertex array.
    inline void CreateVertexArray (const VertexDefinition& vertex_format, vertexhandle_t& vertex_buffer_handle,  vertexhandle_t& vertex_array_handle) {
        return RENDERER.CreateVertexArray(vertex_format, vertex_buffer_handle, vertex_array_handle);
    }
    
    /// Changes the contents of a regular vertex array.
    inline void UpdateVertexArray (vertexhandle_t vertex_buffer_handle, size_t data_size, void* data) {
        return RENDERER.UpdateVertexArray(vertex_buffer_handle, data_size, data);
    }
    
    /// Renders all of the entries in the draw list.
    inline void RenderFrame () {
        return RENDERER.RenderFrame();
    }
    
    /// Sets the rendering image size.
    inline void SetScreenSize (float width, float height) {
        return RENDERER.SetScreenSize(width, height);
    }
}

#endif // RENDER_RENDERER_H