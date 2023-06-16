#include <render/aabb.h>

#include <components/render.h>
#include <render/model.h>

#include <templates/pool.h>
#include <templates/aabb.h>

namespace tram::Render::AABB {

struct AABBLeaf {
    ReferenceType ref_type;
    AABBTree::Node* leaf;
    
    union {
        RenderComponent* rendercomponent;
    };
};
    
static AABBTree scene_tree;
static Pool<AABBLeaf> scene_tree_leaves("Scene AABB tree leaf pool", 250);

aabbleaf_t InsertLeaf(RenderComponent* component, vec3 position, quat rotation) {
    vec3 min = component->GetModel()->GetAABBMin();
    vec3 max = component->GetModel()->GetAABBMax();
    
    vec3 extents[8] = {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {min.x, max.y, min.z},
        {min.x, min.y, max.z},
        {max.x, max.y, min.z},
        {max.x, min.y, max.z},
        {max.x, max.y, max.z},
        {min.x, max.y, max.z}
    };
    
    for (auto& extent : extents) {
        extent = rotation * extent;
    }
    
    min = extents[0];
    max = extents[0];
    
    for (auto& extent : extents) {
        min = AABBTree::MergeAABBMin(min, extent);
        max = AABBTree::MergeAABBMax(max, extent);
    }

    min += position;
    max += position;
    
    AABBLeaf* leaf = scene_tree_leaves.AddNew();

    leaf->ref_type = REFERENCE_RENDERCOMPONENT;
    leaf->rendercomponent = component;
    leaf->leaf = scene_tree.InsertLeaf(scene_tree_leaves.index(leaf), min, max);
    
    return leaf;
}

void RemoveLeaf(aabbleaf_t leaf_id) {
    AABBLeaf* leaf = (AABBLeaf*) leaf_id;
    
    scene_tree.RemoveLeaf(leaf->leaf);
    scene_tree_leaves.Remove(leaf);
}

QueryResponse FindNearestFromRay(vec3 ray_pos, vec3 ray_dir, uint32_t mask) {
    std::vector<uint32_t> results;
    results.reserve(10);
    
    scene_tree.FindIntersection(ray_pos, ray_dir, scene_tree.root, results);
    
    float nearest = INFINITY;
    QueryResponse nearest_result;
    
    nearest_result.data = nullptr;
    
    for (uint32_t i : results) {
        AABBLeaf* leaf = &scene_tree_leaves[i];
        
        std::vector<AABBTriangle> intersected_triangles;
        intersected_triangles.reserve(10);
        
        auto comp_position = leaf->rendercomponent->GetLocation();
        auto comp_rotation = leaf->rendercomponent->GetRotation();
        
        vec3 ray_pos_local = glm::inverse(comp_rotation) * (ray_pos - comp_position);
        vec3 ray_dir_local = glm::inverse(comp_rotation) * ray_dir;
        
        leaf->rendercomponent->GetModel()->FindAllFromRay(ray_pos_local, ray_dir_local, intersected_triangles);
        
        for (auto& tri : intersected_triangles) {
            vec3 intersection = RayTriangleIntersection(ray_pos_local, ray_dir_local, tri.point1, tri.point2, tri.point3);
            
            if (intersection.x == INFINITY) continue;
            
            float intersection_distance = glm::distance(ray_pos_local, intersection);
            
            if (nearest > intersection_distance) {
                nearest = intersection_distance;
                
                nearest_result.type = REFERENCE_RENDERCOMPONENT;
                nearest_result.data = leaf->rendercomponent;
                nearest_result.intersection = comp_position + (comp_rotation * intersection);
                nearest_result.triangle = {
                    comp_position + (comp_rotation * tri.point1),
                    comp_position + (comp_rotation * tri.point2),
                    comp_position + (comp_rotation * tri.point3),
                    comp_rotation * tri.normal,
                    tri.material
                };
            }
        }
    }
    
    return nearest_result;
}


static void DrawAABBNodeChildren(AABBTree::Node* node) {
    if (node->IsLeaf()) {
        AddLineAABB(node->min, node->max, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, COLOR_CYAN);
    } else {
        DrawAABBNodeChildren(node->left);
        DrawAABBNodeChildren(node->right);
        
        if (node->parent == nullptr) {
            AddLineAABB(node->min, node->max, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, COLOR_RED);
        } else {
            AddLineAABB(node->min, node->max, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, COLOR_PINK);
        }
    }
}

void DebugDrawTree() {
    DrawAABBNodeChildren(scene_tree.root);
}

}