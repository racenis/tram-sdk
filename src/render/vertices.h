// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_VERTICES_H
#define RENDER_VERTICES_H

#include <render/renderer.h>

namespace tram::Render {
    template <typename T> VertexDefinition VERTEX_DEFINITION;
    
    struct StaticModelVertex {
        vec3 co;            //< Vertex position in object space.
        vec3 normal;        //< Vertex normal vector.
        vec2 tex;           //< Vertex texture coordinates.
        vec2 lighttex;      //< Vertex lightmap coordinates.
        uint32_t texture;   //< Vertex texture material index.
    };
    
    template <> inline VertexDefinition VERTEX_DEFINITION <StaticModelVertex> = {
        {VertexAttribute::POSITION_VECTOR,       VertexAttribute::FLOAT32, 3, sizeof(StaticModelVertex), offsetof(StaticModelVertex, co)},
        {VertexAttribute::NORMAL_VECTOR,         VertexAttribute::FLOAT32, 3, sizeof(StaticModelVertex), offsetof(StaticModelVertex, normal)},
        {VertexAttribute::TEXTURE_COORDINATE,    VertexAttribute::FLOAT32, 2, sizeof(StaticModelVertex), offsetof(StaticModelVertex, tex)},
        {VertexAttribute::LIGHTMAP_COORDINATE,   VertexAttribute::FLOAT32, 2, sizeof(StaticModelVertex), offsetof(StaticModelVertex, lighttex)},
        {VertexAttribute::TEXTURE_INDEX,         VertexAttribute::UINT32,  1, sizeof(StaticModelVertex), offsetof(StaticModelVertex, texture)}
    };

    struct DynamicModelVertex {
        vec3 co;            //< Vertex position in object space.
        vec3 normal;        //< Vertex normal vector.
        vec2 tex;           //< Vertex texture coordinates.
        ivec4 bone;         //< Vertex bone indices.
        vec4 boneweight;    //< Vertex bone weights.
        uint32_t texture;   //< Vertex texture material index.
    };
    
    template <> inline VertexDefinition VERTEX_DEFINITION <DynamicModelVertex> = {
        {VertexAttribute::POSITION_VECTOR,       VertexAttribute::FLOAT32, 3, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, co)},
        {VertexAttribute::NORMAL_VECTOR,         VertexAttribute::FLOAT32, 3, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, normal)},
        {VertexAttribute::TEXTURE_COORDINATE,    VertexAttribute::FLOAT32, 2, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, tex)},
        {VertexAttribute::BONE_INDEX,            VertexAttribute::UINT32,  4, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, bone)},
        {VertexAttribute::BONE_WEIGHT,           VertexAttribute::FLOAT32, 4, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, boneweight)},
        {VertexAttribute::TEXTURE_INDEX,         VertexAttribute::UINT32,  1, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, texture)}
    };
    
    struct SpriteVertex {
        vec3 co;            //< Sprite position in object space.
        vec2 voffset;       //< Vertex offset in projection space.
        vec2 texco;         //< Vertex texture coordinates.
        vec3 color;         //< Vertex color.
        float verticality;  //< I don't remember.
        uint32_t texture;   //< Vertex texture material index.
    };

    template <> inline VertexDefinition VERTEX_DEFINITION <SpriteVertex> = {
        {VertexAttribute::POSITION_VECTOR,       VertexAttribute::FLOAT32, 3, sizeof(SpriteVertex), 0},
        {VertexAttribute::TEXTURE_COORDINATE,    VertexAttribute::FLOAT32, 2, sizeof(SpriteVertex), offsetof(SpriteVertex, texco)},
        {VertexAttribute::COLOR_VECTOR,          VertexAttribute::FLOAT32, 3, sizeof(SpriteVertex), offsetof(SpriteVertex, color)},
        
        {VertexAttribute::OTHER,                 VertexAttribute::FLOAT32, 2, sizeof(SpriteVertex), offsetof(SpriteVertex, voffset)},
        {VertexAttribute::OTHER,                 VertexAttribute::FLOAT32, 1, sizeof(SpriteVertex), offsetof(SpriteVertex, verticality)},
        
        {VertexAttribute::TEXTURE_INDEX,         VertexAttribute::UINT32,  1, sizeof(SpriteVertex), offsetof(SpriteVertex, texture)}
    };
    
    struct LineVertex {
        vec3 co;            //< Vertex position in object space.
        vec3 color;         //< Vertex color.
    };
    
    template <> inline VertexDefinition VERTEX_DEFINITION <LineVertex> = {
        {VertexAttribute::POSITION_VECTOR,       VertexAttribute::FLOAT32, 3, sizeof(LineVertex), 0},
        {VertexAttribute::COLOR_VECTOR,          VertexAttribute::FLOAT32, 3, sizeof(LineVertex), offsetof(LineVertex, color)}
    };

    struct ModelIndex {
        ivec3 tri;          //< Triangle vertex indices.
    };
    
    struct StaticModelData : public ModelData {
        std::vector<StaticModelVertex> vertices;
        std::vector<ModelIndex> indices;
    };

    struct DynamicModelData : public ModelData {
        std::vector<DynamicModelVertex> vertices;
        std::vector<ModelIndex> indices;
        std::vector<UID> groups;
    };
    
    extern std::vector<LineVertex> colorlines;
    extern std::vector<SpriteVertex> textvertices;
    extern std::vector<SpriteVertex> glyphvertices;

}

#endif // RENDER_VERTICES_H