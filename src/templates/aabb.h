// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

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
    ~AABBTree() {
        //RemoveHierarchy(root);
        // TODO: fix 
    }
    
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
    
    void RemoveHierarchy(Node* node) {
        if (node->IsLeaf()) {
            delete node;
        } else {
            RemoveHierarchy(node->left);
            RemoveHierarchy(node->right);
            delete node;
        }
    }
    
    void FindIntersection(vec3 ray_pos, vec3 ray_dir, Node* node, std::vector<uint32_t>& result) const {
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
    
    uint32_t FindIntersection(vec3 ray_pos, vec3 ray_dir, float distance_limit, auto filter) const {
        bool root_intersects = AABBIntersect(ray_pos, ray_dir, root->min, root->max);

        if (!root_intersects) {
            return -1;
        }
        
        float nearest_dist = INFINITY;
        uint32_t nearest_index = -1;
        
        FindIntersectionRecursive(ray_pos, ray_dir, nearest_dist, nearest_index, distance_limit, root, filter);
        
        return nearest_index;
    }
    
    // this should be marked private
    void FindIntersectionRecursive(vec3 ray_pos, vec3 ray_dir, float& nearest_dist, uint32_t& nearest_index, float distance_limit, Node* node, auto filter) const {
        if (node->IsLeaf() && node != root) {
            float leaf_distance = filter(ray_pos, ray_dir, node->value);
            
            if (leaf_distance < nearest_dist) {
                nearest_dist = leaf_distance;
                nearest_index = node->value;
            }
            
            return;
        }

        float left_distance = INFINITY;
        float right_distance = INFINITY;
        
        if (node->left) left_distance = AABBDistance(ray_pos, ray_dir, node->left->min, node->left->max);
        if (node->right) right_distance = AABBDistance(ray_pos, ray_dir, node->right->min, node->right->max);
        
        if (left_distance < right_distance) {
            
            if (left_distance < nearest_dist && left_distance < distance_limit) {
                FindIntersectionRecursive(ray_pos, ray_dir, nearest_dist, nearest_index, distance_limit, node->left, filter);
            }
            
            if (right_distance < nearest_dist && right_distance < distance_limit) {
                FindIntersectionRecursive(ray_pos, ray_dir, nearest_dist, nearest_index, distance_limit, node->right, filter);
            }
            
        } else {
            
            if (right_distance < nearest_dist && right_distance < distance_limit) {
                FindIntersectionRecursive(ray_pos, ray_dir, nearest_dist, nearest_index, distance_limit, node->right, filter);
            }
            
            if (left_distance < nearest_dist && left_distance < distance_limit) {
                FindIntersectionRecursive(ray_pos, ray_dir, nearest_dist, nearest_index, distance_limit, node->left, filter);
            }
            
        }
        
    }
    
    void FindAABBIntersection(vec3 min, vec3 max, auto callback) {
        FindAABBIntersection(root, min, max, callback);
    }
    
    // should be private
    void FindAABBIntersection(Node* node, vec3 min, vec3 max, auto callback) {
        if (node->IsLeaf() && node != root) {
            if (AABBOverlap(min, max, node->min, node->max)) {
                callback(node->value);
            }
            
            return;
        }
        
        if (node->left && AABBOverlap(min, max, node->left->min, node->left->max)) {
            FindAABBIntersection(node->left, min, max, callback);
        }
        
        if (node->right && AABBOverlap(min, max, node->right->min, node->right->max)) {
            FindAABBIntersection(node->right, min, max, callback);
        }
        
    }
    
//private:
    
    void UpdateParentAABB (Node* node) {
        
        //assert(!node->IsLeaf());
        
        if (node->IsLeaf()) return;
        
        int best_rotation = 0;
        float best_volume = INFINITY;
        
        if (node->left && node->right && !node->right->IsLeaf() && node->right->left) {
            vec3 min = node->left->min;
            vec3 max = node->left->max;
            
            if (node->right->right) min = MergeAABBMin(min, node->right->right->min);
            if (node->right->right) max = MergeAABBMax(max, node->right->right->max);
            
            float volume = AABBVolume(min, max);
            
            if (best_volume > volume && AABBVolume(node->right->min, node->right->max) > volume) {
                best_volume = volume;
                best_rotation = 1;
            }
        }
        
        if (node->left && node->right && !node->right->IsLeaf() && node->right->right) {
            vec3 min = node->left->min;
            vec3 max = node->left->max;
            
            if (node->right->left) min = MergeAABBMin(min, node->right->left->min);
            if (node->right->left) max = MergeAABBMax(max, node->right->left->max);
            
            float volume = AABBVolume(min, max);
            
            if (best_volume > volume && AABBVolume(node->right->min, node->right->max) > volume) {
                best_volume = volume;
                best_rotation = 2;
            }
        }
        
        
        if (node->right && node->left && !node->left->IsLeaf() && node->left->left) {
            vec3 min = node->right->min;
            vec3 max = node->right->max;
            
            if (node->left->right) min = MergeAABBMin(min, node->left->right->min);
            if (node->left->right) max = MergeAABBMax(max, node->left->right->max);
            
            float volume = AABBVolume(min, max);
            
            if (best_volume > volume && AABBVolume(node->left->min, node->left->max) > volume) {
                best_volume = volume;
                best_rotation = 3;
            }
        }
        
        if (node->right && node->left && !node->left->IsLeaf() && node->left->right) {
            vec3 min = node->right->min;
            vec3 max = node->right->max;
            
            if (node->left->left) min = MergeAABBMin(min, node->left->left->min);
            if (node->left->left) max = MergeAABBMax(max, node->left->left->max);
            
            float volume = AABBVolume(min, max);
            
            if (best_volume > volume && AABBVolume(node->left->min, node->left->max) > volume) {
                best_volume = volume;
                best_rotation = 4;
            }
        }
        
        
        
        if (best_rotation == 1) {
            std::swap(node->left->parent, node->right->left->parent);
            std::swap(node->left, node->right->left);
            
            vec3 min = node->right->left->min;
            vec3 max = node->right->left->max;
            
            if (node->right->right) min = MergeAABBMin(min, node->right->right->min);
            if (node->right->right) max = MergeAABBMax(max, node->right->right->max);
            
            node->right->min = min;
            node->right->max = max;
            
        }
        
        if (best_rotation == 2) {
            std::swap(node->left->parent, node->right->right->parent);
            std::swap(node->left, node->right->right);
            
            vec3 min = node->right->right->min;
            vec3 max = node->right->right->max;
            
            if (node->right->left) min = MergeAABBMin(min, node->right->left->min);
            if (node->right->left) max = MergeAABBMax(max, node->right->left->max);
            
            node->right->min = min;
            node->right->max = max;
        }
        
        
        if (best_rotation == 3) {
            std::swap(node->right->parent, node->left->left->parent);
            std::swap(node->right, node->left->left);
            
            vec3 min = node->left->left->min;
            vec3 max = node->left->left->max;
            
            if (node->left->right) min = MergeAABBMin(min, node->left->right->min);
            if (node->left->right) max = MergeAABBMax(max, node->left->right->max);
            
            node->left->min = min;
            node->left->max = max;
            
        }
        
        if (best_rotation == 4) {
            std::swap(node->right->parent, node->left->right->parent);
            std::swap(node->right, node->left->right);
            
            vec3 min = node->left->right->min;
            vec3 max = node->left->right->max;
            
            if (node->left->left) min = MergeAABBMin(min, node->left->left->min);
            if (node->left->left) max = MergeAABBMax(max, node->left->left->max);
            
            node->left->min = min;
            node->left->max = max;
        }
        
        
        
        
        
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
    
    static bool AABBOverlap(vec3 min, vec3 max, vec3 other_min, vec3 other_max) {
        return min.x <= other_max.x && max.x >= other_min.x &&
               min.y <= other_max.y && max.y >= other_min.y &&
               min.z <= other_max.z && max.z >= other_min.z;
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
    
    static float AABBDistance(vec3 ray_pos, vec3 ray_dir, vec3 min, vec3 max) {
        vec3 t1 = (min - ray_pos) / ray_dir; // what happens if ray_dir == 0.0f?
        vec3 t2 = (max - ray_pos) / ray_dir; // TODO: check
        
        vec3 t1min = glm::min(t1, t2);
        vec3 t2max = glm::max(t1, t2);
        
        float tnear = glm::max(glm::max(t1min.x, t1min.y), t1min.z);
        float tfar = glm::min(glm::min(t2max.x, t2max.y), t2max.z);
        
        return tfar >= tnear ? tnear : INFINITY;
    }
    
    int FindDepth() {
        int depth = 0;
        FindDepthRecursive(root, 1, depth);
        return depth;
    }
    
    void FindDepthRecursive(Node* node, int current, int& largest) {
        if (current > largest) largest = current;
        
        if (node->IsLeaf()) return;
        
        if (node->left) {
            FindDepthRecursive(node->left, current + 1, largest);
        }
        
        if (node->right) {
            FindDepthRecursive(node->right, current + 1, largest);
        }
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
    
    Node* root = new Node {nullptr, nullptr, nullptr, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
};

}

#endif // TRAM_SDK_TEMPLATES_AABB_H