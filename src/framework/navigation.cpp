// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/navigation.h>
#include <render/render.h>

using namespace Core;
template <> Pool<Path> PoolProxy<Path>::pool ("path pool", 50, false);
template <> Pool<Navmesh> PoolProxy<Navmesh>::pool ("navmesh pool", 50, false);
template <> Pool<NavigationPlan> PoolProxy<NavigationPlan>::pool("navigation plan pool", 100);
Octree<Navmesh::Node*> Navmesh::all_nodes;
bool DRAW_PATH_DEBUG = false;

NavigationPlan* MakeNavigationPlan(const glm::vec3& from, const glm::vec3& to) {
    NavigationPlan* plan = PoolProxy<NavigationPlan>::New();
    plan->from = from;
    plan->to = to;
    
    // *place the plan in an async job*
    
    return plan;
}

void YeetNavigationPlan(NavigationPlan* plan) {
    PoolProxy<NavigationPlan>::Delete(plan);
}

void Path::Node::CalculateLenghts(){
    float c = 0.1f;
    glm::vec3 prev_p;
    glm::vec3 this_p;
    ProducePoint(prev_p, 0.0f);
    for (int i = 0; i < 10; i++) {
        ProducePoint(this_p, c);
        lens[i] = glm::distance(prev_p, this_p);
        prev_p = this_p;
        c += 0.1f;
    }
}

void Path::Node::ProducePoint(glm::vec3& p, const float& t){
    glm::vec3 l1 = glm::mix(p1, p2, t);
    glm::vec3 l2 = glm::mix(p3, p4, t);
    p = glm::mix(l1, l2, t);
}

void Path::Node::Render(){
    float c = 0.1f;
    glm::vec3 prev_p;
    glm::vec3 this_p;
    ProducePoint(prev_p, 0.0f);
    for (int i = 0; i < 10; i++) {
        ProducePoint(this_p, c);
        Render::AddLine(prev_p, this_p, Render::COLOR_CYAN);
        prev_p = this_p;
        c += 0.1f;
    }
    Render::AddLineMarker(p1, Render::COLOR_BLUE);
    Render::AddLineMarker(p2, Render::COLOR_BLUE);
    Render::AddLineMarker(p3, Render::COLOR_BLUE);
    Render::AddLineMarker(p4, Render::COLOR_BLUE);
}



void Path::Node::Follower::GoForth(float ammount){
    //int index = std::floor(t*10.0f);
    //t += (ammount/current_node->lens[index]) * 0.1f;
    
    glm::vec3 v1;
    glm::vec3 v2;
    current_node->ProducePoint(v1, t);
    current_node->ProducePoint(v2, t+0.01f);
    
    t += (ammount/glm::distance(v1, v2)) * 0.01f;
    if (t >= 1.0f) {
        current_node = current_node->next;
        t = 0.0f;
    }
}

void Path::Node::Follower::Render(){
    glm::vec3 pp;
    current_node->ProducePoint(pp, t);
    Render::AddLineMarker(pp, Render::COLOR_GREEN);
}

void Path::Node::Follower::GetPosition(glm::vec3& pos){
    current_node->ProducePoint(pos, t);
}