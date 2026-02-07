// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_TEMPLATES_OCTREE_H
#define TRAM_SDK_TEMPLATES_OCTREE_H

#include <vector>
#include <algorithm>
#include <framework/math.h>

#include <iostream>

/*
 * insteadable idea:
 * 
 * each node has a bucket
 * first drop every node into root bucket
 * 
 * then when a bucket gets too clustered, check if nodes can be split
 * if can, then deepen the thing
 * 
 * when last node gets removed from bucket, patch it back into parent
 * 
 * ACTIONS:
 * 
 * X    rip out old API
 * 
 * X    replace pool with vector
 * 
 * then just do basic bucket splitting
 * 
 * then do bucket taking back together
 * 
 * */

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
    
    leaf_t Insert(vec3 point, const T& data) {
        return Insert(point, data, root);
    }
    
    void Remove(leaf_t leaf) {
        YeetLeaf(leaf);
    }

    size_t Find(T* array, vec3 point) {
        NearestSearch search = {.point = point};

        Find(search, root);

        for (int i = 0 ; i < search.found; i++) {
            array[i] = GetValue(search.nearest[i]);
        }
        
        return search.found;
    }

    void Draw() {
        Draw(root);
    }
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
        // TODO: find best node to put the data into
        // for now we'll just put it in the first one i.e. root
        return MakeLeaf(node, point, data);
    }
    
    void Find(NearestSearch& search, node_t node) {
        // TODO: implement proper algorithm
        // for now we'll just search the first node i.e. root
        
        // check all leaves for current node
        for (leaf_t leaf : GetLeaves(node)) {
            float dist = glm::distance(search.point, GetPoint(leaf));
            
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
    
    std::pair<vec3, float> GetDimensions(Octant octant, vec3 parent_midpoint, float parent_half_extent) {
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
    
    Octant GetOctant(vec3 mid, vec3 point) {
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
        node_t idx = nodes.size();
        nodes.push_back(Node());
        
        auto [mid, ext] = GetDimensions(octant, nodes[parent].mid_point, nodes[parent].half_extent);
        
        nodes[idx].mid_point = mid;
        nodes[idx].half_extent = ext;
        
        nodes[idx].parent = parent;
        nodes[idx].parent_octant = octant;
        
        nodes[parent].octants[octant] = idx;
        
        return idx;
        
        // TODO: implement freelist
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
        
        
        // TODO: implement freelist
    }
    
    inline leaf_t MakeLeaf(node_t parent, vec3 point, const T& value) {
        leaf_t idx = leaves.size();
        leaves.push_back(Leaf());
        
        leaves[idx].point = point;
        leaves[idx].value = value;
        
        leaves[idx].parent = parent;
        
        nodes[parent].leaves.push_back(idx);
        
        TrySplitting(parent);
        
        return idx;
        
        // TODO: implement freelist
    }
    
    inline void YeetLeaf(leaf_t leaf) {
        node_t parent = leaves[leaf].parent;
        
        auto it = nodes[parent].leaves.begin();
        auto end = nodes[parent].leaves.end();
        while (it < end && *it != leaf) it++;
        
        assert(it != end);
        
        nodes[parent].leaves.erase(it);
        
        TryFolding(parent);

        // TODO: implement freelist
    }

    inline const std::vector<leaf_t>& GetLeaves(node_t node) {
        return nodes[node].leaves;
    }
    
    inline void TryFolding(node_t node) {
        // TODO: implement
        // - check if any leaves
        // - check if any octants
        // - if not, then delete node
    }
    
    inline void TrySplitting(node_t node) {
        // TODO: implement
    }

    inline vec3 GetPoint(leaf_t leaf) {
        return leaves[leaf].point;
    }
    
    inline T GetValue(leaf_t leaf) {
        return leaves[leaf].value;
    }
    
    std::vector<Node> nodes;
    std::vector<Leaf> leaves;
    
    node_t root = INVALID;
};

}

#endif  // TRAM_SDK_TEMPLATES_OCTREE_H