// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/api.h>
#include <components/mesh.h>

#include <render/scene.h>
#include <render/light.h>

#include <config.h>

/**
 * @class tram::MeshComponent components/mesh.h <components/mesh.h>
 * 
 * Renders a user-specified mesh.
 * @see https://racenis.github.io/tram-sdk/documentation/components/mesh.html
 */

namespace tram {

using namespace tram::Render;
using namespace tram::Render::API;

template <> Pool<MeshComponent> PoolProxy<MeshComponent>::pool("MeshComponent pool", COMPONENT_LIMIT_RENDER);
template <> void Component<MeshComponent>::init() { ptr = PoolProxy<MeshComponent>::New(); }
template <> void Component<MeshComponent>::yeet() { PoolProxy<MeshComponent>::Delete(ptr); }

MeshVertex::MeshVertex(Render::vertexformat_t format) {
    assert(GetVertexFormatName(format) && "Is valid format");
    
    this->format = format;
    
    VertexDefinition definition = GetVertexDefinition(format);
    attributes = new value_t[definition.attribute_count];
}

MeshVertex::~MeshVertex() {
    delete[] attributes;
    attributes = nullptr;
}

int GetFFPPosition(Render::vertexformat_t format, decltype(Render::VertexAttribute::ffp_type) ffp) {
    VertexDefinition definition = GetVertexDefinition(format);
    for (size_t i = 0; i < definition.attribute_count; i++) {
        if (definition.attributes[i].ffp_type == ffp) return i;
    }
    assert(false && "Should not happen");
    return -1;
}

void MeshVertex::SetPosition(vec3 position) {
    attributes[GetFFPPosition(format, Render::VertexAttribute::FFP_POSITION)] = position;
}

void MeshVertex::SetNormal(vec3 normal) {
    attributes[GetFFPPosition(format, Render::VertexAttribute::FFP_NORMAL)] = normal;
}

void MeshVertex::SetColor(vec3 color) {
    attributes[GetFFPPosition(format, Render::VertexAttribute::FFP_COLOR)] = color;
}

void MeshVertex::SetTextureUV(vec2 uv) {
    attributes[GetFFPPosition(format, Render::VertexAttribute::FFP_TEXTURE)] = uv;
}

void MeshVertex::SetLightmapUV(vec2 uv) {
    attributes[GetFFPPosition(format, Render::VertexAttribute::FFP_LIGHTMAP)] = uv;
}

/// @warning Due to value_t not supporting ivec and uvec types, setting a value
///          for these attributes currently is not possible.
void MeshVertex::SetAttribute(size_t index, value_t value) {
    VertexDefinition definition = GetVertexDefinition(format);
    assert(index < definition.attribute_count);
    
    switch (definition.attributes[index].type) {
        case Render::VertexAttribute::FLOAT32: switch (definition.attributes[index].size) {
            case 1: assert(value.GetType() == TYPE_FLOAT32); break;
            case 2: assert(value.GetType() == TYPE_VEC2); break;
            case 3: assert(value.GetType() == TYPE_VEC3); break;
            case 4: assert(value.GetType() == TYPE_VEC4); break;
            default: assert(false && "Should not happen.");
        } break;
        case Render::VertexAttribute::INT32:
        case Render::VertexAttribute::UINT32:
            assert(value.IsInt());
    }
    
    attributes[index] = value;
}

std::pair<Render::vertexformat_t, const value_t*> MeshVertex::GetData() const {
    return {format, attributes};
}

MeshComponent::MeshComponent() {
    render_flags = FLAG_RENDER;
    
    materials = new Material*[API::GetMaxIndexRangeLength()];
    offsets = new vec4[API::GetMaxIndexRangeLength()];
    for (size_t i = 0; i < API::GetMaxIndexRangeLength(); i++) {
        materials[i] = nullptr;
        offsets[i] = {0.0f, 0.0f, 0.0f, 0.0f};
    }
}

MeshComponent::~MeshComponent() {
    is_ready = false;
    
    if (draw_list_entry.generic) {
        Render::API::RemoveDrawListEntry(draw_list_entry);
    }

    if (aabb_tree_leaf) {
        AABB::RemoveLeaf(aabb_tree_leaf);
    }
    
    if (buffer) {
        free(buffer);
        buffer = nullptr;
    }
    
    for (size_t i = 0; i < API::GetMaxIndexRangeLength(); i++) {
        if (!materials[i]) continue;
        materials[i]->RemoveReference();
    }
    
    delete[] materials;
    delete[] offsets;
    materials = nullptr;
    offsets = nullptr;
};

static void IntoBuffer(char*& buffer, int& buffer_size, const void* data, int offset, int size) {
    if (buffer_size < offset + size) {
        if (!buffer_size) {
            buffer_size = 1024;
        } else {
            buffer_size += buffer_size >> 2;
        }
        
        buffer = (char*)realloc(buffer, buffer_size);
        
        IntoBuffer(buffer, buffer_size, data, offset, size);
        
        return;
    }
    
    mempcpy(buffer + offset, data, size);
}


void MeshComponent::InsertVertex(const MeshVertex& vertex, vec3& point, uint32_t& material) {
    auto [format, attributes] = vertex.GetData();
    VertexDefinition definition = GetVertexDefinition(format);
    
    if (vertex_format_set) {
        assert(vertex_format == format);
    } else {
        vertex_format = format;
        vertex_format_set = true;
    }

    for (size_t i = 0; i < definition.attribute_count; i++) {
        assert(attributes[i].GetType() != TYPE_UNDEFINED);
        
        int offset = definition.attributes[i].offset + vertices * definition.attributes[i].stride;
        
        switch (definition.attributes[i].type) {
            case Render::VertexAttribute::FLOAT32: switch (definition.attributes[i].size) {
                case 1: { float val = attributes[i]; IntoBuffer(buffer, buffer_size, &val, offset, sizeof(val)); } break;
                case 2: { vec2 val = attributes[i]; IntoBuffer(buffer, buffer_size, &val, offset, sizeof(val)); } break;
                case 3: { vec3 val = attributes[i]; IntoBuffer(buffer, buffer_size, &val, offset, sizeof(val)); } break;
                case 4: { vec4 val = attributes[i]; IntoBuffer(buffer, buffer_size, &val, offset, sizeof(val)); } break;
                default: assert(false && "Should not happen.");
            } break;
            case Render::VertexAttribute::INT32: switch (definition.attributes[i].size) {
                case 1: { int32_t val = attributes[i]; IntoBuffer(buffer, buffer_size, &val, offset, sizeof(val)); } break;
                case 2: { ivec2 val = {(int32_t)attributes[i], 0}; IntoBuffer(buffer, buffer_size, &val, offset, sizeof(val)); } break;
                case 3: { ivec3 val = {(int32_t)attributes[i], 0, 0}; IntoBuffer(buffer, buffer_size, &val, offset, sizeof(val)); } break;
                case 4: { ivec4 val = {(int32_t)attributes[i], 0, 0, 0}; IntoBuffer(buffer, buffer_size, &val, offset, sizeof(val)); } break;
                default: assert(false && "Should not happen.");
            } break;
            case Render::VertexAttribute::UINT32: switch (definition.attributes[i].size) {
                case 1: { uint32_t val = attributes[i]; IntoBuffer(buffer, buffer_size, &val, offset, sizeof(val)); } break;
                case 2: { uvec2 val = {(uint32_t)attributes[i], 0}; IntoBuffer(buffer, buffer_size, &val, offset, sizeof(val)); } break;
                case 3: { uvec3 val = {(uint32_t)attributes[i], 0, 0}; IntoBuffer(buffer, buffer_size, &val, offset, sizeof(val)); } break;
                case 4: { uvec4 val = {(uint32_t)attributes[i], 0, 0, 0}; IntoBuffer(buffer, buffer_size, &val, offset, sizeof(val)); } break;
                default: assert(false && "Should not happen.");
            } break;
        }
        
        if (definition.attributes[i].ffp_type == Render::VertexAttribute::FFP_POSITION) {
            point = attributes[i];
        }
        
        // TODO: actually fix this idiotismus
        if (definition.attributes[i].ffp_type == Render::VertexAttribute::FFP_IGNORE) {
            material = attributes[i];
        }
    }
    
    vertices++;
}

void MeshComponent::Add(const MeshVertex& v1, const MeshVertex& v2) {
    assert(!vertices || (render_flags & FLAG_DRAW_LINES));
    render_flags |= FLAG_DRAW_LINES;
    
    // we'll just be ignoring this point, since there is no meaningful way to
    // build an AABB tree leaf out of a line. we only do this for vertices
    vec3 point;
    uint32_t material;
    InsertVertex(v1, point, material);
    InsertVertex(v2, point, material);
}

void MeshComponent::Add(const MeshVertex& v1, const MeshVertex& v2, const MeshVertex& v3) {
    assert(!(render_flags & FLAG_DRAW_LINES));
    
    const bool any_vertices = vertices;
    vec3 p1 = {0, 0, 0}, p2 = {0, 0, 0}, p3 = {0, 0, 0};
    uint32_t material = 0;
    
    InsertVertex(v1, p1, material);
    InsertVertex(v2, p2, material);
    InsertVertex(v3, p3, material);
    
    vec3 min = MergeAABBMin(p1, MergeAABBMin(p2, p3));
    vec3 max = MergeAABBMax(p1, MergeAABBMax(p2, p3));
    
    if (!any_vertices) {
        aabb_min = min;
        aabb_max = max;
    } else {
        aabb_min = MergeAABBMin(aabb_min, min);
        aabb_max = MergeAABBMax(aabb_max, max);
    }
    
    vec3 normal = glm::normalize(glm::cross(glm::normalize(p2 - p1), glm::normalize(p3 - p1)));
    
    uint32_t index = aabb_triangles.size();
    aabb_triangles.push_back({.point1 = p1, .point2 = p2, .point3 = p3, .normal = normal, .material = material});
    aabb_leaves.push_back(aabb_tree.InsertLeaf(index, min, max));
}

void MeshComponent::Clear() {
    vertices = 0;
    
    // empty aabb tree
    for (auto leaf : aabb_leaves) {
        aabb_tree.RemoveLeaf(leaf);
    }
    aabb_leaves.clear();
    aabb_triangles.clear();
}

void MeshComponent::Reserve(Render::vertexformat_t format, size_t vertex_count) {
    VertexDefinition definition = GetVertexDefinition(format);
    size_t vertex_size = 0;
    for (size_t i = 0; i < definition.attribute_count; i++) {
        // since sizeof(float) == sizeof(u/int32_t) == 4, taking shortcut
        vertex_size += definition.attributes[i].size * 4;
    }
    size_t allocate_size = vertex_size * vertex_count;
    buffer = (char*)realloc(buffer, allocate_size);
    
    aabb_tree.Reserve(vertex_count / 3);
    aabb_triangles.reserve(vertex_count / 3);
    aabb_leaves.reserve(vertex_count / 3);
}

void MeshComponent::Commit() {
    API::UpdateVertexArray(vertex_array, buffer_size, buffer);
    Render::API::SetDrawListIndexRange(draw_list_entry, 0, vertices);
    
    RefreshAABB();
}

void MeshComponent::SetMaterial(Render::Material* material, uint32_t index) {
    assert(!this->IsReady());
    assert(index < API::GetMaxIndexRangeLength());
    if (material_type_set) {
        assert(material_type == material->GetType());
    } else {
        material_type = material->GetType();
        material_type_set = true;
    }
    materials[index] = material;
    material->AddReference();
}

/// Sets the world parameters for model rendering.
void MeshComponent::SetDirectionaLight(bool enabled) {
    if (!enabled) {
        render_flags |= FLAG_NO_DIRECTIONAL;
    } else {
        render_flags &= ~FLAG_NO_DIRECTIONAL;
    }
    
    if (draw_list_entry.generic) {
        Render::API::SetFlags(draw_list_entry, render_flags);
    }
}

/// Sets render debugging.
/// If set to false, the 3D model will not have debug text for it when the
/// render debug text rendering is turned on.
void MeshComponent::SetRenderDebug(bool enabled) {
    if (!enabled) {
        render_flags |= FLAG_NO_DEBUG;
    } else {
        render_flags &= ~FLAG_NO_DEBUG;
    }
    
    if (draw_list_entry.generic) {
        Render::API::SetFlags(draw_list_entry, render_flags);
    }
}

/// Sets the location of the mesh.
void MeshComponent::SetLocation(vec3 location) {
    this->location = location;
    
    if (draw_list_entry.generic) {
        Render::API::SetMatrix(draw_list_entry, PositionRotationScaleToMatrix(location, rotation, scale));
        light_t lights[4];
        Render::LightTree::FindLights(location, lights);
        Render::API::SetLights(draw_list_entry, lights);

        Render::API::SetSphericalHarmonic(draw_list_entry, LightGraph::LookupHarmonic(location, -1));
    
    }
    
    RefreshAABB();
}

/// Sets the rotation of the mesh.
void MeshComponent::SetRotation(quat rotation) {
    this->rotation = rotation;
    
    if (draw_list_entry.generic) {
        Render::API::SetMatrix(draw_list_entry, PositionRotationScaleToMatrix(location, rotation, scale));
    }

    RefreshAABB();
}

/// Sets the scale of the mesh.
void MeshComponent::SetScale(vec3 scale) {
    this->scale = scale;
    
    if (draw_list_entry.generic) {
        Render::API::SetMatrix(draw_list_entry, PositionRotationScaleToMatrix(location, rotation, scale));
    }
    
    RefreshAABB();
}

void MeshComponent::SetLayer(uint32_t layer) {
    this->layer = layer;
    
    if (draw_list_entry.generic) {
        Render::API::SetLayer(draw_list_entry, layer);
    }
}

void MeshComponent::SetTextureOffset(name_t material, vec4 offset) {
    for (uint32_t i = 0; materials[i] && i < API::GetMaxIndexRangeLength(); i++) {
        if (materials[i]->GetName() != material) continue;
        
        offsets[i] = offset;
        
        break;
    }

    if (!is_ready) return;

    Render::API::SetDrawListTextureOffsets(draw_list_entry, API::GetMaxIndexRangeLength(), offsets);
}

void MeshComponent::SetLineDrawingMode(bool enabled) {
    if (enabled) {
        render_flags |= FLAG_LINE_FILL_POLY;
    } else {
        render_flags &= ~FLAG_LINE_FILL_POLY;
    }
    
    if (draw_list_entry.generic) {
        Render::API::SetFlags(draw_list_entry, render_flags);
    }
}

/// Sets the scale of the model.
void MeshComponent::SetColor(vec3 color) {
    this->color = color;
    
    if (is_ready) {
        
        // TODO: fix hardcodings
        vec4 colors[15];
    
        // hold on... why the fuck are we mixing in material colors
        // the material API already has colors.. FUCK
        // TODO: figure this shit out
        for (uint32_t i = 0; i < API::GetMaxIndexRangeLength(); i++) {
            colors[i] = vec4(this->materials[i]->GetColor() * color, 1.0f);
        }
    
        Render::API::SetDrawListColors(draw_list_entry, API::GetMaxIndexRangeLength(), colors);
    }
}

void MeshComponent::Start() {
    // TODO: unhardcode these values
    // actually let's modify stackpool to allow popping, then move these into stackpool
    material_t materials[15]; uint32_t material_count = 0;
    vec4 colors[15];
    
    CreateVertexArray(GetVertexDefinition(vertex_format), vertex_array);
    
    draw_list_entry = Render::API::InsertDrawListEntry();
    
    for (uint32_t i = 0; i < API::GetMaxIndexRangeLength(); material_count = ++i) {
        if (!this->materials[i]) break;
        materials[i] = this->materials[i]->GetMaterial();
        colors[i] = vec4(this->materials[i]->GetColor() * color, 1.0f);
    }

    light_t lights[4];
    Render::LightTree::FindLights(location, lights);
    Render::API::SetLights(draw_list_entry, lights);

    Render::API::SetDrawListVertexArray(draw_list_entry, vertex_array);
    Render::API::SetDrawListMaterials(draw_list_entry, material_count, materials);
    Render::API::SetDrawListColors(draw_list_entry, API::GetMaxIndexRangeLength(), colors);
    Render::API::SetDrawListTextureOffsets(draw_list_entry, API::GetMaxIndexRangeLength(), offsets);
    const bool found_shader = Render::API::SetDrawListShader(draw_list_entry, vertex_format, material_type);
    Render::API::SetDrawListIndexRange(draw_list_entry, 0, vertices);

    Render::API::SetFlags(draw_list_entry, render_flags);
    Render::API::SetLayer(draw_list_entry, layer);

    Render::API::SetMatrix(draw_list_entry, PositionRotationScaleToMatrix(location, rotation, scale));

    if (glm::distance(aabb_min, aabb_max) > 0.001f) {
        render_flags |= FLAG_USE_AABB;
        Render::API::SetDrawListAABB(draw_list_entry, aabb_min, aabb_max);
        Render::API::SetFlags(draw_list_entry, render_flags);
    }
    
    if (!found_shader) {
        Log(Severity::WARNING, System::RENDER, "Can't find shader for combination of {} and {}!", GetVertexFormatName(vertex_format), GetMaterialTypeName(material_type));
    }
    
    Commit();
    
    is_ready = true;
}

void MeshComponent::RefreshAABB() {
    if (aabb_tree_leaf) {
        AABB::RemoveLeaf(aabb_tree_leaf);
    }
    
    aabb_tree_leaf = AABB::InsertLeaf(this);
}

}
