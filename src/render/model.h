// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_MODEL_H
#define RENDER_MODEL_H

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
    Model (name_t name) : Resource(name) {}

    void LoadFromDisk();
    void LoadFromMemory();
    
    void Unload() {}
    
    vertexformat_t GetVertexFormat() const { return vertex_format; } 
    vertexhandle_t GetVertexBuffer() const { return vertex_buffer_handle; }
    vertexhandle_t GetIndexBuffer() const { return index_buffer_handle; }
    vertexhandle_t GetVertexArray() const { return vertex_array_handle; }
    
    const std::vector<Bone>& GetArmature() const { return armature; }
    const std::vector<Material*>& GetMaterials() const { return materials; }
    const std::vector<IndexRange>& GetIndexRanges() const { return index_ranges; }
    
    void DrawAABB(vec3 position, quat rotation);
    void FindAllFromRay(vec3 ray_pos, vec3 ray_dir, std::vector<AABBTriangle>& result);
    
    vec3 GetAABBMin() const { return aabb_min; }
    vec3 GetAABBMax() const { return aabb_max; }
    
    static Model* Find(name_t name);
    
protected:
    vertexformat_t vertex_format = VERTEX_STATIC;
    
    vertexhandle_t vertex_buffer_handle = 0;
    vertexhandle_t index_buffer_handle = 0;
    vertexhandle_t vertex_array_handle = 0;
    
    std::vector<IndexRange> index_ranges;
    
    vec3 aabb_min;
    vec3 aabb_max;
    
    std::vector<Material*> materials;

    std::vector<Bone> armature;
    ModelData* model_data = nullptr;
    ModelAABB* model_aabb = nullptr;
    size_t approx_vram_usage = 0;
};

}


#endif // RENDER_MODEL_H