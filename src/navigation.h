// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// NAVIGATION.H -- Navigation stuff [sketch]

#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <core.h>

namespace Core {
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