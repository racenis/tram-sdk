// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_TEMPLATES_OCTREE_H
#define TRAM_SDK_TEMPLATES_OCTREE_H

#include <vector>
#include <algorithm>
#include <framework/math.h>

#include <iostream>

namespace tram {

template <typename T>
class Octree {
public:
    Octree(vec3 mid_point, float half_extent) {
        root = nodes.size();
        
        nodes.push_back({
            .mid_point = mid_point,
            .half_extent = half_extent
        });
    }
    
    typedef uint32_t node_t;
    typedef uint32_t leaf_t;
    
    static constexpr node_t INVALID = ~0;
    
    leaf_t insert(vec3 point, const T& data) {
        return Insert(point, data, root);
    }
        
    void remove(leaf_t leaf) {
        YeetLeaf(leaf);
    }

    size_t find(T* array, vec3 point) {
        NearestSearch search = {.point = point};

        Find(search, root);

        for (int i = 0 ; i < search.found; i++) {
            array[i] = GetValue(search.nearest[i]);
        }
        
        return search.found;
    }
    
    // aliases, do not use for new code
    leaf_t Insert(vec3 point, const T& data) { return insert(point, data); }
    void Remove(leaf_t leaf) { remove(leaf); }
    size_t Find(T* array, vec3 point) { return find(array, point); }

protected:
    enum Octant {
        OCTANT_TOP_LEFT_BACK,
        OCTANT_TOP_LEFT_FRONT,
        OCTANT_TOP_RIGHT_BACK,
        OCTANT_TOP_RIGHT_FRONT,
        OCTANT_BOTTOM_LEFT_BACK,
        OCTANT_BOTTOM_LEFT_FRONT,
        OCTANT_BOTTOM_RIGHT_BACK,
        OCTANT_BOTTOM_RIGHT_FRONT
    };
    
    struct NearestSearch {
        node_t nearest[4] = {INVALID, INVALID, INVALID, INVALID};
        float distance[4] = {INFINITY, INFINITY, INFINITY, INFINITY};
        int farthest_index = 0;
        float farthest_distance = -INFINITY;
        vec3 point;
        int found = 0;
    };
    
    
    leaf_t Insert(vec3 point, const T& data, node_t node) {
        node_t better_node = GetChild(node, GetOctant(GetMidPoint(node), point));
        if (better_node != INVALID) {
            return Insert(point, data, better_node);
        }
        
        return MakeLeaf(node, point, data);
    }
    
    void Find(NearestSearch& search, node_t node) {
        // find deepest node in which the search point is in
        node_t priority = GetChild(node, GetOctant(GetMidPoint(node), search.point));
        if (priority != INVALID) {
            Find(search, priority);
        }
        
        for (int oct = 0; oct < 8; oct++) {
            node_t child = GetChild(node, (Octant)oct);
            
            if (child == INVALID) continue;
            if (child == priority) continue;
            
            float dist = NodeDistanceNearest2(GetMidPoint(child), GetHalfExtent(child), search.point);
            
            if (search.found >= 4 && dist > search.farthest_distance) continue;
            
            Find(search, child);
        }
        
        // check all leaves for current node
        for (leaf_t leaf : GetLeaves(node)) {
            float dist = Distance2(search.point, GetPoint(leaf));

            // make sure that all spots are filled in
            if (search.found < 4) {
                if (search.nearest[search.found] != INVALID) continue;
                
                search.nearest[search.found] = leaf;
                search.distance[search.found] = dist;
                
                if (dist > search.farthest_distance) {
                    search.farthest_distance = dist;
                    search.farthest_index = search.found;
                }
                
                search.found++;
            } else if (dist < search.farthest_distance) {

                search.nearest[search.farthest_index] = leaf;
                search.distance[search.farthest_index] = dist;
                
                search.farthest_distance = -INFINITY;
                
                // check if some other leaf is now the farthest
                for (int i = 0; i < 4; i++) {
                    if (search.distance[i] < search.farthest_distance) continue;

                    search.farthest_index = i;
                    search.farthest_distance = search.distance[i];
                }
            }
        }
    }
    
