// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/core.h>
#include <framework/stats.h>
#include <framework/file.h>
#include <framework/logging.h>
#include <framework/async.h>

#include <render/model.h>
#include <render/api.h>
#include <render/vertices.h>
#include <render/error.h>

#include <platform/api.h>

#include <cstring>

#include <templates/hashmap.h>
#include <templates/aabb.h>

#include <config.h>

#include <charconv>

using namespace tram;

template <> Pool<Render::Model> PoolProxy<Render::Model>::pool("model pool", 500);

namespace tram::Render {

static Hashmap<Model*> model_list("model name list", 500);

Model* Model::Find(name_t name) {
    Model* model = model_list.find(name);
    
    if (!model) {
        model = PoolProxy<Model>::make(name);
        model_list.insert(name, model);
    }
    
    return model;
}

/// Finds triangles that intersect ray.
/// Finds the triangles that intersect the given ray. The ray's origin and
/// direction must be provided in the local model coordinates.
void ModelData::FindAllFromRay(vec3 ray_pos, vec3 ray_dir, std::vector<AABBTriangle>& result) {
    std::vector<uint32_t> results;
    results.reserve(10);
    
    tree->find(ray_pos, ray_dir, results);
    
    for (auto key : results) {
        AABBTriangle triangle;
        triangle.point1 = GetPosition(key, 0);
        triangle.point2 = GetPosition(key, 1);
        triangle.point3 = GetPosition(key, 2);
        triangle.material = GetMaterial(key);
        
        triangle.normal = {0.0f, 0.0f, 0.0f};
        triangle.normal += GetNormal(key, 0);
        triangle.normal += GetNormal(key, 1);
        triangle.normal += GetNormal(key, 2);
        triangle.normal = glm::normalize(triangle.normal / 3.0f);
        
        result.push_back(triangle);
    }
}

void ModelData::FindAllFromAABB(vec3 min, vec3 max, std::vector<AABBTriangle>& result) {
    tree->find(min, max, [&](uint32_t key) {
        AABBTriangle triangle;
        triangle.point1 = GetPosition(key, 0);
        triangle.point2 = GetPosition(key, 1);
        triangle.point3 = GetPosition(key, 2);
        triangle.material = GetMaterial(key);
        
        triangle.normal = {0.0f, 0.0f, 0.0f};
        triangle.normal += GetNormal(key, 0);
        triangle.normal += GetNormal(key, 1);
        triangle.normal += GetNormal(key, 2);
        triangle.normal = glm::normalize(triangle.normal / 3.0f);
        
        result.push_back(triangle);
    });
}

static int total_counter = 0;
static int node_counter = 0;
static int leaf_counter = 0;

static void DrawAABBNodeChildren(const AABBTree& tree, AABBTree::node_t node, ModelData* data, vec3 position, quat rotation) {
    total_counter++;
    
    if (tree.IsLeaf(node)) {
        vec3 point1 = position + (rotation * data->GetPosition(tree.GetValue(node), 0));
        vec3 point2 = position + (rotation * data->GetPosition(tree.GetValue(node), 1));
        vec3 point3 = position + (rotation * data->GetPosition(tree.GetValue(node), 2));
        
        AddLine(point1, point2, COLOR_WHITE);
        AddLine(point2, point3, COLOR_WHITE);
        AddLine(point3, point1, COLOR_WHITE);
        
        leaf_counter++;
    } else {
        DrawAABBNodeChildren(tree, tree.GetLeft(node), data, position, rotation);
        DrawAABBNodeChildren(tree, tree.GetRight(node), data, position, rotation);
        
        if (tree.GetParent(node) == AABBTree::INVALID) {
            AddLineAABB(tree.GetMin(node), tree.GetMax(node), position, rotation, COLOR_RED);
        } else {
            AddLineAABB(tree.GetMin(node), tree.GetMax(node), position, rotation, COLOR_PINK);
        }
        
        node_counter++;
    }
}

/// Draws the AABB tree.
/// Draws the AABB tree of the 3D model using debug lines. This might be useful
/// for debugging if raycasts or some other lookups fail on the 3D model.
/// @param position Position of the 3D model in the scene.
/// @param rotation Rotation of the 3D model in the scene.
void ModelData::DrawAABB(vec3 position, quat rotation) {
    if (!tree) return;
    
    total_counter = 0;
    node_counter = 0;
    leaf_counter = 0;

    DrawAABBNodeChildren(*tree, tree->get_root(), this, position, rotation);
}

static vec3 TriangleAABBMin(vec3 point1, vec3 point2, vec3 point3) {
    return {
        point1.x < point2.x ? (point1.x < point3.x ? point1.x : point3.x) : (point2.x < point3.x ? point2.x : point3.x),
        point1.y < point2.y ? (point1.y < point3.y ? point1.y : point3.y) : (point2.y < point3.y ? point2.y : point3.y),
        point1.z < point2.z ? (point1.z < point3.z ? point1.z : point3.z) : (point2.z < point3.z ? point2.z : point3.z)
    };
}

static vec3 TriangleAABBMax(vec3 point1, vec3 point2, vec3 point3) {
    return {
        point1.x > point2.x ? (point1.x > point3.x ? point1.x : point3.x) : (point2.x > point3.x ? point2.x : point3.x),
        point1.y > point2.y ? (point1.y > point3.y ? point1.y : point3.y) : (point2.y > point3.y ? point2.y : point3.y),
        point1.z > point2.z ? (point1.z > point3.z ? point1.z : point3.z) : (point2.z > point3.z ? point2.z : point3.z)
    };
}

void ModelData::BuildAABB(vec3& aabb_min, vec3& aabb_max) {
    if (tree) {
        return;
    } else {
        tree = new AABBTree;
    }
    
    for (uint32_t i = 0; i < GetTriangleCount(); i++) {
        const auto point1 = GetPosition(i, 0);
        const auto point2 = GetPosition(i, 1);
        const auto point3 = GetPosition(i, 2);
   
        vec3 triangle_aabb_min = TriangleAABBMin(point1, point2, point3);
        vec3 triangle_aabb_max = TriangleAABBMax(point1, point2, point3);
    
        tree->insert(i, triangle_aabb_min, triangle_aabb_max);
    }
    
    aabb_min = tree->GetAABBMin();
    aabb_max = tree->GetAABBMax();
}

ModelData::~ModelData() {
    if (tree) delete tree;
}

struct TriangleBucket {
    materialtype_t material_type;       // material type for this bucket
    std::vector<uint32_t> materials;    // which materials have already been added to the bucket
    std::vector<Triangle> triangles;    // triangle indices in the bucket
};

struct BucketMapping {
    int32_t bucket = -1;
    int32_t index_in_bucket = -1;
};

static uint32_t PutTriangleInBucket(
    std::vector<TriangleBucket>& buckets,
    std::vector<BucketMapping>& bucket_mappings,
    const std::vector<Material*>& materials,
    uint32_t material_index,
    Triangle triangle
) {
    // check if material is already in a bucket
    if (auto& mapping = bucket_mappings[material_index]; mapping.bucket != -1) {
        buckets[mapping.bucket].triangles.push_back(triangle);
        return mapping.index_in_bucket;
    }
    
    // check if there is already a bucket with the same type as material
    for (size_t i = 0; i < buckets.size(); i++) {
        if (buckets[i].material_type == materials[material_index]->GetType()
            && buckets[i].materials.size() < API::GetMaxIndexRangeLength()
        ) {
            uint32_t bucket_index = buckets[i].materials.size();
            
            buckets[i].materials.push_back(material_index);
            buckets[i].triangles.push_back(triangle);
            
            bucket_mappings[material_index].bucket = i;
            bucket_mappings[material_index].index_in_bucket = bucket_index;
            
            return bucket_index;
        }
    }
    
    // insert a new bucket
    buckets.push_back({
        materials[material_index]->GetType(),
        {material_index},
        {triangle}
    });
    
    buckets.back().triangles.reserve(10000);
    
    bucket_mappings[material_index].bucket = buckets.size() - 1;
    bucket_mappings[material_index].index_in_bucket = 0;
    
    return 0;
}

class StaticModel : public ModelData {
public:
    vec3 GetPosition(int32_t index, int32_t vertex) override {
        switch (vertex) {
            default:
            case 0: return vertices[indices[index].indices.x].co;
            case 1: return vertices[indices[index].indices.y].co;
            case 2: return vertices[indices[index].indices.z].co;
        }
    }
    
