// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <framework/stats.h>
#include <framework/file.h>
#include <framework/logging.h>

#include <render/model.h>
#include <render/api.h>
#include <render/vertices.h>
#include <render/error.h>

#include <framework/async.h>

#include <fstream>
#include <cstring>

#include <templates/hashmap.h>
#include <templates/aabb.h>

//#include <set>

using namespace tram;

template <> Pool<Render::Model> PoolProxy<Render::Model>::pool("model pool", 500);

namespace tram::Render {

static Hashmap<Model*> model_list ("model name list", 500);

Model* Model::Find (name_t name) {
    Model* model = model_list.Find (name);
    
    if (!model) {
        model = PoolProxy<Model>::New (name);
        model_list.Insert (UID(name), model);
    }
    
    return model;
}

void Model::LoadFromMemory() {
    assert(status == LOADED);
    
    if (vertex_format == VERTEX_STATIC){
        StaticModelData* data = (StaticModelData*) model_data;

        API::CreateIndexedVertexArray(
            GetVertexDefinition(VERTEX_STATIC),
            vertex_array,
            index_array,
            data->vertices.size() * sizeof(StaticModelVertex),
            &data->vertices[0],
            data->indices.size() * sizeof(Triangle),
            &data->indices[0]
        );
        
        size_t approx_memory = (data->indices.size() * sizeof(Triangle)) + (data->vertices.size() * sizeof(StaticModelVertex));
        approx_vram_usage += approx_memory;
        Stats::Add(Stats::RESOURCE_VRAM, approx_memory);

        delete model_data;
        model_data = nullptr;

        status = READY;

        return;
    } else if (vertex_format == VERTEX_DYNAMIC){
        DynamicModelData* data = (DynamicModelData*) model_data;

        API::CreateIndexedVertexArray(
            GetVertexDefinition(VERTEX_DYNAMIC),
            vertex_array,
            index_array, 
            data->vertices.size() * sizeof(DynamicModelVertex),
            &data->vertices[0],
            data->indices.size() * sizeof(Triangle),
            &data->indices[0]
        );

        size_t approx_memory = (data->indices.size() * sizeof(Triangle)) + (data->vertices.size() * sizeof(DynamicModelVertex));
        approx_vram_usage += approx_memory;
        //RESOURCE_VRAM_USAGE += approx_memory;
        Stats::Add(Stats::RESOURCE_VRAM, approx_memory);

        delete model_data;
        model_data = nullptr;

        status = READY;

        return;
    }
}

struct ModelAABB {
    AABBTree tree;
    std::vector<AABBTriangle> triangles;
};


void Model::FindAllFromRay(vec3 ray_pos, vec3 ray_dir, std::vector<AABBTriangle>& result) {
    std::vector<uint32_t> results;
    results.reserve(10);
    
    model_aabb->tree.FindIntersection(ray_pos, ray_dir, model_aabb->tree.root, results);
    
    for (auto res : results) {
        result.push_back(model_aabb->triangles[res]);
    }
}




static int total_counter = 0;
static int node_counter = 0;
static int leaf_counter = 0;

//static std::set<uint32_t> lookedat_nodes;

static void DrawAABBNodeChildren (AABBTree::Node* node, const std::vector<AABBTriangle>& triangles, vec3 position, quat rotation) {
    //lookedat_nodes.emplace(node_id);
    
    total_counter++;
    
    if (node->IsLeaf()) {
        //AddLineAABB(node.min, node.max, position, rotation, COLOR_CYAN);
        
        vec3 point1 = position + (rotation * triangles[node->value].point1);
        vec3 point2 = position + (rotation * triangles[node->value].point2);
        vec3 point3 = position + (rotation * triangles[node->value].point3);
        
        AddLine(point1, point2, COLOR_WHITE);
        AddLine(point2, point3, COLOR_WHITE);
        AddLine(point3, point1, COLOR_WHITE);
        
        leaf_counter++;
    } else {
        DrawAABBNodeChildren(node->left, triangles, position, rotation);
        DrawAABBNodeChildren(node->right, triangles, position, rotation);
        
        if (node->parent == nullptr) {
            AddLineAABB(node->min, node->max, position, rotation, COLOR_RED);
        } else {
            AddLineAABB(node->min, node->max, position, rotation, COLOR_PINK);
        }
        
        node_counter++;
    }
}

void Model::DrawAABB(vec3 position, quat rotation) {
    if (!model_aabb) return;
    
    if (name == "kadbusest") return;
    
    //return ;
    
    total_counter = 0;
    node_counter = 0;
    leaf_counter = 0;
    //std::cout << name << " " << model_aabb->tree.GetAABBMin().x << " " << model_aabb->tree.GetAABBMin().y << " " << model_aabb->tree.GetAABBMin().z << " / ";
    //std::cout << model_aabb->tree.GetAABBMax().x << " " << model_aabb->tree.GetAABBMax().y << " " << model_aabb->tree.GetAABBMax().z << std::endl;
    
    //lookedat_nodes.clear();
    
    DrawAABBNodeChildren(model_aabb->tree.root, model_aabb->triangles, position, rotation);
    
    //std::cout << name << std::endl;
    //std::cout << "nodes " << model_aabb->tree.nodes.size() << std::endl;
    //std::cout << "total counter " << total_counter << std::endl;
    //std::cout << "leaf counter " << leaf_counter << std::endl;
    //std::cout << "node counter " << node_counter << std::endl << std::endl;
    
    return;
    /*
    for (uint32_t i = 0; i < model_aabb->tree.nodes.size(); i++) {
        if (lookedat_nodes.contains(i)) continue;
        
        const auto& node = model_aabb->tree.nodes[i];
        
        std::cout << "id: " << i << " parent: " << node.parent << " l: " << model_aabb->tree.nodes[node.parent].left << " r: " << model_aabb->tree.nodes[node.parent].right << std::endl;
    }
    */
    /*for (auto& node : model_aabb->tree.nodes) {
        if (node.right == 0) {
            AddLineAABB(node.min, node.max, position, rotation, COLOR_CYAN);
        }
    }*/
}

static vec3 TriangleAABBMin (vec3 point1, vec3 point2, vec3 point3) {
    return {
        point1.x < point2.x ? (point1.x < point3.x ? point1.x : point3.x) : (point2.x < point3.x ? point2.x : point3.x),
        point1.y < point2.y ? (point1.y < point3.y ? point1.y : point3.y) : (point2.y < point3.y ? point2.y : point3.y),
        point1.z < point2.z ? (point1.z < point3.z ? point1.z : point3.z) : (point2.z < point3.z ? point2.z : point3.z)
    };
}

static vec3 TriangleAABBMax (vec3 point1, vec3 point2, vec3 point3) {
    return {
        point1.x > point2.x ? (point1.x > point3.x ? point1.x : point3.x) : (point2.x > point3.x ? point2.x : point3.x),
        point1.y > point2.y ? (point1.y > point3.y ? point1.y : point3.y) : (point2.y > point3.y ? point2.y : point3.y),
        point1.z > point2.z ? (point1.z > point3.z ? point1.z : point3.z) : (point2.z > point3.z ? point2.z : point3.z)
    };
}

struct TriangleBucket {
    materialtype_t material_type;       // material type for this bucket
    std::vector<uint32_t> materials;    // which materials have already been added to the bucket
    std::vector<Triangle> triangles;    // triangle indices in the bucket
};

struct BucketMapping {
    uint32_t bucket = -1ul;
    uint32_t index_in_bucket = -1ul;
};

static uint32_t PutTriangleInBucket (
    std::vector<TriangleBucket>& buckets,
    std::vector<BucketMapping>& bucket_mappings,
    const std::vector<Material*>& materials,
    uint32_t material_index,
    Triangle triangle
) {
    // check if material is already in a bucket
    if (auto& mapping = bucket_mappings[material_index]; mapping.bucket != -1ul) {
        buckets[mapping.bucket].triangles.push_back(triangle);
        return mapping.index_in_bucket;
    }
    
    // check if there is already a bucket with the same type as material
    for (size_t i = 0; i < buckets.size(); i++) {
        if (buckets[i].material_type == materials[material_index]->GetType()
            && buckets[i].materials.size() < 15
        ) {
            uint32_t bucket_index = buckets[i].materials.size();
            
            buckets[i].materials.push_back(material_index);
            buckets[i].triangles.push_back(triangle);
            
            bucket_mappings[material_index].bucket = i;
            bucket_mappings[material_index].index_in_bucket = bucket_index;
            
            return bucket_index;
        }
    }
    
    // check if allowed to make another bucket
    assert(buckets.size() < 3);
    
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

void Model::LoadFromDisk() {
    assert(status == UNLOADED);
    std::ifstream file;
    char path[200];
    
    std::vector<TriangleBucket> triangle_buckets;
    std::vector<BucketMapping> bucket_mappings;

    // trying to load model as a static model, text mode
    strcpy(path, "data/models/");
    strcat(path, name);
    strcat(path, ".stmdl");

    if (File file (path, MODE_READ); file.is_open()) {
        vertex_format = VERTEX_STATIC;
        StaticModelData* data = new StaticModelData;
        model_data = data;
        model_aabb = new ModelAABB;

        std::cout << "Loading: " << path << std::endl;

        assert(data);

        uint32_t vcount = file.read_uint32();   // number of vertices
        uint32_t tcount = file.read_uint32();   // number of triangles
        uint32_t mcount = file.read_uint32();   // number of materials

        bucket_mappings.resize(mcount);

        model_aabb->triangles.reserve(tcount);
        //model_aabb->tree.nodes.reserve(tcount);

        for (uint32_t i = 0; i < mcount; i++) {
            materials.push_back(Material::Find(file.read_name()));
        }
        
        for (uint32_t i = 0; i < vcount; i++) {
            data->vertices.push_back(StaticModelVertex {
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
            
            uint32_t bucket_index = PutTriangleInBucket(triangle_buckets, bucket_mappings, materials, material_index, index);
            
            const auto& point1 = data->vertices[index.indices.x];
            const auto& point2 = data->vertices[index.indices.y];
            const auto& point3 = data->vertices[index.indices.z];
            
            vec3 triangle_normal = glm::normalize(point1.normal + point2.normal + point3.normal);
            
            uint32_t aabb_triangle_index = model_aabb->triangles.size();
            
            model_aabb->triangles.push_back({point1.co, point2.co, point3.co, triangle_normal, material_index});
            
            vec3 triangle_aabb_min = TriangleAABBMin(point1.co, point2.co, point3.co);
            vec3 triangle_aabb_max = TriangleAABBMax(point1.co, point2.co, point3.co);
            
            model_aabb->tree.InsertLeaf(aabb_triangle_index, triangle_aabb_min, triangle_aabb_max);
            
            /*materialtype_t material_type = materials[material_index]->GetType();
            
            size_t bucket = 0;
            uint32_t material_index_in_bucket = 0;
            
            // try to find the bucket that contains this triangle's material
            for (; bucket < triangle_buckets.size(); bucket++) {
                if (triangle_buckets[bucket].material_type != material_type) continue;
                
                // check if material is in the bucket
                for (size_t i = 0; i < triangle_buckets[bucket].materials.size(); i++) {
                    if (triangle_buckets[bucket].materials[i] == material_index) {
                        material_index_in_bucket = material_index;
                        goto found_bucket;
                    }
                }
                
                // otherwise try to add it to the bucket
                if (triangle_buckets[bucket].materials.size() < 15) {
                    material_index_in_bucket = triangle_buckets[bucket].materials.size();
                    triangle_buckets[bucket].materials.push_back(material_index);
                    goto found_bucket;
                }
            }            
            
            found_bucket:

            if (bucket < triangle_buckets.size()) {
                triangle_buckets[bucket].triangles.push_back(index);
            } else {
                assert(triangle_buckets.size() < 3);
                
                triangle_buckets.push_back(TriangleBucket {
                    .material_type = material_type,
                    .materials = {material_index},
                    .triangles = {index}
                });
            }

            data->vertices[index.indices.x].texture = material_index_in_bucket;
            data->vertices[index.indices.y].texture = material_index_in_bucket;
            data->vertices[index.indices.z].texture = material_index_in_bucket;*/
            
            data->vertices[index.indices.x].texture = bucket_index;
            data->vertices[index.indices.y].texture = bucket_index;
            data->vertices[index.indices.z].texture = bucket_index;
        }

        for (auto& bucket : triangle_buckets) {
            IndexRange range {
                .index_offset = (uint32_t) data->indices.size(),
                .index_length = (uint32_t) bucket.triangles.size(),
                .material_count = (uint32_t) bucket.materials.size(),
                .material_type = bucket.material_type,
            };
            
            for (size_t i = 0; i < bucket.materials.size(); i++) {
                range.materials[i] = bucket.materials[i];
            }
            
            index_ranges.push_back(range);
            data->indices.insert(data->indices.end(), bucket.triangles.begin(), bucket.triangles.end());
        }

        Bone rootbone {
            .name = UID("Root"),
            .parent = (uint32_t) -1,
            .head = {0.0f, 0.0f, 0.0f},
            .tail = {0.0f, 1.0f, 0.0f},
            .roll = 0.0f
        };

        armature.push_back(rootbone);

        status = LOADED;

        for (size_t i = 0; i < materials.size(); i++){
            materials[i]->AddReference();
            Async::LoadDependency(materials[i]);
        }

        aabb_min = model_aabb->tree.GetAABBMin();
        aabb_max = model_aabb->tree.GetAABBMax();

        return;
    }



    // ok, the model isn't static
    // try opening it as a dynamic model

    strcpy(path, "data/models/");
    strcat(path, name);
    strcat(path, ".dymdl");

    if (File file (path, MODE_READ); file.is_open()) {
        vertex_format = VERTEX_DYNAMIC;
        DynamicModelData* data = new DynamicModelData;
        model_data = data;
        model_aabb = new ModelAABB;

        std::cout << "Loading: " << path << std::endl;

        assert(data);
        
        name_t file_version = file.read_name();
        
        if (file_version != UID("DYMDLv1")) {
            std::cout << "Model " << path << " is not using right DYMDLv1 version!" << std::endl;
            std::cout << "Add \"DYMDLv1\" to file and also add bone roll to the bone definitions (0.0 to the end of lines), or reexport." << std::endl;
        }
        
        
        uint32_t vcount = file.read_uint32();   // number of vertices
        uint32_t tcount = file.read_uint32();   // number of triangles
        uint32_t mcount = file.read_uint32();   // number of materials
        uint32_t bcount = file.read_uint32();   // number of bones
        uint32_t gcount = file.read_uint32();   // number of vertex groups

        bucket_mappings.resize(mcount);

        model_aabb->triangles.reserve(tcount);
        //model_aabb->tree.nodes.reserve(tcount);

        for (uint32_t i = 0; i < mcount; i++) {
            materials.push_back(Material::Find(file.read_name()));
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

            data->vertices.push_back(vertex);
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
            
            uint32_t bucket_index = PutTriangleInBucket(triangle_buckets, bucket_mappings, materials, material_index, index);
            
            const auto& point1 = data->vertices[index.indices.x];
            const auto& point2 = data->vertices[index.indices.y];
            const auto& point3 = data->vertices[index.indices.z];
            
            vec3 triangle_normal = glm::normalize(point1.normal + point2.normal + point3.normal);
            
            uint32_t aabb_triangle_index = model_aabb->triangles.size();
            
            model_aabb->triangles.push_back({point1.co, point2.co, point3.co, triangle_normal, material_index});
            
            vec3 triangle_aabb_min = TriangleAABBMin(point1.co, point2.co, point3.co);
            vec3 triangle_aabb_max = TriangleAABBMax(point1.co, point2.co, point3.co);
            
            model_aabb->tree.InsertLeaf(aabb_triangle_index, triangle_aabb_min, triangle_aabb_max);
            
            /*materialtype_t material_type = materials[material_index]->GetType();
            
            size_t bucket = 0;
            
            // try to find the bucket that contains this triangle's material
            for (; bucket < triangle_buckets.size(); bucket++) {
                if (triangle_buckets[bucket].material_type != material_type) continue;
                
                // check if material is in the bucket
                for (size_t i = 0; i < triangle_buckets[bucket].materials.size(); i++) {
                    if (triangle_buckets[bucket].materials[i] == material_index) {
                        goto found_bucket2;
                    }
                }
                
                // otherwise try to add it to the bucket
                if (triangle_buckets[bucket].materials.size() < 15) {
                    triangle_buckets[bucket].materials.push_back(material_index);
                    goto found_bucket2;
                }
            }            
            
            found_bucket2:

            if (bucket < triangle_buckets.size()) {
                triangle_buckets[bucket].triangles.push_back(index);
            } else {
                assert(triangle_buckets.size() < 3);
                
                triangle_buckets.push_back(TriangleBucket {
                    .material_type = material_type,
                    .materials = {material_index},
                    .triangles = {index}
                });
            }

            data->vertices[index.indices.x].texture = material_index;
            data->vertices[index.indices.y].texture = material_index;
            data->vertices[index.indices.z].texture = material_index;*/
            
            
            data->vertices[index.indices.x].texture = bucket_index;
            data->vertices[index.indices.y].texture = bucket_index;
            data->vertices[index.indices.z].texture = bucket_index;
        }
        
        for (uint32_t i = 0; i < bcount; i++) {
            armature.push_back(Bone {
                .name = file.read_name(),
                .parent = file.read_uint32(),
                
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
            
            if (armature[i].name != group) {
                std::cout << "Model " << name << " group " << group << " is not matching bone " << armature[i].name << "!" << std::endl;
            }
            
            data->groups.push_back(group);
        }
        
        for (auto& bucket : triangle_buckets) {
            IndexRange range {
                .index_offset = (uint32_t) data->indices.size(),
                .index_length = (uint32_t) bucket.triangles.size(),
                .material_count = (uint32_t) bucket.materials.size(),
                .material_type = bucket.material_type,
            };
            
            for (size_t i = 0; i < bucket.materials.size(); i++) {
                range.materials[i] = bucket.materials[i];
            }
            
            index_ranges.push_back(range);
            data->indices.insert(data->indices.end(), bucket.triangles.begin(), bucket.triangles.end());
        }

        // push references into material
        status = LOADED;
        for (size_t i = 0; i < materials.size(); i++){
            materials[i]->AddReference();
            Async::LoadDependency(materials[i]);
        }
        
        aabb_min = model_aabb->tree.GetAABBMin();
        aabb_max = model_aabb->tree.GetAABBMax();

        return;
    }

    // ok, so the model isn't static or dynamic
    // we have no other model types, so it means that there actually isn't any usable model

    std::cout << "Model file for " << name << " couldn't be accessed!" << std::endl;

    
    vertex_format = VERTEX_STATIC;

    auto data = MakeNewErrorModel();
    model_data = data;

    materials.push_back(Material::Find("defaulttexture"));

    index_ranges.push_back(IndexRange {
        .index_offset = 0,
        .index_length = (uint32_t) data->indices.size(),
        .material_count = 1,
        .material_type = MATERIAL_TEXTURE,
        .materials = {0}
    });
    
    armature.push_back(Bone {
        .name = UID("Root"),
        .parent = (uint32_t) -1,
        .head = {0.0f, 0.0f, 0.0f},
        .tail = {0.0f, 1.0f, 0.0f},
        .roll = 0.0f
    });
    
    model_aabb = new ModelAABB;

    for (auto index : data->indices) {
        const auto& point1 = data->vertices[index.indices.x];
        const auto& point2 = data->vertices[index.indices.y];
        const auto& point3 = data->vertices[index.indices.z];
        
        vec3 triangle_normal = glm::normalize(point1.normal + point2.normal + point3.normal);
        
        uint32_t aabb_triangle_index = model_aabb->triangles.size();
        
        model_aabb->triangles.push_back({point1.co, point2.co, point3.co, triangle_normal, 0});
        
        vec3 triangle_aabb_min = TriangleAABBMin(point1.co, point2.co, point3.co);
        vec3 triangle_aabb_max = TriangleAABBMax(point1.co, point2.co, point3.co);
        
        model_aabb->tree.InsertLeaf(aabb_triangle_index, triangle_aabb_min, triangle_aabb_max);
    }
    
    aabb_min = model_aabb->tree.GetAABBMin();
    aabb_max = model_aabb->tree.GetAABBMax();
    
    status = LOADED;
    
    load_fail = true;
}

}