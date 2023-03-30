// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_VERTICES_H
#define RENDER_VERTICES_H

#include <render/render.h>

namespace tram::Render {

/// Atributes for vertex buffer vertices.
struct VertexAttribute {
    enum {
        FLOAT32,
        INT32,
        UINT32
    } type;
    uint32_t size;      //< Size of the attribute, in bytes.
    uint32_t stride;    //< Distance between same attribute in different vertices, in bytes.
    uint64_t offset;    //< Offset of beginning of attribute in vertex buffer.
};

typedef std::vector<VertexAttribute> VertexDefinition; // TODO: rename to VertexFormatDefinition .. or get rid of it altogether

void RegisterVertexDefinition (vertexformat_t format, VertexDefinition defintion);

template <typename T> VertexDefinition VERTEX_DEFINITION;

struct StaticModelVertex {
    vec3 co;            //< Vertex position in object space.
    vec3 normal;        //< Vertex normal vector.
    vec2 tex;           //< Vertex texture coordinates.
    vec2 lighttex;      //< Vertex lightmap coordinates.
    uint32_t texture;   //< Vertex texture material index.
};

template <> inline VertexDefinition VERTEX_DEFINITION <StaticModelVertex> = {
    {VertexAttribute::FLOAT32, 3, sizeof(StaticModelVertex), offsetof(StaticModelVertex, co)},
    {VertexAttribute::FLOAT32, 3, sizeof(StaticModelVertex), offsetof(StaticModelVertex, normal)},
    {VertexAttribute::FLOAT32, 2, sizeof(StaticModelVertex), offsetof(StaticModelVertex, tex)},
    {VertexAttribute::FLOAT32, 2, sizeof(StaticModelVertex), offsetof(StaticModelVertex, lighttex)},
    {VertexAttribute::UINT32,  1, sizeof(StaticModelVertex), offsetof(StaticModelVertex, texture)}
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
    {VertexAttribute::FLOAT32, 3, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, co)},
    {VertexAttribute::FLOAT32, 3, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, normal)},
    {VertexAttribute::FLOAT32, 2, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, tex)},
    {VertexAttribute::UINT32,  4, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, bone)},
    {VertexAttribute::FLOAT32, 4, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, boneweight)},
    {VertexAttribute::UINT32,  1, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, texture)}
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
    {VertexAttribute::FLOAT32, 3, sizeof(SpriteVertex), 0},
    {VertexAttribute::FLOAT32, 2, sizeof(SpriteVertex), offsetof(SpriteVertex, texco)},
    {VertexAttribute::FLOAT32, 3, sizeof(SpriteVertex), offsetof(SpriteVertex, color)},
    
    {VertexAttribute::FLOAT32, 2, sizeof(SpriteVertex), offsetof(SpriteVertex, voffset)},
    {VertexAttribute::FLOAT32, 1, sizeof(SpriteVertex), offsetof(SpriteVertex, verticality)},
    
    {VertexAttribute::UINT32,  1, sizeof(SpriteVertex), offsetof(SpriteVertex, texture)}
};

struct LineVertex {
    vec3 co;            //< Vertex position in object space.
    vec3 color;         //< Vertex color.
};

template <> inline VertexDefinition VERTEX_DEFINITION <LineVertex> = {
    {VertexAttribute::FLOAT32, 3, sizeof(LineVertex), 0},
    {VertexAttribute::FLOAT32, 3, sizeof(LineVertex), offsetof(LineVertex, color)}
};

struct Triangle {
    ivec3 indices;          //< Triangle vertex indices.
};

struct ModelData {};

struct StaticModelData : public ModelData {
    std::vector<StaticModelVertex> vertices;
    std::vector<Triangle> indices;
};

struct DynamicModelData : public ModelData {
    std::vector<DynamicModelVertex> vertices;
    std::vector<Triangle> indices;
    std::vector<UID> groups;
};

extern std::vector<LineVertex> colorlines;
extern std::vector<SpriteVertex> textvertices;
extern std::vector<SpriteVertex> glyphvertices;

}

#endif // RENDER_VERTICES_H