    vec3 GetNormal(int32_t index, int32_t vertex) override {
        switch (vertex) {
            default:
            case 0: return vertices[indices[index].indices.x].normal;
            case 1: return vertices[indices[index].indices.y].normal;
            case 2: return vertices[indices[index].indices.z].normal;
        }
    }
    
    int32_t GetMaterial(int32_t index) override {
        return materials[index];
    }
    
    uint32_t GetTriangleCount() override {
        return indices.size();
    }
    
    bool LoadFromDisk(LoadInfo info) override {
        char path[PATH_LIMIT];
    
        // trying to load model as a static model, text mode
        snprintf(path, PATH_LIMIT, "data/models/%s.stmdl", (const char*)info.name);
        
        File file(path, File::READ);
        if (!file.is_open()) return false;
        
        std::vector<TriangleBucket> triangle_buckets;
        std::vector<BucketMapping> bucket_mappings;
        
        info.vertex_format = VERTEX_STATIC;
        
        Log(Severity::INFO, System::RENDER, "Loading file: {}", path);

        // doing some extra work, so that we can load the old .stmdl that didn't
        // have a header
        auto header = file.read_token();

        bool has_header = header == "STMDLv1";

        uint32_t vcount;   // number of vertices
        
        if (!has_header) {
            std::from_chars<uint32_t>(header.begin(), header.end(), vcount);
        } else {
            vcount = file.read_uint32();
        }
        
        uint32_t tcount = file.read_uint32();   // number of triangles
        uint32_t mcount = file.read_uint32();   // number of materials

        if (mcount == 0) {
            Log(Severity::ERROR, System::RENDER, "Model {} has zero materials!", path);
            return false;
        }

        if (has_header) {
            uint32_t metadata_fields = file.read_uint32();
            
            for (uint32_t i = 0; i < metadata_fields; i++) {
                name_t field = file.read_name();
                
                if (field == "lightmap") {
                    file.read_int32();
                    file.read_int32();
                } else if (field == "near") {
                    info.fade_near = file.read_float32();
                } else if (field == "far") {
                    info.fade_far = file.read_float32();
                } else if (field == "origin") {
                    info.origin = {file.read_float32(), file.read_float32(), file.read_float32()};
                } else {
                    Log(Severity::WARNING, System::RENDER, "File {} has unrecognized metadata {}, skipping entry", path, field);
                    file.skip_linebreak();
                }
            }
        }
        
        bucket_mappings.resize(mcount);
        assert(bucket_mappings.size() == mcount);
        
        for (uint32_t i = 0; i < mcount; i++) {
            info.materials.push_back(Material::Find(file.read_name()));
        }
        
        for (uint32_t i = 0; i < vcount; i++) {
            vertices.push_back(StaticModelVertex {
                .co = {
                    file.read_float32(),
                    file.read_float32(),
                    file.read_float32()
                },
                
                .normal = {
                    file.read_float32(),
                    file.read_float32(),
                    file.read_float32()
                },
                
                .tex = {
                    file.read_float32(),
                    file.read_float32()
                },
                
                .lighttex = {
                    file.read_float32(),
                    file.read_float32()
                },
    
                .texture = 0 // will be filled in later
            });
        }
        
        for (uint32_t i = 0; i < tcount; i++) {
            Triangle index {
                .indices = {
                    file.read_uint32(),
                    file.read_uint32(),
                    file.read_uint32()
                }
            };
            
            uint32_t material_index = file.read_uint32();
            assert(material_index < mcount);
            assert(triangle_buckets.size() <= mcount);
            
            materials.push_back(material_index);
            
            uint32_t bucket_index = PutTriangleInBucket(triangle_buckets, bucket_mappings, info.materials, material_index, index);
            
            vertices[index.indices.x].texture = bucket_index;
            vertices[index.indices.y].texture = bucket_index;
            vertices[index.indices.z].texture = bucket_index;
        }

        for (auto& bucket : triangle_buckets) {
            IndexRange range {
                .index_offset = (uint32_t) indices.size(),
                .index_length = (uint32_t) bucket.triangles.size(),
                .material_count = (uint32_t) bucket.materials.size(),
                .material_type = bucket.material_type,
            };
            
            for (size_t i = 0; i < bucket.materials.size(); i++) {
                range.materials[i] = bucket.materials[i];
            }
            
            info.index_ranges.push_back(range);
            indices.insert(indices.end(), bucket.triangles.begin(), bucket.triangles.end());
        }

        Bone rootbone {
            .name = "Root",
            .parent = -1,
            .head = {0.0f, 0.0f, 0.0f},
            .tail = {0.0f, 1.0f, 0.0f},
            .roll = 0.0f
        };

        info.armature.push_back(rootbone);
        
        return true;
    }
    
