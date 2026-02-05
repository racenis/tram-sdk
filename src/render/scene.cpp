// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/scene.h>

#include <components/render.h>
#include <render/model.h>

#include <templates/pool.h>
#include <templates/aabb.h>
#include <templates/octree.h>

/*
 * the AABB tree API is a bit cumbersome to use. it could be significantly
 * improved.
 * 
 * perhaps we should rename the AABB tree to SceneTree??
 */

/**
 * @namespace tram::Render::SceneTree
 * 
 * Scene tree.
 * 
 * Contains the AABBs of all 3D models in the scene. Useful for quickly querying
 * the scene, doing raycasts on the 3D geometry, etc.
 * 
 * Internally uses an AABB tree.
 */
 
 /**
 * @namespace tram::Render::LightTree
 * 
 * Scene light tree.
 * 
 * Contains all of the dynamic lights in the scene. Useful for finding which
 * lights are illuminating a certain 3D model in the scene.
 * 
 * Internally uses an Octree.
 */

namespace tram::Render::AABB {

struct AABBLeaf {
    ReferenceType ref_type;
    AABBTree::node_t leaf;
    
    union {
        RenderComponent* rendercomponent;
    };
};
    
static AABBTree scene_tree;
static Pool<AABBLeaf> scene_tree_leaves("Scene AABB tree leaf pool", 1000);

// this function here take sin position, rotation and scale.
// why tho?? we could just query the component for this!!
// TODO: fix

/// Inserts a RenderComponent leaf into the scene tree.
/// @return Handle to the inserted leaf.
aabbleaf_t InsertLeaf(RenderComponent* component, vec3 position, quat rotation, vec3 scale) {
    vec3 min = component->GetModel()->GetAABBMin() * scale;
    vec3 max = component->GetModel()->GetAABBMax() * scale;
    
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
        min = MergeAABBMin(min, extent);
        max = MergeAABBMax(max, extent);
    }

    min += position;
    max += position;
    
    AABBLeaf* leaf = scene_tree_leaves.AddNew();

    leaf->ref_type = REFERENCE_RENDERCOMPONENT;
    leaf->rendercomponent = component;
    leaf->leaf = scene_tree.InsertLeaf(scene_tree_leaves.index(leaf), min, max);
    
    return leaf;
}

/// Removes a leaf from 
void RemoveLeaf(aabbleaf_t leaf_id) {
    AABBLeaf* leaf = (AABBLeaf*) leaf_id;
    
    scene_tree.RemoveLeaf(leaf->leaf);
    scene_tree_leaves.Remove(leaf);
}

// btw the mask is unused.. why tho??
// we should add the mask!!! somewhere .. i guess .
// maybe we could add an enum in render.h?

/// Peforms a raycast and retrieves the nearest object.
QueryResponse FindNearestFromRay(vec3 ray_pos, vec3 ray_dir, uint32_t mask) {
    std::vector<uint32_t> results;
    results.reserve(10);
    
    scene_tree.FindIntersection(ray_pos, ray_dir, results);
    
    float nearest = INFINITY;
    QueryResponse nearest_result;
    
    nearest_result.data = nullptr;
    
    for (uint32_t i : results) {
        AABBLeaf* leaf = &scene_tree_leaves[i];
        
        std::vector<AABBTriangle> intersected_triangles;
        intersected_triangles.reserve(10);
        
        const mat4 matrix = PositionRotationScaleToMatrix(leaf->rendercomponent->GetLocation(),
                                                          leaf->rendercomponent->GetRotation(),
                                                          leaf->rendercomponent->GetScale());
        const mat4 inv_matrix = glm::inverse(matrix);

        vec3 ray_pos_local = vec3(inv_matrix * vec4(ray_pos, 1.0f));
        vec3 ray_dir_local = glm::normalize(vec3(inv_matrix * vec4(ray_dir, 0.0f)));

        leaf->rendercomponent->GetModel()->FindAllFromRay(ray_pos_local, ray_dir_local, intersected_triangles);
        
        for (auto& tri : intersected_triangles) {
            vec3 intersection = RayTriangleIntersection(ray_pos_local, ray_dir_local, tri.point1, tri.point2, tri.point3);
            
            if (intersection.x == INFINITY) continue;
            
            float intersection_distance = glm::distance(ray_pos, vec3(matrix * vec4(intersection, 1.0f)));
            
            if (nearest > intersection_distance) {
                nearest = intersection_distance;
                
                nearest_result.type = REFERENCE_RENDERCOMPONENT;
                nearest_result.data = leaf->rendercomponent;
                nearest_result.intersection = matrix * vec4(intersection, 1.0f);
                nearest_result.triangle = {
                    matrix * vec4(tri.point1, 1.0f),
                    matrix * vec4(tri.point2, 1.0f),
                    matrix * vec4(tri.point3, 1.0f),
                    matrix * vec4(tri.normal, 0.0f),
                    tri.material
                };
            }
        }
    }
    
    return nearest_result;
}

