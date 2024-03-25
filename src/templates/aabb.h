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

class AABBTree {
public:
    AABBTree() {}
    ~AABBTree() {}
    
    vec3 GetAABBMin() { return root->min; }
    vec3 GetAABBMax() { return root->max; }
    
    struct Node;
    
    Node* InsertLeaf (uint32_t value, vec3 min, vec3 max) {
        Node* new_node = new Node;
        
        new_node->value = value;
        
        new_node->min = min;
        new_node->max = max;
        
        if (root->left == nullptr) {
            root->left = new_node;
            new_node->parent = root;
            
            if (root->right) {
                UpdateParentAABB(root);
            } else {
                root->min = new_node->min;
                root->max = new_node->max;
            }
            
            return new_node;
        }
        
        if (root->right == nullptr) {
            root->right = new_node;
            new_node->parent = root;
            
            if (root->left) {
                UpdateParentAABB(root);
            } else {
                root->min = new_node->min;
                root->max = new_node->max;
            }
            
            return new_node;
        }
        
        Node* sibling = FindSibling(min, max, root);
        Node* sibling_parent = sibling->parent;
        Node* new_parent = new Node;
        
        if (sibling_parent->left == sibling) {
            sibling_parent->left = new_parent;
        } else if (sibling_parent->right == sibling) {
            sibling_parent->right = new_parent;
        } else {
            assert(false);
        }
        
        new_parent->parent = sibling_parent;
        
        new_parent->left = new_node;
        new_parent->right = sibling;
        
        sibling->parent = new_parent;
        new_node->parent = new_parent;
        
        UpdateParentAABB(new_parent);
        
        ValidateTree(root);
        
        return new_node;
    }
    
    void RemoveLeaf(Node* node) {
        assert(node);
        
        Node* parent = node->parent;
        Node* sibling = parent->left == node ? parent->right : parent->left;
        
        if (parent->left != node && parent->right != node) {
            assert(false);
        }
        
        if (parent == root) {
            if (parent->left == node) {
                parent->left = nullptr;
                
                if (sibling) {
                    parent->min = sibling->min;
                    parent->max = sibling->max;
                }
            } else {
                parent->right = nullptr;
                
                if (sibling) {
                    parent->min = sibling->min;
                    parent->max = sibling->max;
                }
            }
            
            delete node;
            
            ValidateTree(root);
            
            return;
        }
        
        Node* grandparent = parent->parent;
        
        if (grandparent->left == parent) {
            grandparent->left = sibling;
        } else {
            grandparent->right = sibling;
        }
        
        sibling->parent = grandparent;
        
        UpdateParentAABB(grandparent);
        
        delete node;
        delete parent;
        
        ValidateTree(root);
    }
    
    void FindIntersection (vec3 ray_pos, vec3 ray_dir, Node* node, std::vector<uint32_t>& result) {
        bool is_node_intersect = AABBIntersect(ray_pos, ray_dir, node->min, node->max);
        
        if (is_node_intersect) {
            if (node->IsLeaf() && node != root) {
                result.push_back(node->value);
            } else {
                if (node->left) FindIntersection (ray_pos, ray_dir, node->left, result);
                if (node->right) FindIntersection (ray_pos, ray_dir, node->right, result);
            }
        }
    }
    
//private:
    
    void UpdateParentAABB (Node* node) {
        assert(!node->IsLeaf());
        
        Node* left_child = node->left;
        Node* right_child = node->right;
        
        if (!left_child || !right_child) {
            assert(node == root);
            
            if (!left_child && right_child) {
                node->min = right_child->min;
                node->max = right_child->max;
            } else if (left_child && !right_child) {
                node->min = left_child->min;
                node->max = left_child->max;
            } else {
                node->min = {0.0f, 0.0f, 0.0f};
                node->max = {0.0f, 0.0f, 0.0f};
            }
            
            return;
        }
        
        
        node->min = MergeAABBMin(left_child->min, right_child->min);
        node->max = MergeAABBMax(left_child->max, right_child->max);
        
        if (node->parent != nullptr) {
            UpdateParentAABB(node->parent);
        }
        
        assert(node->parent != node);
    }
    
    // searches the children of search_node to find a sibling for target_node
    Node* FindSibling (vec3 min, vec3 max, Node* node) {
        assert(node);

        if (node->IsLeaf()) {
            return node;
        }
        
        if (node->left == nullptr) {
            assert(false);
        }
        assert(node->left);
        assert(node->right);
        
        vec3 left_merge_min = MergeAABBMin(min, node->left->min);
        vec3 left_merge_max = MergeAABBMax(max, node->left->max);
        
        vec3 right_merge_min = MergeAABBMin(min, node->right->min);
        vec3 right_merge_max = MergeAABBMax(max, node->right->max);
        
        float left_merge_volume = AABBVolume(left_merge_min, left_merge_max);
        float right_merge_volume = AABBVolume(right_merge_min, right_merge_max);
        
        //float left_merge_volume = AABBSurface(left_merge_min, left_merge_max);
        //float right_merge_volume = AABBSurface(right_merge_min, right_merge_max);
        
        if (left_merge_volume < right_merge_volume) {
            return FindSibling(min, max, node->left);
        } else {
            return FindSibling(min, max, node->right);
        }
    }
    
    
    void ValidateTree (Node* node) {
        return;
        if (root->parent != nullptr) {
            //if (((Node*)0)->IsLeaf()) assert(false);
        }
        
        return;
        ValidateTree (node, 0);
    }
    void ValidateTree (Node* node, size_t num) {
        assert(node);
        assert((long long)node > 100);
        
        if (num > 400) {
            //if (((Node*)0)->IsLeaf()) assert(false);
        }
        
        if (node->IsLeaf() && node != root) {
            //assert(node->value < 40000);
        } else {
            if (node != root || (node == root && node->left))ValidateTree(node->left, num+1);
            if (node != root || (node == root && node->right))ValidateTree(node->right, num+1);
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
    
    static bool AABBIntersect (vec3 ray_pos, vec3 ray_dir, vec3 min, vec3 max) {
        vec3 t1 = (min - ray_pos) / ray_dir; // what happens if ray_dir == 0.0f?
        vec3 t2 = (max - ray_pos) / ray_dir; // TODO: check
        
        vec3 t1min = glm::min(t1, t2);
        vec3 t2max = glm::max(t1, t2);
        
        float tnear = glm::max(glm::max(t1min.x, t1min.y), t1min.z);
        float tfar = glm::min(glm::min(t2max.x, t2max.y), t2max.z);
        
        return tfar >= tnear;
    }
    
    struct Node {
        bool IsLeaf () const { return right == 0; }
        
        void Print () const { std::cout << " l: " << left << " r: " << right << " p: " << parent << std::endl; }
        
        union {
            Node* left = nullptr;
            uint32_t value;
        };
        
        Node* right = nullptr;
        Node* parent = nullptr;
        
        vec3 min;
        vec3 max;
    };
    
    size_t validnum;
    
    Node* root = new Node {nullptr, nullptr, nullptr, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
};

}

#endif // TRAM_SDK_TEMPLATES_AABB_H