    void LoadFromMemory(LoadInfo info) override {
        API::CreateIndexedVertexArray(
            GetVertexDefinition(VERTEX_STATIC),
            info.vertex_array,
            info.index_array,
            vertices.size() * sizeof(StaticModelVertex),
            &vertices[0],
            indices.size() * sizeof(Triangle),
            &indices[0]
        );
        
        size_t approx_memory = (indices.size() * sizeof(Triangle)) + (vertices.size() * sizeof(StaticModelVertex));
        info.approx_vram_usage += approx_memory;
        Stats::Add(Stats::RESOURCE_VRAM, approx_memory);
    }
    
    ~StaticModel() override {
        
    }
    
    std::vector<StaticModelVertex> vertices;
    std::vector<Triangle> indices;
    std::vector<uint32_t> materials;
};

class DynamicModel : public ModelData {
public:
    vec3 GetPosition(int32_t index, int32_t vertex) override {
        switch (vertex) {
            default:
            case 0: return vertices[indices[index].indices.x].co;
            case 1: return vertices[indices[index].indices.y].co;
            case 2: return vertices[indices[index].indices.z].co;
        }
    }
    
    vec3 GetNormal(int32_t index, int32_t vertex) override {
        switch (vertex) {
            default:
            case 0: return vertices[indices[index].indices.x].normal;
            case 1: return vertices[indices[index].indices.y].normal;
            case 2: return vertices[indices[index].indices.z].normal;
        }
    }
    
