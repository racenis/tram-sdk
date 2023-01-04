// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// RENDERER.H -- Abstract rendering interface
// Rendering interface on the level of draw call, vertex lists and textures.

#ifndef RENDERER_H
#define RENDERER_H

#include <templates/octree.h>

#include <render/render.h>

namespace Core::Render {
    enum renderflags: uint32_t {
        FLAG_NO_RENDER = 0,
        FLAG_RENDER = 1,

        FLAG_DRAW_INDEXED = 4,
        FLAG_REVERSE_SORT = 8,
        FLAG_TRANSPARENT = 16,
        FLAG_NO_DEPTH_TEST = 32,
        FLAG_INTERIOR_LIGHTING = 64,
        FLAG_DRAW_LINES = 128
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
    
    struct VertexProperty {
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
        uint32_t size;
        uint32_t stride;
        uint64_t offset;
    };
    
    typedef std::vector<VertexProperty> VertexDefinition;
    
    
    
    
    struct StaticModelVertex{
        glm::vec3 co;
        glm::vec3 normal;
        glm::vec2 tex;
        glm::vec2 lighttex;
        uint32_t texture;
    };
    
    static const VertexDefinition STATIC_MODEL_VERTEX_DEFINITION = {
        {VertexProperty::POSITION_VECTOR,       VertexProperty::FLOAT32, 3, sizeof(StaticModelVertex), offsetof(StaticModelVertex, co)},
        {VertexProperty::NORMAL_VECTOR,         VertexProperty::FLOAT32, 3, sizeof(StaticModelVertex), offsetof(StaticModelVertex, normal)},
        {VertexProperty::TEXTURE_COORDINATE,    VertexProperty::FLOAT32, 2, sizeof(StaticModelVertex), offsetof(StaticModelVertex, tex)},
        {VertexProperty::LIGHTMAP_COORDINATE,   VertexProperty::FLOAT32, 2, sizeof(StaticModelVertex), offsetof(StaticModelVertex, lighttex)},
        {VertexProperty::TEXTURE_INDEX,         VertexProperty::UINT32,  1, sizeof(StaticModelVertex), offsetof(StaticModelVertex, texture)}
    };

    struct DynamicModelVertex{
        glm::vec3 co;
        glm::vec3 normal;
        glm::vec2 tex;
        glm::ivec4 bone;
        glm::vec4 boneweight;
        uint32_t texture;
    };
    
