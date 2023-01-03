// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// NAVIGATION.H -- Navigation stuff [incomplete]

#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <framework/core.h>

namespace Core {
    struct Path {
        struct Node {
            Node* next;
            Node* prev;
            Node* left;
            Node* right;
            glm::vec3 p1;
            glm::vec3 p2;
            glm::vec3 p3;
            glm::vec3 p4;
            float lens[10];
            
            void ProducePoint(glm::vec3& p, const float& t);
            void CalculateLenghts();
            void Render();
            
            struct Follower {
                Node* current_node;
                float t = 0.0f;
                
                void GoForth(float ammount);
                void Render();
                void GetPosition(glm::vec3& pos);
            };
        };
        
        name_t name = 0;
        std::vector<Node> nodes;
    };
    
    struct Navmesh {
        struct Node {
            Node* next;
            Node* prev;
            Node* left;
            Node* right;
            glm::vec3 location;
        };
        
        name_t name = 0;
        std::vector<Node> nodes;
        static Octree<Node*> all_nodes;
    };
    
    struct NavigationPlan {
        glm::vec3 from;
        glm::vec3 to;
        size_t len = -1;
        Navmesh::Node* nodes[100];
    };
    
    NavigationPlan* MakeNavigationPlan(const glm::vec3& from, const glm::vec3& to);
    void YeetNavigationPlan(NavigationPlan* plan);
}

#endif //NAVIGATION_H