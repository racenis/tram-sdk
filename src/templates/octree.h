// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#ifndef TRAM_SDK_TEMPLATES_OCTREE_H
#define TRAM_SDK_TEMPLATES_OCTREE_H

#include <vector>
#include <algorithm>
#include <framework/math.h>
#include <templates/pool.h>

namespace tram {

template <typename T>
class Octree {
public:
    Octree(std::string name, size_t size) : nodes(name, size) {
        root = nodes.AddNew();
        root->mid_point = {0.0f, 0.0f, 0.0f};
        root->half_extent = 100.0f;
    }
    
    uint32_t Insert(vec3 point, T data) {
        Node* new_leaf = nodes.AddNew(Node {.point = point, .data = data});
        Insert(root, new_leaf);
        return nodes.index(new_leaf);
    }
    
    void Remove(uint32_t node) {
        Remove(&nodes[node]);
    }

    size_t Find(T* array, vec3 point) {
        NearestSearch search = {.point = point};
        search.point = point;

        //std::cout << "search start " << std::endl;
        FindNearest(&search, root);
        //std::cout << " " << std::endl;
        
        for (int i = 0 ; i < search.found ; i++) {
            array[i] = search.nearest[i]->data;
        }
        
        return search.found;
    }

    void Draw() {
        Draw(root);
    }
    
    
    // temporary compatibility methods
    // TODO: remove
    uint32_t AddLeaf(T type, float x, float y, float z){
        return Insert({x, y, z}, type);
    }

    void RemoveLeaf(uint32_t leaf_id){
        Remove(leaf_id);
    }

