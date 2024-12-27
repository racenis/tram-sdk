// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_VERTICES_H
#define TRAM_SDK_RENDER_VERTICES_H

#include <render/renderer.h>

#include <render/render.h>

namespace tram::Render {

struct StaticModelVertex {
    vec3 co;            //< Vertex position in object space.
    vec3 normal;        //< Vertex normal vector.
    vec2 tex;           //< Vertex texture coordinates.
    vec2 lighttex;      //< Vertex lightmap coordinates.
    uint32_t texture;   //< Vertex texture material index.
};

struct DynamicModelVertex {
    vec3 co;            //< Vertex position in object space.
    vec3 normal;        //< Vertex normal vector.
    vec2 tex;           //< Vertex texture coordinates.
    ivec4 bone;         //< Vertex bone indices.
    vec4 boneweight;    //< Vertex bone weights.
    uint32_t texture;   //< Vertex texture material index.
};

struct SpriteVertex {
    vec3 co;            //< Sprite position in object space.
    vec2 voffset;       //< Vertex offset in projection space.
    vec2 texco;         //< Vertex texture coordinates.
    vec3 color;         //< Vertex color.
    float verticality;  //< I don't remember.
    uint32_t texture;   //< Vertex texture material index.
};

struct LineVertex {
    vec3 co;            //< Vertex position in object space.
    vec3 color;         //< Vertex color.
};

struct Triangle {
    ivec3 indices;          //< Triangle vertex indices.
};

struct SpritePoint {
    vec3 position;
    vec3 center;
    vec3 color;
    float rotation;
    vec2 dimensions;
    vec2 texture_offset;
    vec2 texture_size;
    uint32_t texture;
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

}

#endif // TRAM_SDK_RENDER_VERTICES_H