    int32_t GetMaterial(int32_t index) override {
        return materials[index];
    }
    
    uint32_t GetTriangleCount() override {
        return indices.size();
    }
    
    bool LoadFromDisk(LoadInfo info) override {
        char path[PATH_LIMIT];
        
        snprintf(path, PATH_LIMIT, "data/models/%s.dymdl", info.name);

        File file(path, File::READ);
        if (!file.is_open()) return false;
        
        std::vector<TriangleBucket> triangle_buckets;
        std::vector<BucketMapping> bucket_mappings;
        
        info.vertex_format = VERTEX_DYNAMIC;

        Log(Severity::INFO, System::RENDER, "Loading file: {}", path);

        name_t file_version = file.read_name();
        
        if (file_version != "DYMDLv1") {
            Log(Severity::WARNING, System::RENDER, "Model {} is not using right DYMDLv1 version!", path);
            Log(Severity::WARNING, System::RENDER, "Add \"DYMDLv1\" to file and also add bone roll to the bone definitions (0.0 to the end of lines), or reexport.");
            return false;
        }
        
        uint32_t vcount = file.read_uint32();   // number of vertices
        uint32_t tcount = file.read_uint32();   // number of triangles
        uint32_t mcount = file.read_uint32();   // number of materials
        uint32_t bcount = file.read_uint32();   // number of bones
        uint32_t gcount = file.read_uint32();   // number of vertex groups

        if (mcount == 0) {
            Log(Severity::WARNING, System::RENDER, "Model {} has zero materials!", path);
            return false;
        }

        bucket_mappings.resize(mcount);

        for (uint32_t i = 0; i < mcount; i++) {
            info.materials.push_back(Material::Find(file.read_name()));
        }
        
        for (uint32_t i = 0; i < vcount; i++) {
            DynamicModelVertex vertex {
                .co = {
                    file.read_float32(),
                    file.read_float32(),
                    file.read_float32()
                },
                
                .normal = {
                    file.read_float32(),
                    file.read_float32(),
                    file.read_float32()
                },
                
                .tex = {
                    file.read_float32(),
                    file.read_float32()
                },
                
                // this looks stupid, but its because I accidentally made the
                // model format stupid. maybe will be fixed in DYMDLv2
                .bone = {0, 0, 0, 0},
                
                .boneweight = {0.0f, 0.0f, 0.0f, 0.0f},
    
                .texture = 0 // will be filled in later
            };
            
            vertex.bone.x = file.read_uint32();
            vertex.boneweight.x = file.read_float32();
            vertex.bone.y = file.read_uint32();
            vertex.boneweight.y = file.read_float32();
            vertex.bone.z = file.read_uint32();
            vertex.boneweight.z = file.read_float32();
            vertex.bone.w = file.read_uint32();
            vertex.boneweight.w = file.read_float32();

            vertices.push_back(vertex);
        }
        
        // this is basically a repetion of the same code as for static model
        // so, it could be possible to make this a function.
        for (uint32_t i = 0; i < tcount; i++) {
            Triangle index {
                .indices = {
                    file.read_uint32(),
                    file.read_uint32(),
                    file.read_uint32()
                }
            };
            
            uint32_t material_index = file.read_uint32();
            materials.push_back(material_index);
            
            uint32_t bucket_index = PutTriangleInBucket(triangle_buckets, bucket_mappings, info.materials, material_index, index);
            
            vertices[index.indices.x].texture = bucket_index;
            vertices[index.indices.y].texture = bucket_index;
            vertices[index.indices.z].texture = bucket_index;
        }
        
        for (uint32_t i = 0; i < bcount; i++) {
            info.armature.push_back(Bone {
                .name = file.read_name(),
                .parent = file.read_int32(),
                
                .head = {
                    file.read_float32(),
                    file.read_float32(),
                    file.read_float32()
                },
                
                .tail = {
                    file.read_float32(),
                    file.read_float32(),
                    file.read_float32()
                },
                
                .roll = file.read_float32()
            });
        }
        
        for (uint32_t i = 0; i < gcount; i++) {
            name_t group = file.read_name();
            
            if (info.armature[i].name != group) {
                Log(Severity::WARNING, System::RENDER, "Model {} group {} is not matching bone {}!", info.name, group, info.armature[i].name);
            }
            
            groups.push_back(group);
        }
        
        for (auto& bucket : triangle_buckets) {
            IndexRange range {
                .index_offset = (uint32_t) indices.size(),
                .index_length = (uint32_t) bucket.triangles.size(),
                .material_count = (uint32_t) bucket.materials.size(),
                .material_type = bucket.material_type,
            };
            
            for (size_t i = 0; i < bucket.materials.size(); i++) {
                range.materials[i] = bucket.materials[i];
            }
            
            info.index_ranges.push_back(range);
            indices.insert(indices.end(), bucket.triangles.begin(), bucket.triangles.end());
        }
        
        return true;
    }
    
