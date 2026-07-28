// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_MODEL_H
#define TRAM_SDK_RENDER_MODEL_H

#include <render/material.h>

namespace tram {
    class AABBTree;
}

namespace tram::Render {

class ModelData;

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
    
    void Unload();
    
    vertexformat_t GetVertexFormat() const { return vertex_format; } 

    vertexarray_t GetVertexArray() const { return vertex_array; }
    indexarray_t GetIndexArray() const { return index_array; }
    
    const std::vector<Bone>& GetArmature() const { return armature; }
    const std::vector<Material*>& GetMaterials() const { return materials; }
    const std::vector<IndexRange>& GetIndexRanges() const { return index_ranges; }

    vec3 GetAABBMin() const { return aabb_min; }
    vec3 GetAABBMax() const { return aabb_max; }
    
    vec3 GetOrigin() const { return origin; }
    
    float GetNearDistance() const { return fade_near; }
    float GetFarDistance() const { return fade_far; }
    void SetNearDistance(float dist) { fade_near = dist; }
    void SetFarDistance(float dist) { fade_far = dist; }
    
    ModelData* GetData() const { return model_data; }
    
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
    ModelData* model_data = nullptr;

    std::vector<Bone> armature;
    size_t approx_vram_usage = 0;
};

class ModelData {
public:
    void DrawAABB(vec3 position, quat rotation);
    void FindAllFromRay(vec3 ray_pos, vec3 ray_dir, std::vector<AABBTriangle>& result);
    void FindAllFromAABB(vec3 min, vec3 max, std::vector<AABBTriangle>& result);
    
    virtual vec3 GetPosition(int32_t index, int32_t vertex) = 0;
    virtual vec3 GetNormal(int32_t index, int32_t vertex) = 0;
    virtual int32_t GetMaterial(int32_t index) = 0;
    virtual uint32_t GetTriangleCount() = 0;
    
    //static void Register(ModelData*(*)());
protected:
    AABBTree* tree = nullptr;
    
    struct LoadInfo {
        const char* name;
        vertexformat_t& vertex_format;
        vertexarray_t& vertex_array;
        indexarray_t& index_array;
        std::vector<IndexRange>& index_ranges;
        float& fade_near;
        float& fade_far;
        vec3& origin;
        std::vector<Material*>& materials;
        Model*& source;
        ModelData*& model_data;
        std::vector<Bone>& armature;
        size_t& approx_vram_usage;
    };
    
    virtual bool LoadFromDisk(LoadInfo info) = 0;
    virtual void LoadFromMemory(LoadInfo info) = 0;
    
    void BuildAABB(vec3& min, vec3& max);
    
    virtual ~ModelData();
    
    friend class Model;
};


}

#endif // TRAM_SDK_RENDER_MODEL_H