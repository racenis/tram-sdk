// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_TEMPLATES_AABB_H
#define TRAM_SDK_TEMPLATES_AABB_H

#include <framework/math.h>

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
        
        leaf = {value, 0, 0, min, max};
        
        // check if root node has free spots
        if (root.left == 0) {
            root.left = leaf_index;
            leaf.parent = 0;
            
            if (root.right) {
                UpdateParentAABB(0);
            } else {
                root.min = leaf.min;
                root.max = leaf.max;
            }
            
            return leaf_index;
        }
        
        if (root.right == 0) {
            root.right = leaf_index;
            leaf.parent = 0;
            
            if (root.left) {
                UpdateParentAABB(0);
            } else {
                root.min = leaf.min;
                root.max = leaf.max;
            }
            
            return leaf_index;
        }
        
        aabb_t sibling_index = FindSibling(leaf_index, 0);
        aabb_t new_parent_index =  AllocateNode();
        aabb_t old_parent_index = nodes[sibling_index].parent;
        
        auto& sibling = nodes[sibling_index];
        auto& new_parent = nodes[new_parent_index];
        auto& old_parent = nodes[old_parent_index];
        
        if (old_parent.left == sibling_index) {
            old_parent.left = new_parent_index;
        } else {
            old_parent.right = new_parent_index;
        }
        
        new_parent.left = sibling_index;
        new_parent.right = leaf_index;
        new_parent.parent = old_parent_index;
        
        sibling.parent = new_parent_index;
        leaf.parent = new_parent_index;
        
        UpdateParentAABB(new_parent_index);
        
        return leaf_index;
    }
    
    void RemoveLeaf(aabb_t node_index) {
        aabb_t parent_index = nodes[node_index].parent;
        
        auto& node = nodes[node_index];
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
        auto& right_child = nodes[node_self.left];
        
        node_self.min = MergeAABBMin(left_child.min, right_child.min);
        node_self.max = MergeAABBMax(left_child.max, right_child.max);
        
        if (node != 0) {
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
            node = lastfree_node;
            lastfree_node = nodes[lastfree_node].parent;
            
            nodes[node] = Node();
        }
        
        return node;
    }
    
    // marks a tree node as free
    void FreeNode (aabb_t node) {
        nodes[node].parent = lastfree_node;
        lastfree_node = node;
    }
    
    // searches the children of search_node to find a sibling for target_node
    aabb_t FindSibling (aabb_t target_node, aabb_t search_node) {
        auto& s_node = nodes[search_node];
        auto& t_node = nodes[target_node];
        auto& l_node = nodes[s_node.left];
        auto& r_node = nodes[s_node.right];
        
        
        if (s_node.IsLeaf()) {
            return search_node;
        }
        
        vec3 left_merge_min = MergeAABBMin(s_node.min, l_node.min);
        vec3 left_merge_max = MergeAABBMax(s_node.max, l_node.max);
        
        vec3 right_merge_min = MergeAABBMin(s_node.min, r_node.min);
        vec3 right_merge_max = MergeAABBMax(s_node.max, r_node.max);
        
        float left_merge_volume = AABBVolume(left_merge_min, left_merge_max);
        float right_merge_volume = AABBVolume(right_merge_min, right_merge_max);
        
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
        };
    }
    
    static vec3 MergeAABBMax (vec3 a, vec3 b) {
        return vec3 {
            a.x > b.x ? a.x : b.x,
            a.y > b.y ? a.y : b.y,
            a.z > b.z ? a.z : b.z
        };
    }
    
    static float AABBVolume (vec3 min, vec3 max) {
        return (max.x - min.x) * (max.y - min.y) * (max.z - min.z);
    }
    
    struct Node {
        bool IsLeaf () { return right == 0; }
        
        union {
            aabb_t left = 0;
            aabb_t value;
        };
        
        aabb_t right = 0;
        aabb_t parent = 0;
        
        vec3 min;
        vec3 max;
    };
    
    aabb_t lastfree_node = 0;
    aabb_t lastfree_value = 0;
    
    std::vector<Node> nodes = {{0, 0, ~0ul, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}};
};

}

#endif // TRAM_SDK_TEMPLATES_AABB_H