void FindAllIntersectionsFromAABB(vec3 min, vec3 max, std::function<void(ReferenceType, EntityComponent*)> callback) {
    scene_tree.FindAABBIntersection(min, max, [&](uint32_t key) {
        auto reference_type = scene_tree_leaves[key].ref_type;
        auto reference = scene_tree_leaves[key].rendercomponent;
        
        callback(reference_type, reference);
    });
}


static void DrawAABBNodeChildren(const AABBTree& tree, AABBTree::node_t node) {
    if (tree.IsLeaf(node)) {
        AddLineAABB(tree.GetMin(node), tree.GetMax(node), {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, COLOR_CYAN);
    } else {
        DrawAABBNodeChildren(tree, tree.GetLeft(node));
        DrawAABBNodeChildren(tree, tree.GetRight(node));
        
        if (tree.GetParent(node) == AABBTree::INVALID) {
            AddLineAABB(tree.GetMin(node), tree.GetMax(node), {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, COLOR_RED);
        } else {
            AddLineAABB(tree.GetMin(node), tree.GetMax(node), {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, COLOR_PINK);
        }
    }
}

/// Draws the scene tree for a single frame.
void DebugDrawTree() {
    DrawAABBNodeChildren(scene_tree, scene_tree.GetRoot());
}

}

namespace tram::Render::LightTree {

struct Light {
    light_t handle;
    vec3 position;
    float distance;
    uint32_t tree_handle;
};

Pool<Light> light_list("lighttree pool", 200);
Octree<Light*> light_tree("light tree", 200);;

/// Adds a light to the light tree.
void AddLight(light_t light, vec3 pos, float dist) {
    Light* new_light = light_list.AddNew();
    
    new_light->handle = light;
    new_light->position = pos;
    new_light->distance = dist;
    
    new_light->tree_handle = light_tree.AddLeaf(new_light, pos.x, pos.y, pos.z);
}

/// Finds the 4 nearest lights to a given position.
/// @param position Position from which the lights will be searched.
/// @param dest     Pointer to an array of 4 light_t.
void FindLights(vec3 position, light_t* dest) {
    Light* lights[4] = {nullptr, nullptr, nullptr, nullptr};
    light_tree.FindNearest(lights, position.x, position.y, position.z);
    
    for (int i = 0; i < 4; i++) {
        if (lights[i]) {
            dest[i] = lights[i]->handle;
        } else {
            dest[i] = {};
        }
    }
}

/// Removes a light from the light tree.
void RemoveLight(light_t light) {
    Light* remove_light = nullptr;
    
    for (auto& entry : light_list) {
        if (entry.handle.generic == light.generic) {
            remove_light = &entry;
        }
    }
    
    light_tree.RemoveLeaf(remove_light->tree_handle);
    light_list.Remove(remove_light);
}

}