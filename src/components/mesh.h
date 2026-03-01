// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_MESHCOMPONENT_H
#define TRAM_SDK_COMPONENTS_MESHCOMPONENT_H

#include <framework/entitycomponent.h>
#include <render/material.h>
#include <render/renderer.h>

#include <framework/value.h>

#include <templates/aabb.h>

namespace tram {

class MeshVertex {
public:
    MeshVertex(Render::vertexformat_t format);
    ~MeshVertex();
    
    void SetPosition(vec3 position);
    void SetNormal(vec3 normal);
    void SetColor(vec3 color);
    void SetTextureUV(vec2 uv);
    void SetLightmapUV(vec2 uv);
    
    void SetAttribute(size_t index, value_t value);
    
    std::pair<Render::vertexformat_t, const value_t*> GetData() const;
private:
    value_t* attributes;
    Render::vertexformat_t format;
};

class MeshComponent : public EntityComponent {
public:
    MeshComponent();
    ~MeshComponent();

    void Add(const MeshVertex& v1, const MeshVertex& v2);
    void Add(const MeshVertex& v1, const MeshVertex& v2, const MeshVertex& v3);
    void Clear();
    void Reserve(Render::vertexformat_t format, size_t vertex_count);
    void Commit();

    void SetMaterial(Render::Material* material, uint32_t index);

    vec3 GetLocation() const { return location; }
    quat GetRotation() const { return rotation; }
    vec3 GetScale() const { return scale; }

    void SetLocation(vec3 nlocation);
    void SetRotation(quat nrotation);
    void SetScale(vec3 scale);
    void SetColor(vec3 color);
    void SetLayer(uint32_t layer);
    void SetTextureOffset(name_t material, vec4 offset);
    
    void SetLineDrawingMode(bool enabled);
    
    void SetDirectionaLight(bool enabled);
    void SetRenderDebug(bool enabled);
    
    inline vec3 GetAABBMin() { return aabb_min; }
    inline vec3 GetAABBMax() { return aabb_max; }
    
    void DrawAABB();
    void FindAllFromRay(vec3 ray_pos, vec3 ray_dir, std::vector<Render::AABBTriangle>& result);
    void FindAllFromAABB(vec3 min, vec3 max, std::vector<Render::AABBTriangle>& result);

    void EventHandler(Event &event) { return; }
protected:
    void RefreshAABB();
    
    void Start();
    
    void InsertVertex(const MeshVertex& vertex, vec3& point, uint32_t& material);

    char* buffer = nullptr;
    int buffer_size = 0;
    int vertices = 0;

    Render::drawlistentry_t draw_list_entry;
    Render::Material** materials;
    vec4* offsets;
    
    Render::vertexarray_t vertex_array;
    Render::vertexformat_t vertex_format;
    Render::materialtype_t material_type;
    bool vertex_format_set = false;
    bool material_type_set = false;
    
    
    vec3 location = {0.0f, 0.0f, 0.0f};
    quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    vec3 scale = {1.0f, 1.0f, 1.0f};
    vec3 color = {1.0f, 1.0f, 1.0f};
    uint32_t layer = 0;
    
    vec3 aabb_min = {0.0f, 0.0f, 0.0f};
    vec3 aabb_max = {0.0f, 0.0f, 0.0f};

    std::vector<Render::AABBTriangle> aabb_triangles;
    AABBTree aabb_tree;
    std::vector<AABBTree::node_t> aabb_leaves;

    void* aabb_tree_leaf = 0;

    uint32_t render_flags = 0;
};

}

#endif // TRAM_SDK_COMPONENTS_MESHCOMPONENT_H