    static inline std::pair<vec3, float> GetDimensions(Octant octant, vec3 parent_midpoint, float parent_half_extent) {
        float half_extent = parent_half_extent / 2.0f;
        vec3 mid_point = parent_midpoint;
        
        switch (octant) {
            case OCTANT_TOP_LEFT_BACK:
                mid_point.x -= half_extent;
                mid_point.y += half_extent;
                mid_point.z -= half_extent;
                break;
            case OCTANT_TOP_LEFT_FRONT:
                mid_point.x -= half_extent;
                mid_point.y += half_extent;
                mid_point.z += half_extent;
                break;
            case OCTANT_TOP_RIGHT_BACK:
                mid_point.x += half_extent;
                mid_point.y += half_extent;
                mid_point.z -= half_extent;
                break;
            case OCTANT_TOP_RIGHT_FRONT:
                mid_point.x += half_extent;
                mid_point.y += half_extent;
                mid_point.z += half_extent;
                break;
            case OCTANT_BOTTOM_LEFT_BACK:
                mid_point.x -= half_extent;
                mid_point.y -= half_extent;
                mid_point.z -= half_extent;
                break;
            case OCTANT_BOTTOM_LEFT_FRONT:
                mid_point.x -= half_extent;
                mid_point.y -= half_extent;
                mid_point.z += half_extent;
                break;
            case OCTANT_BOTTOM_RIGHT_BACK:
                mid_point.x += half_extent;
                mid_point.y -= half_extent;
                mid_point.z -= half_extent;
                break;
            case OCTANT_BOTTOM_RIGHT_FRONT:
                mid_point.x += half_extent;
                mid_point.y -= half_extent;
                mid_point.z += half_extent;
                break;
        }
        
        return {mid_point, half_extent};
    }
    
    static inline Octant GetOctant(vec3 mid, vec3 point) {
        if (point.y < mid.y) {
            if (point.x < mid.x) {
                if (point.z < mid.z) {
                    return OCTANT_BOTTOM_LEFT_BACK;
                } else {
                    return OCTANT_BOTTOM_LEFT_FRONT;
                }
            } else {
                if (point.z < mid.z) {
                    return OCTANT_BOTTOM_RIGHT_BACK;
                } else {
                    return OCTANT_BOTTOM_RIGHT_FRONT;
                }
            }
        } else {
            if (point.x < mid.x) {
                if (point.z < mid.z) {
                    return OCTANT_TOP_LEFT_BACK;
                } else {
                    return OCTANT_TOP_LEFT_FRONT;
                }
            } else {
                if (point.z < mid.z) {
                    return OCTANT_TOP_RIGHT_BACK;
                } else {
                    return OCTANT_TOP_RIGHT_FRONT;
                }
            }
        }
        
        assert(false);
    }
    
    inline float Distance2(vec3 a, vec3 b) {
        vec3 d = a - b;
        return d.x * d.x + d.y * d.y + d.z * d.z;
    }
    
    static inline float NodeDistanceNearest2(vec3 mid_point, float half_extent, vec3 query) {
        vec3 d = glm::abs(query - mid_point) - half_extent;
        vec3 outside = max(d, vec3(0.0f));

        return outside.x * outside.x + outside.y * outside.y + outside.z * outside.z;
    }
    
    static inline float NodeDistanceNearest(vec3 mid_point, float half_extent, vec3 query) {
        vec3 d = glm::abs(query - mid_point) - half_extent;
        vec3 outside = max(d, vec3(0.0f));

        return glm::length(outside);
    }
    
    static inline float NodeDistanceFarthest(vec3 mid_point, float half_extent, vec3 query) {
        return glm::length(glm::abs(query - mid_point) + half_extent);
    }
    
    // TODO: move this code to??? wherever scene AABB tree gets drawn
    /*
    void Draw(Node * node) {
        if (node->IsNode()) {
            *//*vec3 p00 = node->mid_point + vec3(node->width, node->width, node->width);
            vec3 p01 = node->mid_point + vec3(-node->width, node->width, node->width);
            vec3 p02 = node->mid_point + vec3(node->width, node->width, -node->width);
            vec3 p03 = node->mid_point + vec3(-node->width, node->width, -node->width);
            vec3 p10 = node->mid_point + vec3(node->width, -node->width, node->width);
            vec3 p11 = node->mid_point + vec3(-node->width, -node->width, node->width);
            vec3 p12 = node->mid_point + vec3(node->width, -node->width, -node->width);
            vec3 p13 = node->mid_point + vec3(-node->width, -node->width, -node->width);
            
            AddLine(p00, p01, COLOR_YELLOW);
            AddLine(p00, p02, COLOR_YELLOW);
            AddLine(p03, p01, COLOR_YELLOW);
            AddLine(p03, p02, COLOR_YELLOW);
            
            AddLine(p10, p11, COLOR_YELLOW);
            AddLine(p10, p12, COLOR_YELLOW);
            AddLine(p13, p11, COLOR_YELLOW);
            AddLine(p13, p12, COLOR_YELLOW);
            
            AddLine(p00, p10, COLOR_YELLOW);
            AddLine(p01, p11, COLOR_YELLOW);
            AddLine(p02, p12, COLOR_YELLOW);
            AddLine(p03, p13, COLOR_YELLOW);*//*
            
            for (int i = 0 ; i < 8; i++) {
                if (node->octants[i]) Draw(node->octants[i]);
            }
        } else {
            //AddLineMarker(node->point, COLOR_GREEN);
        }
    }*/
    
    
    
    struct Node {
        std::vector<leaf_t> leaves;
        
        node_t octants[8] = {INVALID, INVALID, INVALID, INVALID, INVALID, INVALID, INVALID, INVALID};
        
        node_t parent = INVALID;
        Octant parent_octant = OCTANT_BOTTOM_LEFT_BACK;
        
