// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// RENDERER.H -- Renderer
// Functions that all renderer backends need to implement.

#ifndef RENDERER_H
#define RENDERER_H

#include <render/render.h>

namespace Core::Render {
    extern Pool<RenderListObject> renderList;
    extern Pool<LightListObject> lightPool;
    extern Octree<uint32_t> lightTree;

    extern std::vector<LineVertex> colorlines;
    extern std::vector<SpriteVertex> textvertices;
    extern std::vector<SpriteVertex> glyphvertices;
    
    class GeometryBatch {
    public:
        Material::Type material_type;
        Model::VertexFormat vertex_format;
        
        uint32_t vertex_array;
        uint32_t vertex_buffer;
        
        RenderListObject* render_object = nullptr;
        Material* material_list[16] = {nullptr};
        union {
            std::vector<SpriteVertex>* sprite_vector = nullptr;
            std::vector<LineVertex>* line_vector;
        };
        
        static void Find(uint32_t& index, GeometryBatch*& ptr, Material* mat, Material::Type mat_type, Model::VertexFormat vert_fmt);
        static void Remove(uint32_t index, GeometryBatch* ptr);
        static void Update();
    protected:
        static std::vector<GeometryBatch> geometry_batches;
    };
    
    enum ColorMode {
        COLORMODE_R,
        COLORMODE_RG,
        COLORMODE_RGB,
        COLORMODE_RGBA
    };
    
    enum TextureFilter {
        TEXTUREFILTER_LINEAR,
        TEXTUREFILTER_LINEAR_MIPMAPPED
    };
}

namespace Core::Render::OpenGL {
    void Init();

    void Render();
    
    void ScreenSize(float width, float height);
    
    void CompileShaders();
    
    uint32_t FindShader(Model::VertexFormat format, Material::Type type);
    
    void BindUniformBlock (const char* name, uint32_t binding);
    
    
    
    DrawListEntryHandle InsertDrawListEntry();
    DrawListEntryHandle InsertDrawListEntry(Model* model);
    
    uint32_t GetFlags(DrawListEntryHandle entry);
    void SetFlags(DrawListEntryHandle entry, uint32_t flags);
    void SetPose(DrawListEntryHandle entry, PoseListObject* pose);
    void SetLightmap(DrawListEntryHandle entry, uint32_t lightmap);
    void SetLights(DrawListEntryHandle entry, uint32_t* lights);
    void SetLocation(DrawListEntryHandle entry, glm::vec3& location);
    void SetRotation(DrawListEntryHandle entry, glm::quat& rotation);
    void SetDrawListVertexArray(DrawListEntryHandle entry, uint32_t vertex_array_handle);
    void SetDrawListElements(DrawListEntryHandle entry, uint32_t element_offset, uint32_t element_length);
    void SetDrawListShader(DrawListEntryHandle entry, Model::VertexFormat vertex_format, Material::Type material_type);
    void SetDrawListTextures(DrawListEntryHandle entry, size_t texture_count, uint32_t* textures);
    
    void RemoveDrawListEntry(DrawListEntryHandle entry);
    
    uint32_t CreateTexture(ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data);
    
    void CreateIndexedVertexArray(const VertexDefinition& vertex_format, uint32_t& vertex_buffer_handle, uint32_t& element_buffer_handle,  uint32_t& vertex_array_handle, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data);
    void CreateVertexArray(const VertexDefinition& vertex_format, uint32_t& vertex_buffer_handle,  uint32_t& vertex_array_handle);
    void UpdateVertexArray(uint32_t vertex_buffer_handle, size_t data_size, void* data);
    
}

#endif //RENDERER_H