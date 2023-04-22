// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_TEMPLATES_AABB_H
#define TRAM_SDK_TEMPLATES_AABB_H

#include <framework/math.h>
#include <framework/logging.h>
#include <framework/core.h>

#include <iostream>

#include <vector>

namespace tram {

typedef uint32_t aabb_t;

// TODO: make the aabb_t be definable from a template!!

class AABBTree {
public:
    AABBTree() {}
    ~AABBTree() {}
    
    vec3 GetAABBMin() { return nodes[0].min; }
    vec3 GetAABBMax() { return nodes[0].max; }
    
    aabb_t InsertLeaf (aabb_t value, vec3 min, vec3 max) {
        aabb_t leaf_index = AllocateNode();
        
        auto& leaf = nodes[leaf_index];
        auto& root = nodes[0];
        
        leaf.value = value;
        leaf.right = 0;
        leaf.min = min;
        leaf.max = max;
        
        if (root.left == 0 || root.right == 0) {
            if (root.left == 0) {
                root.left = leaf_index;
                leaf.parent = 0;
                
                if (root.right == 0) {
                    return leaf_index;
                }
            } else {
                root.right = leaf_index;
                leaf.parent = 0;
                
                if (root.left == 0) {
                    return leaf_index;
                }
            }
            
            UpdateParentAABB(0);
            
            return leaf_index;
        }
        
        aabb_t sibling_index = FindSibling(min, max, 0);
        aabb_t new_sibling_index = AllocateNode();
        
        auto& sibling = nodes[sibling_index];
        auto& new_sibling = nodes[new_sibling_index];
        
        new_sibling = sibling;
        
        sibling.left = leaf_index;
        sibling.right = new_sibling_index;
 
        leaf.parent = sibling_index;
        new_sibling.parent = sibling_index;
        
        UpdateParentAABB(sibling_index);
        
        return leaf_index;
    }
    
    void RemoveLeaf(aabb_t node_index) {
        /*if (!nodes[node_index].IsLeaf()) {
            std::cout << node_index << std::endl;
            abort();
        }*/
        
        
        
        
        
        return;
        aabb_t parent_index = nodes[node_index].parent;
        
        auto& parent = nodes[parent_index];
        
        aabb_t sibling_index = parent.left == node_index ? parent.right : parent.left;
        
        auto& sibling = nodes[sibling_index];
        
        // check if parent is root
        if (parent_index == 0) {
            if (parent.left == node_index) {
                parent.left = 0;
                
                if (parent.right) {
                    parent.min = sibling.min;
                    parent.max = sibling.max;
                }
            } else {
                parent.right = 0;
                
                if (parent.left) {
                    parent.min = sibling.min;
                    parent.max = sibling.max;
                }
            }
            
            FreeNode(node_index);
            return;
        }
        
        aabb_t grandparent_index = parent.parent;
        
        auto& grandparent = nodes[grandparent_index];
        
        if (grandparent.left == parent_index) {
            grandparent.left = sibling_index;
        } else {
            grandparent.right = sibling_index;
        }
        
        sibling.parent = grandparent_index;
        
        UpdateParentAABB(grandparent_index);
        
        FreeNode(parent_index);
        FreeNode(node_index);
    }
    
//private:
    
    void UpdateParentAABB (aabb_t node) {
        auto& node_self = nodes[node];
        auto& left_child = nodes[node_self.left];
        auto& right_child = nodes[node_self.right];
        
        node_self.min = MergeAABBMin(left_child.min, right_child.min);
        node_self.max = MergeAABBMax(left_child.max, right_child.max);
        
        if (GetTick() > 60 && false) {
             std::cout << "node " << node << " " << (node != 0) << std::endl;
             std::cout << node_self.left << " " << node_self.right << " " << node_self.parent << std::endl;
             
             
        }
        
        if (node != 0) {
            UpdateParentAABB(node_self.parent);
        }
        
        assert(node_self.parent != node);
    }
    
    // creates a tree node
    aabb_t AllocateNode () {
        if (freenodes.size()) {
            aabb_t node = freenodes.back();
            
            freenodes.pop_back();
            
            if (node == 0) std::cout << "this should not happend" << std::endl;
            
            return node;
        } else {
            aabb_t node = nodes.size();
            nodes.push_back(Node());
            
            if (node == 0) std::cout << "this should not happend" << std::endl;
            
            return node;
        }
    }
    
    // marks a tree node as free
    void FreeNode (aabb_t node) {
        freenodes.push_back(node);
        nodes[node] = Node();
    }
    
    // searches the children of search_node to find a sibling for target_node
    aabb_t FindSibling (vec3 min, vec3 max, aabb_t search_node) {
        const auto& s_node = nodes[search_node];
        const auto& l_node = nodes[s_node.left];
        const auto& r_node = nodes[s_node.right];
        
        if (s_node.IsLeaf()) {
            return search_node;
        }
        
        vec3 left_merge_min = MergeAABBMin(min, l_node.min);
        vec3 left_merge_max = MergeAABBMax(max, l_node.max);
        
        vec3 right_merge_min = MergeAABBMin(min, r_node.min);
        vec3 right_merge_max = MergeAABBMax(max, r_node.max);
        
        float left_merge_volume = AABBVolume(left_merge_min, left_merge_max);
        float right_merge_volume = AABBVolume(right_merge_min, right_merge_max);
        
        //float left_merge_volume = AABBSurface(left_merge_min, left_merge_max);
        //float right_merge_volume = AABBSurface(right_merge_min, right_merge_max);
        
        if (left_merge_volume < right_merge_volume) {
            return FindSibling(min, max, s_node.left);
        } else {
            return FindSibling(min, max, s_node.right);
        }
    }
    
    static vec3 MergeAABBMin (vec3 a, vec3 b) {
        return vec3 {
            a.x < b.x ? a.x : b.x,
            a.y < b.y ? a.y : b.y,
            a.z < b.z ? a.z : b.z
        };// - vec3 {0.1f, 0.1f, 0.1f};
    }
    
    static vec3 MergeAABBMax (vec3 a, vec3 b) {
        return vec3 {
            a.x > b.x ? a.x : b.x,
            a.y > b.y ? a.y : b.y,
            a.z > b.z ? a.z : b.z
        };// + vec3 {0.1f, 0.1f, 0.1f};
    }
    
    static float AABBVolume (vec3 min, vec3 max) {
        return (max.x - min.x) * (max.y - min.y) * (max.z - min.z);
    }
    
    static float AABBSurface (vec3 min, vec3 max) {
        float x = max.x - min.x;
        float y = max.y - min.y;
        float z = max.z - min.z;
        
        assert(max.x >= min.x);
        assert(max.y >= min.y);
        assert(max.z >= min.z);
        
        return 2 * ((x * y) + (x * z) + (y * z));
    }
    
    struct Node {
        bool IsLeaf () const { return right == 0; }
        
        union {
            aabb_t left = 0;
            aabb_t value;
        };
        
        aabb_t right = 0;
        aabb_t parent = 0;
        
        vec3 min;
        vec3 max;
    };
    
    std::vector<aabb_t> freenodes;
    
    std::vector<Node> nodes = {{0, 0, ~0ul, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}};
};

}

#endif // TRAM_SDK_TEMPLATES_AABB_H