    void LoadFromMemory(LoadInfo info) override {
        API::CreateIndexedVertexArray(
            GetVertexDefinition(VERTEX_DYNAMIC),
            info.vertex_array,
            info.index_array, 
            vertices.size() * sizeof(DynamicModelVertex),
            &vertices[0],
            indices.size() * sizeof(Triangle),
            &indices[0]
        );

        size_t approx_memory = (indices.size() * sizeof(Triangle)) + (vertices.size() * sizeof(DynamicModelVertex));
        info.approx_vram_usage += approx_memory;
        Stats::Add(Stats::RESOURCE_VRAM, approx_memory);
    }
    
    ~DynamicModel() override {
        
    }
    
    std::vector<DynamicModelVertex> vertices;
    std::vector<Triangle> indices;
    std::vector<UID> groups;
    std::vector<uint32_t> materials;
};


class ModificationModel : public ModelData {
public:
    vec3 GetPosition(int32_t index, int32_t vertex) override {
        return source->GetData()->GetPosition(index, vertex);
    }
    
    vec3 GetNormal(int32_t index, int32_t vertex) override {
        return source->GetData()->GetNormal(index, vertex);
    }
    
    int32_t GetMaterial(int32_t index) override {
        return source->GetData()->GetMaterial(index);
    }
    
