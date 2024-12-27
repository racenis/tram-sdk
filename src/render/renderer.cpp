// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/renderer.h>

#include <render/vertices.h>

#include <framework/logging.h>

namespace tram::Render {

const static VertexAttribute vertex_static_definition[5] = {
    {VertexAttribute::FLOAT32, VertexAttribute::FFP_POSITION,    3, sizeof(StaticModelVertex), offsetof(StaticModelVertex, co)},
    {VertexAttribute::FLOAT32, VertexAttribute::FFP_NORMAL,      3, sizeof(StaticModelVertex), offsetof(StaticModelVertex, normal)},
    {VertexAttribute::FLOAT32, VertexAttribute::FFP_TEXTURE,     2, sizeof(StaticModelVertex), offsetof(StaticModelVertex, tex)},
    {VertexAttribute::FLOAT32, VertexAttribute::FFP_LIGHTMAP,    2, sizeof(StaticModelVertex), offsetof(StaticModelVertex, lighttex)},
    {VertexAttribute::UINT32,  VertexAttribute::FFP_IGNORE,      1, sizeof(StaticModelVertex), offsetof(StaticModelVertex, texture)}
};

const static VertexAttribute vertex_dynamic_definition[6] = {
    {VertexAttribute::FLOAT32, VertexAttribute::FFP_POSITION,    3, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, co)},
    {VertexAttribute::FLOAT32, VertexAttribute::FFP_NORMAL,      3, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, normal)},
    {VertexAttribute::FLOAT32, VertexAttribute::FFP_TEXTURE,     2, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, tex)},
    {VertexAttribute::UINT32,  VertexAttribute::FFP_BONE_INDEX,  4, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, bone)},
    {VertexAttribute::FLOAT32, VertexAttribute::FFP_BONE_WEIGHT, 4, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, boneweight)},
    {VertexAttribute::UINT32,  VertexAttribute::FFP_IGNORE,      1, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, texture)}
};

const static VertexAttribute vertex_sprite_definition[6] = {
    {VertexAttribute::FLOAT32,  VertexAttribute::FFP_IGNORE,     3, sizeof(SpriteVertex), 0},
    {VertexAttribute::FLOAT32,  VertexAttribute::FFP_IGNORE,     2, sizeof(SpriteVertex), offsetof(SpriteVertex, texco)},
    {VertexAttribute::FLOAT32,  VertexAttribute::FFP_IGNORE,     3, sizeof(SpriteVertex), offsetof(SpriteVertex, color)},
    {VertexAttribute::FLOAT32,  VertexAttribute::FFP_IGNORE,     2, sizeof(SpriteVertex), offsetof(SpriteVertex, voffset)},
    {VertexAttribute::FLOAT32,  VertexAttribute::FFP_IGNORE,     1, sizeof(SpriteVertex), offsetof(SpriteVertex, verticality)},
    {VertexAttribute::UINT32,   VertexAttribute::FFP_IGNORE,     1, sizeof(SpriteVertex), offsetof(SpriteVertex, texture)}
};

const static VertexAttribute vertex_line_definition[2] = {
    {VertexAttribute::FLOAT32,  VertexAttribute::FFP_POSITION,   3, sizeof(LineVertex), 0},
    {VertexAttribute::FLOAT32,  VertexAttribute::FFP_COLOR,      3, sizeof(LineVertex), offsetof(LineVertex, color)}
};
    
static const uint32_t MAX_VERTEX_TYPES = 10; // TODO: put this in config?

struct RegisteredVertexDefinition {
    bool registered = false;
    VertexDefinition definition;
};

// here's the 4 default vertices, all defined up
static RegisteredVertexDefinition all_vertex_definitions[MAX_VERTEX_TYPES] = {
    {true, {vertex_static_definition,   sizeof(vertex_static_definition)    / sizeof(VertexAttribute)}},
    {true, {vertex_dynamic_definition,  sizeof(vertex_dynamic_definition)   / sizeof(VertexAttribute)}},
    {true, {vertex_sprite_definition,   sizeof(vertex_sprite_definition)    / sizeof(VertexAttribute)}},
    {true, {vertex_line_definition,     sizeof(vertex_line_definition)      / sizeof(VertexAttribute)}}
};

/// Registers a VertexDefinition for a specified vertex format.
/// @note For the VertexDefinition's 'attributes' only the pointer will be copied,
///       the underlying memory will not be copied -- make sure that it doesn't
///       point to stack memory, or other memory that could be overwritten.
void RegisterVertexDefinition (vertexformat_t format, VertexDefinition definition) {
    assert(format < MAX_VERTEX_TYPES);
    
    if (all_vertex_definitions[format].registered) {
        Log(SEVERITY_WARNING, System::SYSTEM_RENDER, "VertexDefinition for format {} already registered, overwriting.", format);
        // TODO: replace raw format int with a name
    }
    
    // TODO: we should validate the FFP here (there should not be more than one of each type
    
    all_vertex_definitions[format].definition = definition;
    all_vertex_definitions[format].registered = true;
}

/// Returns a previously registered VertexDefinition.
VertexDefinition GetVertexDefinition(vertexformat_t format) {
    assert(format < MAX_VERTEX_TYPES);
    
    if (!all_vertex_definitions[format].registered) {
        Log(SEVERITY_ERROR, System::SYSTEM_RENDER, "VertexDefinition for format {} not registered!", format);
        // TODO: replace raw format int with a name
        
        format = 0; // we do need to return something.
    }
    
    return all_vertex_definitions[format].definition;
}

}