    size_t FindNearest(T result[], float x, float y, float z){
        return Find(result, {x, y, z});
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
    
    struct Node {
        Node* octants[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
        
        Node* parent = nullptr;
        
        vec3 mid_point;
        
        float half_extent = 1.0f;
        
        vec3 point;
        
        T data;
        
        inline bool IsLeaf() {
            return !IsNode();
        }
        
        inline bool IsNode() {
            for (int i = 0; i < 8; i++) if (octants[i]) return true; 
            return false;
        }
    };
    
    Pool<Node> nodes;
    
    Node* root = nullptr;
    
    
    
    void Insert(Node* parent, Node* leaf) {
        Octant octant_type = GetOctant(parent->mid_point, leaf->point);
        Node*& octant = parent->octants[octant_type];
        
        if (octant) {
            if (octant->IsNode()) {
                Insert(octant, leaf);
            } else {
                Node* existing_leaf = octant;
                
                if (glm::distance(existing_leaf->point, leaf->point) == 0.0f) {
                    std::cout << "Point " << leaf->point.x << " " <<  leaf->point.y << " " <<  leaf->point.z << " already in tree!" << std::endl; 
                    return;
                }
                
                octant = NewNode(octant_type, parent);
                
                Insert(octant, existing_leaf);
                Insert(octant, leaf);
            }
        } else {
            octant = leaf;
            leaf->parent = parent;
        }
    }
    
    
    
    void Remove(Node* node) {
        
        // don't allow removing root node
        if (!node->parent) return;
        
        // remove node from parent
        for (int i = 0; i < 8; i++) {
            if (node->parent->octants[i] == node) {
                node->parent->octants[i] = nullptr;
                break;
            }
        }
        
        // remove parent if it has no other children
        //if (node->parent->IsLeaf()) Remove(node->parent);
        
        //delete node;
        nodes.Remove(node);
    }
    
    struct NearestSearch {
        Node* nearest[4] = {nullptr, nullptr, nullptr, nullptr};
        float distance[4] = {INFINITY, INFINITY, INFINITY, INFINITY};
        int farthest_index = 0;
        float farthest_distance = -INFINITY;
        vec3 point;
        int found = 0;
    };
    
    /*struct NearestSearch {
        vec3 nearest;
        float nearest_distance = INFINITY;
        
        vec3 point;
        bool found = false;
    };*/
    
    
    
    void FindNearest(NearestSearch* search, Node* parent_node) {
        int first_octant = GetOctant(parent_node->mid_point, search->point);
        
        FindNearest(search, parent_node, first_octant);
        
        for (int i  = 0 ; i < 8; i++) {
            if (i != first_octant) FindNearest(search, parent_node, i);
        }
    }
    
    void FindNearest(NearestSearch* search, Node* parent_node, int octant) {
        Node* search_node = parent_node->octants[octant];
        
        if (!search_node) return;
        
        if (search_node->IsNode()) {
            // do the nearest check
            if (search->found >= 4) {
                float nearest_possible = glm::distance(search->point, search_node->mid_point) - 1.73205f * search_node->half_extent;
                if (nearest_possible > search->farthest_distance) return;
            }
            
            FindNearest(search, search_node);
        } else {
            if (search->found < 4) {
                float distance = glm::distance(search_node->point, search->point);
                
                //std::cout << "packed " << distance << std::endl;
                
                search->nearest[search->found] = search_node;
                search->distance[search->found] = distance;
                
                if (search->farthest_distance < distance) {
                    search->farthest_distance = distance;
                    search->farthest_index = search->found;
                }
                
                search->found++;
            } else {
                float distance = glm::distance(search_node->point, search->point);
                
                //if (search->farthest_distance < distance) return;
                if (search->farthest_distance < distance) {
                    //std::cout << "ignore " << distance << "\t" << search->farthest_distance << std::endl;
                    return;
                } else {
                    //std::cout << "replace " << distance << "\t" << search->farthest_distance << std::endl;
                }
                
                search->distance[search->farthest_index] = distance;
                search->nearest[search->farthest_index] = search_node;
                
                search->farthest_distance = -INFINITY;
                for (int i = 0; i < 4; i++) {
                    if (search->farthest_distance < search->distance[i]) {
                        search->farthest_distance = search->distance[i];
                        search->farthest_index = i;
                    }
                }
            }
        }
    }
    
    /*void Find(NearestSearch* search, Node* node) {
        if (node->IsNode()) {
            float nearest_possible = glm::distance(search->point, node->mid_point) - 1.73205f * node->half_extent;
            if (nearest_possible > search->nearest_distance) return;
            
            for (int i = 0 ; i < 8; i++) {
                if (node->octants[i]) Find(search, node->octants[i]);
            }
        } else {
            float distance = glm::distance(search->point, node->point);
            if (search->nearest_distance > distance) {
                search->nearest_distance = distance;
                search->nearest = node->point;
                search->found = true;
            }
        }
    }*/
    
    Node* NewNode(Octant octant, Node* parent) {
        Node* node = nodes.AddNew();
        
        node->parent = parent;
        
        node->half_extent = parent->half_extent / 2.0f;
        node->mid_point = parent->mid_point;
        
        switch (octant) {
            case OCTANT_TOP_LEFT_BACK:
                node->mid_point.x -= node->half_extent;
                node->mid_point.y += node->half_extent;
                node->mid_point.z -= node->half_extent;
                break;
            case OCTANT_TOP_LEFT_FRONT:
                node->mid_point.x -= node->half_extent;
                node->mid_point.y += node->half_extent;
                node->mid_point.z += node->half_extent;
                break;
            case OCTANT_TOP_RIGHT_BACK:
                node->mid_point.x += node->half_extent;
                node->mid_point.y += node->half_extent;
                node->mid_point.z -= node->half_extent;
                break;
            case OCTANT_TOP_RIGHT_FRONT:
                node->mid_point.x += node->half_extent;
                node->mid_point.y += node->half_extent;
                node->mid_point.z += node->half_extent;
                break;
            case OCTANT_BOTTOM_LEFT_BACK:
                node->mid_point.x -= node->half_extent;
                node->mid_point.y -= node->half_extent;
                node->mid_point.z -= node->half_extent;
                break;
            case OCTANT_BOTTOM_LEFT_FRONT:
                node->mid_point.x -= node->half_extent;
                node->mid_point.y -= node->half_extent;
                node->mid_point.z += node->half_extent;
                break;
            case OCTANT_BOTTOM_RIGHT_BACK:
                node->mid_point.x += node->half_extent;
                node->mid_point.y -= node->half_extent;
                node->mid_point.z -= node->half_extent;
                break;
            case OCTANT_BOTTOM_RIGHT_FRONT:
                node->mid_point.x += node->half_extent;
                node->mid_point.y -= node->half_extent;
                node->mid_point.z += node->half_extent;
                break;
        }
        
        return node;
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
        
    }
    
    void Draw(Node * node) {
        if (node->IsNode()) {
            /*vec3 p00 = node->mid_point + vec3(node->width, node->width, node->width);
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
            AddLine(p03, p13, COLOR_YELLOW);*/
            
            for (int i = 0 ; i < 8; i++) {
                if (node->octants[i]) Draw (node->octants[i]);
            }
        } else {
            //AddLineMarker(node->point, COLOR_GREEN);
        }
    }
    
};

    
/*
template <typename T>
class Octree {
    // TODO: actually implement the octree
    struct Node {
        uint32_t id;
        float x, y, z;
        T type;
    };

    std::vector<Node> nodevec;
    uint32_t last_id = 1;
public:

    uint32_t AddLeaf(T type, float x, float y, float z){
        //Node n = {last_id, x, y, z, type};
        nodevec.push_back(Node{last_id, x, y, z, type});
        last_id++;
        return last_id - 1;
    }

    void RemoveLeaf(uint32_t leaf_id){
        nodevec.erase(std::find_if(nodevec.begin(), nodevec.end(), [=](Node& n){return n.id == leaf_id;}));
    }

    size_t FindNearest(T result[], float x, float y, float z){
        std::sort(nodevec.begin(), nodevec.end(), [=](const Node& a, const Node& b){return glm::distance(vec3(x, y, z), vec3(a.x, a.y, a.z)) < glm::distance(vec3(x, y, z), vec3(b.x, b.y, b.z));});
        for (size_t i = 0; i < 4 && i < nodevec.size(); i++){
            result[i] = nodevec[i].type;
        }
        return nodevec.size() > 4 ? 4 : nodevec.size();
    }
};
*/
    
}

#endif  // TRAM_SDK_TEMPLATES_OCTREE_H