    uint32_t GetTriangleCount() override {
        return source->GetData()->GetTriangleCount();
    }
    
    bool LoadFromDisk(LoadInfo info) override {
        char path[PATH_LIMIT];
        
        snprintf(path, PATH_LIMIT, "data/models/%s.mdmdl", info.name);
        
        File file(path, File::READ);
        if (!file.is_open()) return false;
        
        name_t file_version = file.read_name();
        
        if (file_version != "MDMDLv1") {
            Log(Severity::WARNING, System::RENDER, "Model {} is not using right MDMDLv1 version!", path);
        }
        
        Log(Severity::INFO, System::RENDER, "Loading file: {}", path);
        
        name_t source_model = file.read_name();
        
        info.source = Model::Find(source_model);
        this->source = info.source;
        
        info.source->AddReference();
        Async::LoadDependency(info.source);
        
        std::vector<std::pair<name_t, name_t>> mappings;
        
        while (file.is_continue()) {
            mappings.push_back({file.read_name(), file.read_name()});
        }
        
        for (Material* mat : info.source->GetMaterials()) {
            for (auto mapping : mappings) {
                if (mapping.first == mat->GetName()) {
                    info.materials.push_back(Material::Find(mapping.second));
                    goto next;
                }
            }
            info.materials.push_back(mat);
            next:;
        }
        
        return true;
    }
    
    void LoadFromMemory(LoadInfo info) override {
        
    }
    
    ~ModificationModel() override {
        
    }
    
