// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_RENDER_RENDERER_H
#define TRAM_SDK_RENDER_RENDERER_H

#include <render/render.h>

namespace tram::Render {

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
    enum {
        FLOAT32,
        INT32,
        UINT32
    } type;
    uint32_t size;      //< Size of the attribute, in bytes.
    uint32_t stride;    //< Distance between same attribute in different vertices, in bytes.
    uint64_t offset;    //< Offset of beginning of attribute in vertex buffer.
};

struct VertexDefinition {
    const VertexAttribute* attributes;
    size_t attribute_count;
};

void RegisterVertexDefinition (vertexformat_t format, VertexDefinition definition);
VertexDefinition GetVertexDefinition(vertexformat_t format);


}

#endif // TRAM_SDK_RENDER_RENDERER_H