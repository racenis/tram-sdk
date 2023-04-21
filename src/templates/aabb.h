// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_TEMPLATES_AABB_H
#define TRAM_SDK_TEMPLATES_AABB_H

#include <framework/math.h>
#include <framework/logging.h>

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
        
        auto& root = nodes[0];
        auto& leaf = nodes[leaf_index];
        
        //leaf = {value, 0, 0, min - vec3(0.1f, 0.1f, 0.1f), max + vec3(0.1f, 0.1f, 0.1f)};
        leaf = {value, 0, 2000, min, max};
        
        // check if root node has free spots
        if (root.left == 0) {
            root.left = leaf_index;
            leaf.parent = 0;
            
            std::cout << "inserted root left" << std::endl;
            
            if (root.right) {
                UpdateParentAABB(0);
            } else {
                root.min = leaf.min;// - vec3(0.1f, 0.1f, 0.1f);
                root.max = leaf.max;// + vec3(0.1f, 0.1f, 0.1f);
            }
            
            assert(leaf.parent != 2000);
            
            return leaf_index;
        }
        
        if (root.right == 0) {
            root.right = leaf_index;
            leaf.parent = 0;
            
            std::cout << "inserted root right" << std::endl;
            
            if (root.left) {
                UpdateParentAABB(0);
            } else {
                root.min = leaf.min;// - vec3(0.1f, 0.1f, 0.1f);
                root.max = leaf.max;// + vec3(0.1f, 0.1f, 0.1f);
            }
            
            assert(leaf.parent != 2000);
            
            return leaf_index;
        }
        
        aabb_t sibling_index = FindSibling(leaf_index, 0);
        aabb_t new_parent_index =  AllocateNode();
        aabb_t old_parent_index = nodes[sibling_index].parent;
        
        assert(old_parent_index != 2000);
        
        auto& sibling = nodes[sibling_index];
        auto& new_parent = nodes[new_parent_index];
        auto& old_parent = nodes[old_parent_index];
        
        if (old_parent.left == sibling_index) {
            old_parent.left = new_parent_index;
        } else if (old_parent.right == sibling_index) {
            old_parent.right = new_parent_index;
        } else {
            old_parent.right = new_parent_index;
            std::cout << "fucky wucky " << sibling_index << " " << sibling.parent << std::endl;
            std::cout << sibling.left << " " << sibling.right << " " << sibling.min.x << " " << sibling.max.x << std::endl;
        }
        
        new_parent.left = sibling_index;
        new_parent.right = leaf_index;
        new_parent.parent = old_parent_index;
        
        sibling.parent = new_parent_index;
        leaf.parent = new_parent_index;
        
        UpdateParentAABB(new_parent_index);
        
        assert(leaf.parent != 2000);
        
        return leaf_index;
    }
    
    void RemoveLeaf(aabb_t node_index) {
        aabb_t parent_index = nodes[node_index].parent;
        
        //auto& node = nodes[node_index];
        auto& parent = nodes[node_index];
        
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
        
        if (node != 0) {
            assert(node_self.parent != 2000);
            UpdateParentAABB(node_self.parent);
        }
    }
    
    // creates a tree node
    aabb_t AllocateNode () {
        aabb_t node = 0;
        
        if (lastfree_node == 0) {
            node = nodes.size();
            nodes.push_back(Node());
        } else {
            assert(false);
            
            node = lastfree_node;
            lastfree_node = nodes[lastfree_node].parent;
            
            nodes[node] = Node();
        }
        
        if (node == 0) std::cout << "this should not happend" << std::endl;
        
        return node;
    }
    
    // marks a tree node as free
    void FreeNode (aabb_t node) {
        nodes[node].parent = lastfree_node;
        lastfree_node = node;
    }
    
    // searches the children of search_node to find a sibling for target_node
    aabb_t FindSibling (aabb_t target_node, aabb_t search_node) {
        const auto& s_node = nodes[search_node];
        const auto& t_node = nodes[target_node];
        const auto& l_node = nodes[s_node.left];
        const auto& r_node = nodes[s_node.right];
        
        
        if (s_node.IsLeaf()) {
            return search_node;
        }
        
        vec3 left_merge_min = MergeAABBMin(t_node.min, l_node.min);
        vec3 left_merge_max = MergeAABBMax(t_node.max, l_node.max);
        
        vec3 right_merge_min = MergeAABBMin(t_node.min, r_node.min);
        vec3 right_merge_max = MergeAABBMax(t_node.max, r_node.max);
        
        float left_merge_volume = AABBVolume(left_merge_min, left_merge_max);
        float right_merge_volume = AABBVolume(right_merge_min, right_merge_max);
        
        //float left_merge_volume = AABBSurface(left_merge_min, left_merge_max);
        //float right_merge_volume = AABBSurface(right_merge_min, right_merge_max);
        
        if (left_merge_volume < right_merge_volume) {
            return FindSibling(target_node, s_node.left);
        } else {
            return FindSibling(target_node, s_node.right);
        }
    }
    
    static vec3 MergeAABBMin (vec3 a, vec3 b) {
        return vec3 {
            a.x < b.x ? a.x : b.x,
            a.y < b.y ? a.y : b.y,
            a.z < b.z ? a.z : b.z
        } - vec3 {0.1f, 0.1f, 0.1f};
    }
    
    static vec3 MergeAABBMax (vec3 a, vec3 b) {
        return vec3 {
            a.x > b.x ? a.x : b.x,
            a.y > b.y ? a.y : b.y,
            a.z > b.z ? a.z : b.z
        } + vec3 {0.1f, 0.1f, 0.1f};
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
        aabb_t parent = 2001;
        
        vec3 min;
        vec3 max;
    };
    
    aabb_t lastfree_node = 0;
    aabb_t lastfree_value = 0;
    
    std::vector<Node> nodes = {{0, 0, ~0ul, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}};
};

}

#endif // TRAM_SDK_TEMPLATES_AABB_H