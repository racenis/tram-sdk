// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_RENDERER_H
#define RENDER_RENDERER_H

#include <templates/octree.h>

#include <render/render.h>
#include <render/model.h>

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
    
    
    
    
    struct StaticModelVertex {
        vec3 co;
        vec3 normal;
        vec2 tex;
        vec2 lighttex;
        uint32_t texture;
    };
    
    static const VertexDefinition STATIC_MODEL_VERTEX_DEFINITION = {
        {VertexProperty::POSITION_VECTOR,       VertexProperty::FLOAT32, 3, sizeof(StaticModelVertex), offsetof(StaticModelVertex, co)},
        {VertexProperty::NORMAL_VECTOR,         VertexProperty::FLOAT32, 3, sizeof(StaticModelVertex), offsetof(StaticModelVertex, normal)},
        {VertexProperty::TEXTURE_COORDINATE,    VertexProperty::FLOAT32, 2, sizeof(StaticModelVertex), offsetof(StaticModelVertex, tex)},
        {VertexProperty::LIGHTMAP_COORDINATE,   VertexProperty::FLOAT32, 2, sizeof(StaticModelVertex), offsetof(StaticModelVertex, lighttex)},
        {VertexProperty::TEXTURE_INDEX,         VertexProperty::UINT32,  1, sizeof(StaticModelVertex), offsetof(StaticModelVertex, texture)}
    };

    struct DynamicModelVertex {
        vec3 co;
        vec3 normal;
        vec2 tex;
        ivec4 bone;
        vec4 boneweight;
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
        vec3 co;
        vec2 voffset;
        vec2 texco;
        vec3 color;
        float verticality;
        uint32_t texture;
    };

    static const VertexDefinition SPRITE_VERTEX_DEFINITION = {
        {VertexProperty::POSITION_VECTOR,       VertexProperty::FLOAT32, 3, sizeof(SpriteVertex), 0},
        {VertexProperty::TEXTURE_COORDINATE,    VertexProperty::FLOAT32, 2, sizeof(SpriteVertex), offsetof(SpriteVertex, texco)},
        {VertexProperty::COLOR_VECTOR,          VertexProperty::FLOAT32, 3, sizeof(SpriteVertex), offsetof(SpriteVertex, color)},
        
        {VertexProperty::OTHER,                 VertexProperty::FLOAT32, 2, sizeof(SpriteVertex), offsetof(SpriteVertex, voffset)},
        {VertexProperty::OTHER,                 VertexProperty::FLOAT32, 1, sizeof(SpriteVertex), offsetof(SpriteVertex, verticality)},
        
        {VertexProperty::TEXTURE_INDEX,         VertexProperty::UINT32,  1, sizeof(SpriteVertex), offsetof(SpriteVertex, texture)}
    };
    
    struct LineVertex {
        vec3 co;
        vec3 color;
    };
    
    static const VertexDefinition LINE_VERTEX_DEFINITION = {
        {VertexProperty::POSITION_VECTOR,       VertexProperty::FLOAT32, 3, sizeof(LineVertex), 0},
        {VertexProperty::COLOR_VECTOR,          VertexProperty::FLOAT32, 3, sizeof(LineVertex), offsetof(LineVertex, color)}
    };

    struct ModelIndex {
        ivec3 tri;
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

    extern std::vector<LineVertex> colorlines;
    extern std::vector<SpriteVertex> textvertices;
    extern std::vector<SpriteVertex> glyphvertices;
    

    extern void (*SetScreenSize) (float width, float height);

    extern DrawListEntryHandle (*InsertDrawListEntry) ();
    extern DrawListEntryHandle (*InsertDrawListEntryFromModel) (Model* model);
    
    extern uint32_t (*GetFlags) (DrawListEntryHandle entry);
    extern void (*SetFlags) (DrawListEntryHandle entry, uint32_t flags);
    extern void (*SetPose) (DrawListEntryHandle entry, Pose* pose);
    extern void (*SetLightmap) (DrawListEntryHandle entry, uint32_t lightmap);
    extern void (*SetLights) (DrawListEntryHandle entry, uint32_t* lights);
    extern void (*SetLocation) (DrawListEntryHandle entry, glm::vec3& location);
    extern void (*SetRotation) (DrawListEntryHandle entry, glm::quat& rotation);
    extern void (*SetDrawListVertexArray) (DrawListEntryHandle entry, uint32_t vertex_array_handle);
    extern void (*SetDrawListElements) (DrawListEntryHandle entry, uint32_t element_offset, uint32_t element_length);
    extern void (*SetDrawListShader) (DrawListEntryHandle entry, Model::VertexFormat vertex_format, Material::Type material_type);
    extern void (*SetDrawListTextures) (DrawListEntryHandle entry, size_t texture_count, uint32_t* textures);
    
    extern void (*RemoveDrawListEntry) (DrawListEntryHandle entry);
    
    extern uint32_t (*CreateTexture) (ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data);
    
    extern void (*CreateIndexedVertexArray) (const VertexDefinition& vertex_format, vertexhandle_t& vertex_buffer_handle, vertexhandle_t& element_buffer_handle,  vertexhandle_t& vertex_array_handle, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data);
    extern void (*CreateVertexArray) (const VertexDefinition& vertex_format, vertexhandle_t& vertex_buffer_handle,  vertexhandle_t& vertex_array_handle);
    extern void (*UpdateVertexArray) (vertexhandle_t vertex_buffer_handle, size_t data_size, void* data);
    
    extern void (*RenderFrame) ();
}

#endif // RENDER_RENDERER_H