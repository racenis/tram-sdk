// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_NAVIGATION_H
#define FRAMEWORK_NAVIGATION_H

#include <templates/octree.h>

#include <framework/core.h>
#include <framework/uid.h>

namespace Core {
    extern bool DRAW_PATH_DEBUG;
    
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

#endif // FRAMEWORK_NAVIGATION_H