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
// 4. X     start moving away from directly accessing Node struct to static inline methods on the AABBTree class
// 5. X     finish moving away
// 6. X     now Node has been completely abstracted and node_t can be fully opaque
// 7. switch node_t to uint32_t mapping it to a Node*
// 8. replace new/deletes with an array
// 9. devise improved packing schemes for better performance
    
class AABBTree {
public:
    AABBTree() {
        this->root = MakeNode();
    }
    
    ~AABBTree() {
        RemoveHierarchy(root);
    }
    
    typedef uint32_t node_t;
    
    static constexpr node_t INVALID = ~0;
    
    vec3 GetAABBMin() { return GetMin(root); }
    vec3 GetAABBMax() { return GetMax(root); }
    
private:
    struct Node;
public:
    
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
        
        return new_node;
    }
    
    void RemoveLeaf(node_t node) {
        assert(node != INVALID);
        assert(node != root);
        
        node_t parent = GetParent(node);
        
        assert(parent != INVALID);
        
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
    }
    
    void FindIntersection(vec3 ray_pos, vec3 ray_dir, std::vector<uint32_t>& result) const {
        FindIntersection(ray_pos, ray_dir, root, result);
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
    
    const node_t GetRoot() const {
        return root;
    }
    
private:
    void RemoveHierarchy(node_t node) {
        if (IsLeaf(node)) {
            YeetNode(node);
        } else {
            if (GetLeft(node) != INVALID) RemoveHierarchy(GetLeft(node));
            if (GetRight(node) != INVALID) RemoveHierarchy(GetRight(node));
            YeetNode(node);
        }
    }
    
    void FindIntersectionRecursive(vec3 ray_pos, vec3 ray_dir, float& nearest_dist, uint32_t& nearest_index, float distance_limit, node_t node, auto filter) const {
        if (IsLeaf(node) && node != root) {
            float leaf_distance = filter(ray_pos, ray_dir, GetValue(node));
            
            if (leaf_distance < nearest_dist) {
                nearest_dist = leaf_distance;
                nearest_index = GetValue(node);
            }
            
            return;
        }

        float left_distance = INFINITY;
        float right_distance = INFINITY;
        
        if (GetLeft(node) != INVALID) left_distance = AABBDistance(ray_pos, ray_dir, GetMin(GetLeft(node)), GetMax(GetLeft(node)));
        if (GetRight(node) != INVALID) right_distance = AABBDistance(ray_pos, ray_dir, GetMin(GetRight(node)), GetMax(GetRight(node)));
        
        if (left_distance < right_distance) {
            
            if (left_distance < nearest_dist && left_distance < distance_limit) {
                FindIntersectionRecursive(ray_pos, ray_dir, nearest_dist, nearest_index, distance_limit, GetLeft(node), filter);
            }
            
            if (right_distance < nearest_dist && right_distance < distance_limit) {
                FindIntersectionRecursive(ray_pos, ray_dir, nearest_dist, nearest_index, distance_limit, GetRight(node), filter);
            }
            
        } else {
            
            if (right_distance < nearest_dist && right_distance < distance_limit) {
                FindIntersectionRecursive(ray_pos, ray_dir, nearest_dist, nearest_index, distance_limit, GetRight(node), filter);
            }
            
            if (left_distance < nearest_dist && left_distance < distance_limit) {
                FindIntersectionRecursive(ray_pos, ray_dir, nearest_dist, nearest_index, distance_limit, GetLeft(node), filter);
            }
            
        }
        
    }
    
    void FindIntersection(vec3 ray_pos, vec3 ray_dir, node_t node, std::vector<uint32_t>& result) const {
        bool is_node_intersect = AABBIntersect(ray_pos, ray_dir, GetMin(node), GetMax(node));
        
        if (is_node_intersect) {
            if (IsLeaf(node) && node != root) {
                result.push_back(GetValue(node));
            } else {
                if (GetLeft(node) != INVALID) FindIntersection(ray_pos, ray_dir, GetLeft(node), result);
                if (GetRight(node) != INVALID) FindIntersection(ray_pos, ray_dir, GetRight(node), result);
            }
        }
    }
    
    void FindAABBIntersection(node_t node, vec3 min, vec3 max, auto callback) {
        if (IsLeaf(node) && node != root) {
            if (AABBOverlap(min, max, GetMin(node), GetMax(node))) {
                callback(GetValue(node));
            }
            
            return;
        }
        
        if (GetLeft(node) != INVALID && AABBOverlap(min, max, GetMin(GetLeft(node)), GetMax(GetLeft(node)))) {
            FindAABBIntersection(GetLeft(node), min, max, callback);
        }
        
        if (GetRight(node) != INVALID && AABBOverlap(min, max, GetMin(GetRight(node)), GetMax(GetRight(node)))) {
            FindAABBIntersection(GetRight(node), min, max, callback);
        }
        
    }
    
    void UpdateParentAABB (node_t node) {
        if (IsLeaf(node)) return;
        
        int best_rotation = 0;
        float best_volume = INFINITY;
        
        bool has_children = GetLeft(node) != INVALID && GetRight(node) != INVALID;
        
        if (has_children && !IsLeaf(GetRight(node)) && GetLeft(GetRight(node)) != INVALID) {
            vec3 min = GetMin(GetLeft(node));
            vec3 max = GetMax(GetLeft(node));
            
            if (GetRight(GetRight(node)) != INVALID) min = MergeAABBMin(min, GetMin(GetRight(GetRight(node))));
            if (GetRight(GetRight(node)) != INVALID) max = MergeAABBMax(max, GetMax(GetRight(GetRight(node))));
            
            float volume = AABBVolume(min, max);
            
            if (best_volume > volume && AABBVolume(GetMin(GetRight(node)), GetMax(GetRight(node))) > volume) {
                best_volume = volume;
                best_rotation = 1;
            }
        }
        
        if (has_children && !IsLeaf(GetRight(node)) && GetRight(GetRight(node)) != INVALID) {
            vec3 min = GetMin(GetLeft(node));
            vec3 max = GetMax(GetLeft(node));
            
            if (GetLeft(GetRight(node)) != INVALID) min = MergeAABBMin(min, GetMin(GetLeft(GetRight(node))));
            if (GetLeft(GetRight(node)) != INVALID) max = MergeAABBMax(max, GetMax(GetLeft(GetRight(node))));
            
            float volume = AABBVolume(min, max);
            
            if (best_volume > volume && AABBVolume(GetMin(GetRight(node)), GetMax(GetRight(node))) > volume) {
                best_volume = volume;
                best_rotation = 2;
            }
        }
        
        
        if (has_children && !IsLeaf(GetLeft(node)) && GetLeft(GetLeft(node)) != INVALID) {
            vec3 min = GetMin(GetRight(node));
            vec3 max = GetMax(GetRight(node));
            
            if (GetRight(GetLeft(node)) != INVALID) min = MergeAABBMin(min, GetMin(GetRight(GetLeft(node))));
            if (GetRight(GetLeft(node)) != INVALID) max = MergeAABBMax(max, GetMax(GetRight(GetLeft(node))));
            
            float volume = AABBVolume(min, max);
            
            if (best_volume > volume && AABBVolume(GetMin(GetLeft(node)), GetMax(GetLeft(node))) > volume) {
                best_volume = volume;
                best_rotation = 3;
            }
        }
        
        if (has_children && !IsLeaf(GetLeft(node)) && GetRight(GetLeft(node)) != INVALID) {
            vec3 min = GetMin(GetRight(node));
            vec3 max = GetMax(GetRight(node));
            
            if (GetLeft(GetLeft(node)) != INVALID) min = MergeAABBMin(min, GetMin(GetLeft(GetLeft(node))));
            if (GetLeft(GetLeft(node)) != INVALID) max = MergeAABBMax(max, GetMax(GetLeft(GetLeft(node))));
            
            float volume = AABBVolume(min, max);
            
            if (best_volume > volume && AABBVolume(GetMin(GetLeft(node)), GetMax(GetLeft(node))) > volume) {
                best_volume = volume;
                best_rotation = 4;
            }
        }
        
        
        
        if (best_rotation == 1) {
            node_t parent1 = GetParent(GetLeft(node));
            node_t parent2 = GetParent(GetLeft(GetRight(node)));
            
            SetParent(GetLeft(node), parent2);
            SetParent(GetLeft(GetRight(node)), parent1);
            
            node_t node1 = GetLeft(node);
            node_t node2 = GetLeft(GetRight(node));
            
            SetLeft(node, node2);
            SetLeft(GetRight(node), node1);
            
            vec3 min = GetMin(GetLeft(GetRight(node)));
            vec3 max = GetMax(GetLeft(GetRight(node)));
            
            if (GetRight(GetRight(node)) != INVALID) min = MergeAABBMin(min, GetMin(GetRight(GetRight(node))));
            if (GetRight(GetRight(node)) != INVALID) max = MergeAABBMax(max, GetMax(GetRight(GetRight(node))));

            SetMin(GetRight(node), min);
            SetMax(GetRight(node), max);
        }
        
        if (best_rotation == 2) {
            node_t parent1 = GetParent(GetLeft(node));
            node_t parent2 = GetParent(GetRight(GetRight(node)));
            
            SetParent(GetLeft(node), parent2);
            SetParent(GetRight(GetRight(node)), parent1);
            
            node_t node1 = GetLeft(node);
            node_t node2 = GetRight(GetRight(node));
            
            SetLeft(node, node2);
            SetRight(GetRight(node), node1);
            
            vec3 min = GetMin(GetRight(GetRight(node)));
            vec3 max = GetMax(GetRight(GetRight(node)));
            
            if (GetLeft(GetRight(node)) != INVALID) min = MergeAABBMin(min, GetMin(GetLeft(GetRight(node))));
            if (GetLeft(GetRight(node)) != INVALID) max = MergeAABBMax(max, GetMax(GetLeft(GetRight(node))));

            SetMin(GetRight(node), min);
            SetMax(GetRight(node), max);
        }
        
        
        if (best_rotation == 3) {
            node_t parent1 = GetParent(GetRight(node));
            node_t parent2 = GetParent(GetLeft(GetLeft(node)));
            
            SetParent(GetRight(node), parent2);
            SetParent(GetLeft(GetLeft(node)), parent1);
            
            node_t node1 = GetRight(node);
            node_t node2 = GetLeft(GetLeft(node));
            
            SetRight(node, node2);
            SetLeft(GetLeft(node), node1);
            
            vec3 min = GetMin(GetLeft(GetLeft(node)));
            vec3 max = GetMax(GetLeft(GetLeft(node)));
            
            if (GetRight(GetLeft(node)) != INVALID) min = MergeAABBMin(min, GetMin(GetRight(GetLeft(node))));
            if (GetRight(GetLeft(node)) != INVALID) max = MergeAABBMax(max, GetMax(GetRight(GetLeft(node))));

            SetMin(GetLeft(node), min);
            SetMax(GetLeft(node), max);
        }
        
        if (best_rotation == 4) {
            node_t parent1 = GetParent(GetRight(node));
            node_t parent2 = GetParent(GetRight(GetLeft(node)));
            
            SetParent(GetRight(node), parent2);
            SetParent(GetRight(GetLeft(node)), parent1);
            
            node_t node1 = GetRight(node);
            node_t node2 = GetRight(GetLeft(node));
            
            SetRight(node, node2);
            SetRight(GetLeft(node), node1);
            
            vec3 min = GetMin(GetRight(GetLeft(node)));
            vec3 max = GetMax(GetRight(GetLeft(node)));

            if (GetLeft(GetLeft(node)) != INVALID) min = MergeAABBMin(min, GetMin(GetLeft(GetLeft(node))));
            if (GetLeft(GetLeft(node)) != INVALID) max = MergeAABBMax(max, GetMax(GetLeft(GetLeft(node))));

            SetMin(GetLeft(node), min);
            SetMax(GetLeft(node), max);
        }
        
        
        node_t left_child = GetLeft(node);
        node_t right_child = GetRight(node);
        
        if (left_child == INVALID || right_child == INVALID) {
            assert(node == root);
            
            if (left_child == INVALID && right_child != INVALID) {
                SetMin(node, GetMin(right_child));
                SetMax(node, GetMax(right_child));
            } else if (left_child != INVALID && right_child == INVALID) {
                SetMin(node, GetMin(left_child));
                SetMax(node, GetMax(left_child));
            } else {
                SetMin(node, {0.0f, 0.0f, 0.0f});
                SetMax(node, {0.0f, 0.0f, 0.0f});
            }
            
            return;
        }
        
        
        SetMin(node, MergeAABBMin(GetMin(left_child), GetMin(right_child)));
        SetMax(node, MergeAABBMax(GetMax(left_child), GetMax(right_child)));
        
        if (GetParent(node) != INVALID) {
            UpdateParentAABB(GetParent(node));
        }
        
        assert(GetParent(node) != node);
    }
    
    // searches the children of search_node to find a sibling for target_node
    node_t FindSibling (vec3 min, vec3 max, node_t node) {
        assert(node != INVALID);

        if (IsLeaf(node)) {
            return node;
        }
        
        assert(GetLeft(node) != INVALID);
        assert(GetRight(node) != INVALID);
        
        vec3 left_merge_min = MergeAABBMin(min, GetMin(GetLeft(node)));
        vec3 left_merge_max = MergeAABBMax(max, GetMax(GetLeft(node)));
        
        vec3 right_merge_min = MergeAABBMin(min, GetMin(GetRight(node)));
        vec3 right_merge_max = MergeAABBMax(max, GetMax(GetRight(node)));
        
        float left_merge_volume = AABBVolume(left_merge_min, left_merge_max);
        float right_merge_volume = AABBVolume(right_merge_min, right_merge_max);
        
        if (left_merge_volume < right_merge_volume) {
            return FindSibling(min, max, GetLeft(node));
        } else {
            return FindSibling(min, max, GetRight(node));
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
        vec3 t1 = (min - ray_pos) / ray_dir;
        vec3 t2 = (max - ray_pos) / ray_dir;
        
        vec3 t1min = glm::min(t1, t2);
        vec3 t2max = glm::max(t1, t2);
        
        float tnear = glm::max(glm::max(t1min.x, t1min.y), t1min.z);
        float tfar = glm::min(glm::min(t2max.x, t2max.y), t2max.z);
        
        return tfar >= tnear;
    }
    
    static float AABBDistance(vec3 ray_pos, vec3 ray_dir, vec3 min, vec3 max) {
        vec3 t1 = (min - ray_pos) / ray_dir;
        vec3 t2 = (max - ray_pos) / ray_dir;
        
        vec3 t1min = glm::min(t1, t2);
        vec3 t2max = glm::max(t1, t2);
        
        float tnear = glm::max(glm::max(t1min.x, t1min.y), t1min.z);
        float tfar = glm::min(glm::min(t2max.x, t2max.y), t2max.z);
        
        return tfar >= tnear ? tnear : INFINITY;
    }
    
    void FindDepthRecursive(node_t node, int current, int& largest) {
        if (current > largest) largest = current;
        
        if (IsLeaf(node)) return;
        
        if (GetLeft(node) != INVALID) {
            FindDepthRecursive(GetLeft(node), current + 1, largest);
        }
        
        if (GetRight(node) != INVALID) {
            FindDepthRecursive(GetRight(node), current + 1, largest);
        }
    }

public:
    inline node_t MakeNode() {
        if (node_freelist.size()) {
            const node_t idx = node_freelist.back();
            node_freelist.pop_back();
            
            // TODO: optimize cleaning
            nodes[idx] = Node();
            
            mins[idx] = {0.0f, 0.0f, 0.0f};
            maxes[idx] = {0.0f, 0.0f, 0.0f};
            
            return idx;
        }
        
        const node_t idx = nodes.size();
        
        nodes.push_back(Node());
        mins.push_back({0.0f, 0.0f, 0.0f});
        maxes.push_back({0.0f, 0.0f, 0.0f});
        
        return idx;
    }
    
    inline void YeetNode(node_t node) {
        if (IsLeaf(node)) {
            value_freelist.push_back(GetLeft(node));
        }
        
        node_freelist.push_back(node);
    }
    
    inline node_t GetLeft(node_t node) const {
        return nodes[node].left;
    }
    
    inline node_t GetRight(node_t node) const {
        return nodes[node].right;
    }
    
    inline node_t GetParent(node_t node) const {
        return nodes[node].parent;
    }
    
    inline void SetLeft(node_t node, node_t value) {
        nodes[node].left = value;
    }
    
    inline void SetRight(node_t node, node_t value) {
        nodes[node].right = value;
    }
    
    inline void SetParent(node_t node, node_t value) {
        nodes[node].parent = value;
    }
    
    
    inline vec3 GetMin(node_t node) const {
        return mins[node];
    }
    
    inline vec3 GetMax(node_t node) const {
        return maxes[node];
    }
    
    inline void SetMin(node_t node, vec3 value) {
        mins[node] = value;
    }
    
    inline void SetMax(node_t node, vec3 value) {
        maxes[node] = value;
    }
    
    
    inline uint32_t GetValue(node_t node) const {
        assert(IsLeaf(node));
        
        return values[GetLeft(node)];
    }
    
    inline void SetValue(node_t node, uint32_t value) {
        assert(IsLeaf(node));
        
        if (GetLeft(node) != INVALID) {
            values[GetLeft(node)] = value;
            return;
        }
        
        if (value_freelist.size()) {
            const node_t idx = value_freelist.back();
            value_freelist.pop_back();
            
            values[idx] = value;
            
            SetLeft(node, idx);
            
            return;
        }
        
        const node_t idx = values.size();
        values.push_back(value);
        
        SetLeft(node, idx);
    }
    
    inline bool IsLeaf(node_t node) const {
        return nodes[node].right == INVALID;
    }

    void Reserve(size_t items) {
        float depth = floorf(1.5f * logf((float)items));
        float node_count = ceilf(powf(2.0f, depth + 1.0f) - 1.0f);
        nodes.reserve(node_count);
        mins.reserve(node_count);
        maxes.reserve(node_count);
        values.reserve(items);
    }

private:

    struct Node {
        node_t left = INVALID;
        node_t right = INVALID;
        node_t parent = INVALID;
    };
    
    std::vector<vec3> mins;
    std::vector<vec3> maxes;
    
    std::vector<Node> nodes;
    
    std::vector<uint32_t> values;
    
    std::vector<uint32_t> node_freelist;
    std::vector<uint32_t> value_freelist;

    node_t root = INVALID;
};

}

#endif // TRAM_SDK_TEMPLATES_AABB_H