    static const VertexDefinition DYNAMIC_MODEL_VERTEX_DEFINITION = {
        {VertexProperty::POSITION_VECTOR,       VertexProperty::FLOAT32, 3, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, co)},
        {VertexProperty::NORMAL_VECTOR,         VertexProperty::FLOAT32, 3, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, normal)},
        {VertexProperty::TEXTURE_COORDINATE,    VertexProperty::FLOAT32, 2, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, tex)},
        {VertexProperty::BONE_INDEX,            VertexProperty::UINT32,  4, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, bone)},
        {VertexProperty::BONE_WEIGHT,           VertexProperty::FLOAT32, 4, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, boneweight)},
        {VertexProperty::TEXTURE_INDEX,         VertexProperty::UINT32,  1, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, texture)}
    };
    
    struct SpriteVertex {
        glm::vec3 co;
        glm::vec2 voffset;
        glm::vec2 texco;
        glm::vec3 color;
        float verticality;
        uint32_t texture;
    };

    static const VertexDefinition SPRITE_VERTEX_DEFINITION = {
        {VertexProperty::POSITION_VECTOR,       VertexProperty::FLOAT32, 2, sizeof(SpriteVertex), 0},
        {VertexProperty::TEXTURE_COORDINATE,    VertexProperty::FLOAT32, 2, sizeof(SpriteVertex), offsetof(SpriteVertex, texco)},
        {VertexProperty::COLOR_VECTOR,          VertexProperty::FLOAT32, 3, sizeof(SpriteVertex), offsetof(SpriteVertex, color)},
        
        {VertexProperty::OTHER,                 VertexProperty::FLOAT32, 2, sizeof(SpriteVertex), offsetof(SpriteVertex, voffset)},
        {VertexProperty::OTHER,                 VertexProperty::FLOAT32, 1, sizeof(SpriteVertex), offsetof(SpriteVertex, verticality)},
        
        {VertexProperty::TEXTURE_INDEX,         VertexProperty::UINT32,  1, sizeof(SpriteVertex), offsetof(SpriteVertex, texture)}
    };
    
    struct LineVertex{
        glm::vec3 co;
        glm::vec3 color;
    };
    
    static const VertexDefinition LINE_VERTEX_DEFINITION = {
        {VertexProperty::POSITION_VECTOR,       VertexProperty::FLOAT32, 3, sizeof(LineVertex), 0},
        {VertexProperty::COLOR_VECTOR,          VertexProperty::FLOAT32, 3, sizeof(LineVertex), offsetof(LineVertex, color)}
    };

    struct ModelIndex{
        glm::ivec3 tri;
    };
    
    struct StaticModelData : public ModelData {
        std::vector<StaticModelVertex> vertices;
        std::vector<ModelIndex> indices;
    };

    struct DynamicModelData : public ModelData {
        std::vector<DynamicModelVertex> vertices;
        std::vector<ModelIndex> indices;
        std::vector<unsigned int> groups;
    };
    
    struct LightListObject {
        glm::vec3 location = {0.0f, 0.0f, 0.0f};
        float padding;
        glm::vec3 color = {0.0f, 0.0f, 0.0f};
        float distance = 0.0f;
        float padding2[8];
    };

    extern Pool<LightListObject> lightPool;
    extern Octree<uint32_t> lightTree;
    
    extern Pool<LightListObject> lightPool;
    extern Octree<uint32_t> lightTree;

    extern std::vector<LineVertex> colorlines;
    extern std::vector<SpriteVertex> textvertices;
    extern std::vector<SpriteVertex> glyphvertices;
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
    void SetPose(DrawListEntryHandle entry, Pose* pose);
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
    
    void CreateIndexedVertexArray(const VertexDefinition& vertex_format, vertexhandle_t& vertex_buffer_handle, vertexhandle_t& element_buffer_handle,  vertexhandle_t& vertex_array_handle, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data);
    void CreateVertexArray(const VertexDefinition& vertex_format, vertexhandle_t& vertex_buffer_handle,  vertexhandle_t& vertex_array_handle);
    void UpdateVertexArray(vertexhandle_t vertex_buffer_handle, size_t data_size, void* data);
    
    struct DrawListEntry {
        uint32_t flags = 0;

        Pose* pose = nullptr;

        glm::vec3 location = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::quat rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

        uint32_t lights[4] = {0};
        
        uint32_t layer = 0;
        uint32_t lightmap = 0;
        uint32_t vao = 0;
        uint32_t eboLen = 0;
        uint32_t eboOff = 0;
        uint32_t shader = 0;
        uint32_t texCount = 0;
        uint32_t textures[15] = {0};

        /// Copies the element and vertex array names, offsets, etc. from the Model.
        //void FillFromModel(Model* mdl, uint32_t eboIndex);

        /// Assembles a key for sorting
        uint64_t CalcSortKey(const glm::vec3& cameraPosition){
            uint64_t sortkey = 0;
            sortkey = flags & FLAG_TRANSPARENT ? 1 : 0;
            sortkey = sortkey << 60;
            sortkey = sortkey | (((uint64_t)layer) << 61);    // 3 bits for the layer number
            sortkey = sortkey | (((uint64_t)shader) << 48);   // 12 bits for the shader
            sortkey = sortkey | (((uint64_t)vao) << 32);      // 16 bits or the vertex array number
            // TODO: reverse the distance if FLAG_REVERSE_SORT is set
            // also i think that the bitmask for the distance thing is incorrect
            sortkey = sortkey | (((uint64_t)(glm::distance(cameraPosition, location) * 3000000.0f)) & 0x00000000FFFFFFFF); // 32 bits for the distance
            return sortkey;
        }
    };
}

#endif //RENDERER_H