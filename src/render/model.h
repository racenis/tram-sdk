// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_MODEL_H
#define TRAM_SDK_RENDER_MODEL_H

#include <render/material.h>

namespace tram::Render {

struct ModelData;
struct ModelAABB;

struct AABBTriangle {
    vec3 point1, point2, point3;
    vec3 normal;
    uint32_t material;
};

struct IndexRange {
    uint32_t index_offset = 0;
    uint32_t index_length = 0;
    uint32_t material_count = 0;
    materialtype_t material_type;
    uint32_t materials[15] = { 0 };
};

class Model : public Resource {
public:
    Model(name_t name) : Resource(name) {}

    void LoadFromDisk();
    void LoadFromMemory();
    
    void Unload() {}
    
    vertexformat_t GetVertexFormat() const { return vertex_format; } 

    vertexarray_t GetVertexArray() const { return vertex_array; }
    indexarray_t GetIndexArray() const { return index_array; }
    
    const std::vector<Bone>& GetArmature() const { return armature; }
    const std::vector<Material*>& GetMaterials() const { return materials; }
    const std::vector<IndexRange>& GetIndexRanges() const { return index_ranges; }
    
    void DrawAABB(vec3 position, quat rotation);
    void FindAllFromRay(vec3 ray_pos, vec3 ray_dir, std::vector<AABBTriangle>& result);
    void FindAllFromAABB(vec3 min, vec3 max, std::vector<AABBTriangle>& result);
    
    vec3 GetAABBMin() const { return aabb_min; }
    vec3 GetAABBMax() const { return aabb_max; }
    
    vec3 GetOrigin() const { return origin; }
    
    float GetNearDistance() const { return fade_near; }
    float GetFarDistance() const { return fade_far; }
    void SetNearDistance(float dist) { fade_near = dist; }
    void SetFarDistance(float dist) { fade_far = dist; }
    
    void LoadAsModificationModel(Model* source, std::initializer_list<std::pair<Material*, Material*>> mapping);
    
    static Model* Find(name_t name);
    
protected:
    vertexformat_t vertex_format = VERTEX_STATIC;
    
    vertexarray_t vertex_array = {};
    indexarray_t index_array = {};
    
    std::vector<IndexRange> index_ranges;
    
    vec3 aabb_min = {0.0f, 0.0f, 0.0f};
    vec3 aabb_max = {0.0f, 0.0f, 0.0f};

    float fade_near = 0.0f;
    float fade_far = INFINITY;

    vec3 origin = {0.0f, 0.0f, 0.0f};
    
    std::vector<Material*> materials;

    Model* source = nullptr;

    std::vector<Bone> armature;
    ModelData* model_data = nullptr;
    ModelAABB* model_aabb = nullptr;
    size_t approx_vram_usage = 0;
};

}


#endif // TRAM_SDK_RENDER_MODEL_H