    Model* source = nullptr;
};


void Model::LoadFromMemory() {
    if (status != LOADED) {
        Log(Severity::WARNING, System::RENDER, "Model {} hasn't been loaded! Ignoring Model::LoadFromMemory() call.", name);
        return;
    }
    
    if (!Platform::Window::IsRenderContextThread()) {
        Log(Severity::WARNING, System::RENDER, "Model::LoadFromMemory() not being called from render thread! Ignoring.");
        return;
    }
    
    ModelData::LoadInfo info {
        .name = name,
        .vertex_format = vertex_format,
        .vertex_array = vertex_array,
        .index_array = index_array,
        .index_ranges = index_ranges,
        .fade_near = fade_near,
        .fade_far = fade_far,
        .origin = origin,
        .materials = materials,
        .source = source,
        .model_data = model_data,
        .armature = armature,
        .approx_vram_usage = approx_vram_usage,
    };
    
    model_data->LoadFromMemory(info);
    
    if (source) {
        this->vertex_format = source->vertex_format;
        this->vertex_array = source->vertex_array;
        this->index_array = source->index_array;
        this->index_ranges = source->index_ranges;
        this->aabb_min = source->aabb_min;
        this->aabb_max = source->aabb_max;
        this->armature = source->armature;
        this->model_data = source->model_data;
    }
    
    status = LOADED;
}

void Model::LoadFromDisk() {
    if (status != UNLOADED) {
        Log(Severity::WARNING, System::RENDER, "Model {} already loaded! Ignoring Model::LoadFromDisk() call.", name);
        return;
    }
    
    ModelData::LoadInfo info {
        .name = name,
        .vertex_format = vertex_format,
        .vertex_array = vertex_array,
        .index_array = index_array,
        .index_ranges = index_ranges,
        .fade_near = fade_near,
        .fade_far = fade_far,
        .origin = origin,
        .materials = materials,
        .source = source,
        .model_data = model_data,
        .armature = armature,
        .approx_vram_usage = approx_vram_usage,
    };
    
    model_data = new StaticModel;
    if (model_data->LoadFromDisk(info)) {
        model_data->BuildAABB(aabb_min, aabb_max);
        goto finish;
    }
    delete model_data;
    
    model_data = new DynamicModel;
    if (model_data->LoadFromDisk(info)) {
        model_data->BuildAABB(aabb_min, aabb_max);
        goto finish;
    }
    delete model_data;
    
    model_data = new ModificationModel;
    if (model_data->LoadFromDisk(info)) {
        delete model_data;
        model_data = source->GetData();
        goto finish;
    }
    delete model_data;
    
    
    
    Log(Severity::NOTE, System::RENDER, "Model file for {} couldn't be accessed!", name);

    {
        vertex_format = VERTEX_STATIC;

        StaticModel* static_data = new StaticModel;
        model_data = static_data;
        MakeNewErrorModel(static_data->vertices, static_data->indices);
        
        Material* error_material = Material::Find("defaulttexture");
        materials.push_back(error_material);

        index_ranges.push_back(IndexRange {
            .index_offset = 0,
            .index_length = (uint32_t) static_data->indices.size(),
            .material_count = 1,
            .material_type = MATERIAL_TEXTURE,
            .materials = {0}
        });
        
        armature.push_back(Bone {
            .name = "Root",
            .parent = -1,
            .head = {0.0f, 0.0f, 0.0f},
            .tail = {0.0f, 1.0f, 0.0f},
            .roll = 0.0f
        });
        
        load_fail = true;
    }
    
    model_data->BuildAABB(aabb_min, aabb_max);
    
finish:
    for (Material* mat : this->materials) {
        mat->AddReference();
        Async::LoadDependency(mat);
    }
    
    
    
    status = LOADED;
}

void Model::LoadAsModificationModel(Model* source, std::initializer_list<std::pair<Material*, Material*>> mappings) {
    assert(status == Resource::UNLOADED);
    
    this->source = source;
    
    this->source->AddReference();
    Async::LoadDependency(this->source);
        
    for (Material* mat : this->source->materials) {
        for (const auto& mapping : mappings) {
            if (mapping.first == mat) {
                materials.push_back(mapping.second);
                goto next;
            }
        }
        materials.push_back(mat);
        next:;
    }
    
    for (Material* mat : this->materials) {
        mat->AddReference();
        Async::LoadDependency(mat);
    }
    
    status = LOADED;
    
    return;
}

void Model::Unload() {
    if (status != READY) {
        Log(Severity::WARNING, System::RENDER, "Model {} hasn't been loaded! Ignoring Model::Unload() call.", name);
        return;
    }
    
    if (!Platform::Window::IsRenderContextThread()) {
        Log(Severity::WARNING, System::RENDER, "Model::Unload() not being called from render thread! Ignoring.");
        return;
    }
    
    if (!source) {
        API::RemoveVertexArray(vertex_array, index_array);
    } else {
        source->RemoveReference();
    }
    
    for (size_t i = 0; i < materials.size(); i++) {
        materials[i]->RemoveReference();
    }
    
    index_ranges.clear();
    materials.clear();
    armature.clear();

    if (!source && model_data) delete model_data;
    
    model_data = nullptr;
    
    Stats::Remove(Stats::RESOURCE_VRAM, approx_vram_usage);
    
    status = UNLOADED;
}

}