        vec3 mid_point = {0.0f, 0.0f, 0.0f};
        float half_extent = 1.0f;
    };
    
    struct Leaf {
        node_t parent = INVALID;
        vec3 point = {0.0f, 0.0f, 0.0f};
        T value;
    };
    
    inline node_t MakeNode(node_t parent, Octant octant) {
        node_t idx = INVALID;
        if (node_freelist.size()) {
            idx = node_freelist.back();
            node_freelist.pop_back();
            nodes[idx] = Node{};
        } else {
            idx = nodes.size();
            nodes.push_back(Node());
        }
        
        auto [mid, ext] = GetDimensions(octant, nodes[parent].mid_point, nodes[parent].half_extent);
        
        nodes[idx].mid_point = mid;
        nodes[idx].half_extent = ext;
        
        nodes[idx].parent = parent;
        nodes[idx].parent_octant = octant;
        
        nodes[parent].octants[octant] = idx;
        
        return idx;
    }
    
    inline void YeetNode(node_t node) {
        node_t parent = nodes[node].parent;
        
        assert(node != root);
        assert(parent != INVALID);
        
        // reparent all children
        for (leaf_t leaf : nodes[node].leaves) {
            nodes[parent].leaves.push_back(leaf);
            leaves[leaf].parent = parent;
        }
        
        nodes[parent].octants[nodes[node].parent_octant] = INVALID;
        nodes[node].parent = INVALID;
        
        node_freelist.push_back(node);
    }
    
    inline leaf_t MakeLeaf(node_t parent, vec3 point, const T& value) {
        leaf_t idx = INVALID;
        if (leaf_freelist.size()) {
            idx = leaf_freelist.back();
            leaf_freelist.pop_back();
            leaves[idx] = Leaf{};
        } else {
            idx = leaves.size();
            leaves.push_back(Leaf());
        }

        leaves[idx].point = point;
        leaves[idx].value = value;
        
        leaves[idx].parent = parent;
        
        nodes[parent].leaves.push_back(idx);
        
        TrySplitting(parent);
        
        return idx;
    }
    
    inline void YeetLeaf(leaf_t leaf) {
        node_t parent = leaves[leaf].parent;
        
        auto it = nodes[parent].leaves.begin();
        auto end = nodes[parent].leaves.end();
        while (it < end && *it != leaf) it++;
        
        assert(it != end);
        
        nodes[parent].leaves.erase(it);
        
        TryFolding(parent);

        leaf_freelist.push_back(leaf);
    }

    inline std::vector<leaf_t>& GetLeaves(node_t node) {
        return nodes[node].leaves;
    }
    
    inline vec3 GetMidPoint(node_t node) {
        return nodes[node].mid_point;
    }
    
    inline float GetHalfExtent(node_t node) {
        return nodes[node].half_extent;
    }
    
    inline node_t GetChild(node_t node, Octant octant) {
        return nodes[node].octants[octant];
    }
    
    
    inline void TryFolding(node_t node) {
        if (node == root) return;
        
        if (GetLeaves(node).size()) return;
        
        for (int i = 0; i < 8; i++) {
            if (GetChild(node, (Octant)i) != INVALID) return;
        }
        
        node_t parent = nodes[node].parent;
        YeetNode(node);
        
        TryFolding(parent);
    }
    
    inline void TrySplitting(node_t node) {
        if (GetLeaves(node).size() < 4) return;
        
        // first we'll check if it's even worth splitting the tree.
        // if all leaves are right next to each other, they'll just
        // end up in the same node anyway and we'll just increase
        // the depth of the tree for no reason. also identical points
        // could force the split function to recurse indefinitely
        bool worth_it = false;
        vec3 first = GetPoint(GetLeaves(node)[0]);
        for (size_t i = 1; i < GetLeaves(node).size(); i++) {
            if (Distance2(first, GetPoint(GetLeaves(node)[i])) < 1.0f) continue;
            
            worth_it = true;
            break;
        }
        
        if (!worth_it) return;
        
        // move all leaves
        for (leaf_t leaf : GetLeaves(node)) {
            
            Octant oct = GetOctant(GetMidPoint(node), GetPoint(leaf));
            node_t oct_node = GetChild(node, oct);
            
            if (oct_node == INVALID) {
                oct_node = MakeNode(node, oct);
            }
            
            leaves[leaf].parent = oct_node;
            GetLeaves(oct_node).push_back(leaf);
        }
        
        GetLeaves(node).clear();
    }

    inline vec3 GetPoint(leaf_t leaf) {
        return leaves[leaf].point;
    }
    
    inline T GetValue(leaf_t leaf) {
        return leaves[leaf].value;
    }
    
    std::vector<Node> nodes;
    std::vector<Leaf> leaves;
    
    std::vector<node_t> node_freelist;
    std::vector<leaf_t> leaf_freelist;
    
    node_t root = INVALID;
};

}

#endif  // TRAM_SDK_TEMPLATES_OCTREE_H