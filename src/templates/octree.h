// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// STACKPOOL.H -- Stackpool container template.

#ifndef TEMPLATES_OCTREE_H
#define TEMPLATES_OCTREE_H

#include <vector>
#include <algorithm>
#include <glm/glm.hpp>

namespace Core {
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
            std::sort(nodevec.begin(), nodevec.end(), [=](const Node& a, const Node& b){return glm::distance(glm::vec3(x, y, z), glm::vec3(a.x, a.y, a.z)) > glm::distance(glm::vec3(x, y, z), glm::vec3(b.x, b.y, b.z));});
            for (size_t i = 0; i < 4 && i < nodevec.size(); i++){
                result[i] = nodevec[i].type;
            }
            return nodevec.size() > 4 ? 4 : nodevec.size();
        }
    };
}

#endif  // TEMPLATES_OCTREE_H