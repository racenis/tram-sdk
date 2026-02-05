// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_TEMPLATES_AABB_H
#define TRAM_SDK_TEMPLATES_AABB_H

#include <framework/math.h>
#include <framework/logging.h>
#include <framework/core.h>

#include <iostream>

#include <vector>

namespace tram {

// TODO: increase speed even more!!
//
// Currently we are storing nodes as structs, connected as a linked list via pointers.
// It would be more efficient to store indices into an array instead.
// Unfortunately such a refactor is a massive PITA.
//
// Plan:
// 1. X     separate 'public' and 'private' functions
// 2. X     alias node_t to void* and replace Node* with it
// 3. X     since Node is now opaque, add small static inline getter/setter methods
// 4. start moving away from directly accessing Node struct to static inline methods on the AABBTree class
// 5. finish moving away
// 6. now Node has been completely abstracted and node_t can be fully opaque
// 7. switch node_t to uint32_t mapping it to a Node*
// 8. replace new/deletes with an array
// 9. devise improved packing schemes for better performance
    
class AABBTree {
public:
    AABBTree() {
        this->root = (Node*)MakeNode();
    }
    ~AABBTree() {
        //RemoveHierarchy(root);
        // TODO: fix 
    }
    
    typedef void* node_t;
    
    node_t INVALID = nullptr;
    
    vec3 GetAABBMin() { return GetMin(root); }
    vec3 GetAABBMax() { return GetMax(root); }
    
    struct Node;
    
    node_t InsertLeaf (uint32_t value, vec3 min, vec3 max) {
        node_t new_node = MakeNode();
        
        SetValue(new_node, value);
        
        SetMin(new_node, min);
        SetMax(new_node, max);
        
        // if root doesn't have both children, we want to fill them in asap,
        // since it makes next operations significantly simpler
        if (GetLeft(root) == INVALID) {
            SetLeft(root, new_node);
            SetParent(new_node, root);
            
            if (GetRight(root) != INVALID) {
                UpdateParentAABB(root);
            } else {
                SetMin(root, GetMin(new_node));
                SetMax(root, GetMax(new_node));
            }
            
            return new_node;
        }
        
        if (GetRight(root) == INVALID) {
            SetRight(root, new_node);
            SetParent(new_node, root);

            if (GetLeft(root) != INVALID) {
                UpdateParentAABB(root);
            } else {
                SetMin(root, GetMin(new_node));
                SetMax(root, GetMax(new_node));
            }
            
            return new_node;
        }
        
        node_t sibling = FindSibling(min, max, root);
        node_t sibling_parent = GetParent(sibling);
        node_t new_parent = MakeNode();
        
        if (GetLeft(sibling_parent) == sibling) {
            SetLeft(sibling_parent, new_parent);
        } else if (GetRight(sibling_parent) == sibling) {
            SetRight(sibling_parent, new_parent);
        } else {
            assert(false);
        }
        
        SetParent(new_parent, sibling_parent);

        SetLeft(new_parent, new_node);
        SetRight(new_parent, sibling);

        SetParent(sibling, new_parent);
        SetParent(new_node, new_parent);
        
        UpdateParentAABB(new_parent);
        
        ValidateTree(root);
        
        return new_node;
    }
    
    void RemoveLeaf(node_t node) {
        assert(node != INVALID);
        
        node_t parent = GetParent(node);
        node_t sibling = GetLeft(parent) == node ? GetRight(parent) : GetLeft(parent);
        
        if (GetLeft(parent) != node && GetRight(parent) != node) {
            assert(false);
        }
        
        if (parent == root) {
            if (GetLeft(parent) == node) {
                SetLeft(parent, INVALID);
                
                if (sibling != INVALID) {
                    SetMin(parent, GetMin(sibling));
                    SetMax(parent, GetMax(sibling));
                }
            } else {
                SetRight(parent, INVALID);
                
                if (sibling != INVALID) {
                    SetMin(parent, GetMin(sibling));
                    SetMax(parent, GetMax(sibling));
                }
            }
            
            YeetNode(node);
            
            ValidateTree(root);
            
            return;
        }
        
        node_t grandparent = GetParent(parent);
        
        if (GetLeft(grandparent) == parent) {
            SetLeft(grandparent, sibling);
        } else {
            SetRight(grandparent, sibling);
        }
        
        assert(sibling != INVALID);
        SetParent(sibling, grandparent);
        
        UpdateParentAABB(grandparent);
        
        YeetNode(node);
        YeetNode(parent);
        
        ValidateTree(root);
    }
    
