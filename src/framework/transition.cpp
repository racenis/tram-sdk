// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/transition.h>

#include <framework/worldcell.h>
#include <render/render.h>
#include <templates/hashmap.h>

#include <algorithm>

namespace tram {

template <> Pool<Transition> PoolProxy<Transition>::pool("worldcelltransition pool", 250, false);
static Hashmap<Transition*> transition_list ("transition list", 250);

Transition* Transition::Find (name_t name) {
    return transition_list.Find(name);
}

Transition* Transition::Make (name_t name, WorldCell* cell_from, WorldCell* cell_into) {
    Transition* transition = PoolProxy<Transition>::New(name, cell_from, cell_into);
    
    if (name) {
        if (transition_list.Find(name)) {
            std::cout << "Transition named " << name << " already exists!" << std::endl;
        }
        
        transition_list.Insert(name, transition);
    }
    
    return transition;
}

Transition::Transition (name_t name, WorldCell* cell_from, WorldCell* cell_into) {
    this->cell_into = cell_into;
    this->cell_from = cell_from;
    this->name = name;
    
    if (cell_from == cell_into && cell_into) {
        cell_into->AddTransitionInto(this);
        return;
    }
    
    if (cell_from) {
        cell_from->AddTransitionFrom(this);
    }
    
    if (cell_into) {
        cell_into->AddTransitionInto(this);
    }
}

void Transition::AddPoint (vec3 point) {
    points.push_back(point);
}

bool Transition::IsInside (vec3 point) {
    for(size_t i = 0; i < planes.size(); i++)
        if(glm::dot(planes[i], vec4(point, 1.0f)) < 0.0f) return false;    
    return true;
}

void Transition::GeneratePlanes (bool disp) {
    assert(points.size() > 3);
    
    planes.clear();
    
    for (size_t i = 0; i < points.size(); i++) {
        for (size_t j = i+1; j < points.size(); j++) {
            for (size_t k = j+1; k < points.size(); k++) {
                auto& A = points[i];
                auto& B = points[j];
                auto& C = points[k];
                auto AB = B - A;
                auto AC = C - A;
                auto cross = glm::cross(AB, AC);
                auto d = -(cross.x*A.x + cross.y*A.y + cross.z*A.z);
                
                vec4 plane = vec4(cross, d);
                
                for (size_t it = 0; it < points.size(); it++) {
                    if(i == it || j == it || k == it) continue;
                    if(glm::dot(plane, vec4(points[it], 1.0f)) < 0.0f) {
                        plane *= -1.0f;
                        goto tryagain;
                    }
                }
                
                okay:
                
                if (disp) {
                    Render::AddLine(A, B, Render::COLOR_WHITE);
                    Render::AddLine(A, C, Render::COLOR_WHITE);
                    Render::AddLine(B, C, Render::COLOR_WHITE);
                    
                    Render::AddLine(A, A+(glm::normalize(cross)*0.5f)+vec3(0.002f), Render::COLOR_CYAN);
                    Render::AddLine(B, B+(glm::normalize(cross)*0.1f)+vec3(0.002f), Render::COLOR_CYAN);
                    Render::AddLine(C, C+(glm::normalize(cross)*0.1f)+vec3(0.002f), Render::COLOR_CYAN);
                }
                
                planes.push_back(plane);
                
                yeet:
                continue;
                
                tryagain:
                
                for (size_t it = 0; it < points.size(); it++) {
                    if(i == it || j == it || k == it) continue;
                    if(glm::dot(plane, vec4(points[it], 1.0f)) < 0.0f) {
                        goto yeet;
                    }
                }
                
                goto okay;
            }
        }
    }
    
    std::sort(planes.begin(), planes.end(), [](const vec4& a, const vec4& b){ 
        if (a.x != b.x) return a.x < b.x;
        if (a.y != b.y) return a.y < b.y;
        if (a.z != b.z) return a.z < b.z;
        return a.w < b.w;});
    planes.erase(unique( planes.begin(), planes.end() ), planes.end());
}

}