    void FindIntersection(vec3 ray_pos, vec3 ray_dir, node_t node, std::vector<uint32_t>& result) const {
        bool is_node_intersect = AABBIntersect(ray_pos, ray_dir, GetMin(node), GetMax(node));
        
        if (is_node_intersect) {
            if (IsLeaf(node) && node != root) {
                result.push_back(GetValue(node));
            } else {
                if (GetLeft(node) != INVALID) FindIntersection (ray_pos, ray_dir, GetLeft(node), result);
                if (GetRight(node) != INVALID) FindIntersection (ray_pos, ray_dir, GetRight(node), result);
            }
        }
    }
    
    uint32_t FindIntersection(vec3 ray_pos, vec3 ray_dir, float distance_limit, auto filter) const {
        bool root_intersects = AABBIntersect(ray_pos, ray_dir, GetMin(root), GetMax(root));

        if (!root_intersects) {
            return -1;
        }
        
        float nearest_dist = INFINITY;
        uint32_t nearest_index = -1;
        
        FindIntersectionRecursive(ray_pos, ray_dir, nearest_dist, nearest_index, distance_limit, root, filter);
        
        return nearest_index;
    }
    
    void FindAABBIntersection(vec3 min, vec3 max, auto callback) {
        FindAABBIntersection(root, min, max, callback);
    }
    
    int FindDepth() {
        int depth = 0;
        FindDepthRecursive(root, 1, depth);
        return depth;
    }
    
private:
    
    // do we need this even?
    void RemoveHierarchy(node_t node) {
        if (IsLeaf(node)) {
            YeetNode(node);
        } else {
            RemoveHierarchy(GetLeft(node));
            RemoveHierarchy(GetRight(node));
            YeetNode(node);
        }
    }
    
    void FindIntersectionRecursive(vec3 ray_pos, vec3 ray_dir, float& nearest_dist, uint32_t& nearest_index, float distance_limit, node_t opaque_node, auto filter) const {
        Node* node = (Node*)opaque_node;
        
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
    
    void FindAABBIntersection(node_t opaque_node, vec3 min, vec3 max, auto callback) {
        Node* node = (Node*)opaque_node;
        
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
    
    void UpdateParentAABB (node_t opaque_node) {
        Node* node = (Node*)opaque_node;
        
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
    node_t FindSibling (vec3 min, vec3 max, node_t opaque_node) {
        Node* node = (Node*)opaque_node;
        
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
    
    
    void ValidateTree (node_t opaque_node) {
        Node* node = (Node*)opaque_node;
        
        return;
        if (root->parent != nullptr) {
            //if (((Node*)0)->IsLeaf()) assert(false);
        }
        
        return;
        ValidateTree (node, 0);
    }
    void ValidateTree (node_t opaque_node, size_t num) {
        Node* node = (Node*)opaque_node;
        
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
    
    void FindDepthRecursive(node_t opaque_node, int current, int& largest) {
        Node* node = (Node*)opaque_node;
        
        if (current > largest) largest = current;
        
        if (node->IsLeaf()) return;
        
        if (node->left) {
            FindDepthRecursive(node->left, current + 1, largest);
        }
        
        if (node->right) {
            FindDepthRecursive(node->right, current + 1, largest);
        }
    }

public:
    inline node_t MakeNode() {
        return new Node;
    }
    
    inline void YeetNode(node_t node) {
        delete (Node*)node;
    }
    
    inline node_t GetLeft(node_t node) const {
        return ((Node*)node)->left;
    }
    
    inline node_t GetRight(node_t node) const {
        return ((Node*)node)->right;
    }
    
    inline node_t GetParent(node_t node) const {
        return ((Node*)node)->parent;
    }
    
    inline void SetLeft(node_t node, node_t value) {
        ((Node*)node)->left = (Node*)value;
    }
    
    inline void SetRight(node_t node, node_t value) {
        ((Node*)node)->right = (Node*)value;
    }
    
    inline void SetParent(node_t node, node_t value) {
        ((Node*)node)->parent = (Node*)value;
    }
    
    
    inline vec3 GetMin(node_t node) const {
        return ((Node*)node)->min;
    }
    
    inline vec3 GetMax(node_t node) const {
        return ((Node*)node)->max;
    }
    
    inline void SetMin(node_t node, vec3 value) {
        ((Node*)node)->min = value;
    }
    
    inline void SetMax(node_t node, vec3 value) {
        ((Node*)node)->max = value;
    }
    
    
    inline uint32_t GetValue(node_t node) const {
        return ((Node*)node)->value;
    }
    
    inline void SetValue(node_t node, uint32_t value) {
        ((Node*)node)->value = value;
    }
    
    
    inline bool IsLeaf(node_t node) const {
        return ((Node*)node)->IsLeaf();
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
        
        vec3 min = {0.0f, 0.0f, 0.0f};
        vec3 max = {0.0f, 0.0f, 0.0f};
    };
    
    Node* root = nullptr;
};

}

#endif // TRAM_SDK_TEMPLATES_AABB_H