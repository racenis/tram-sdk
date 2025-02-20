// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/transition.h>

#include <framework/worldcell.h>
#include <framework/logging.h>
#include <render/render.h>
#include <templates/hashmap.h>

#include <config.h>

#include <algorithm>

/**
 * @class tram::Transition framework/transition.h <framework/transition.h>
 * 
 * Connects WorldCells together.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/transition.html
 */

namespace tram {

template <> Pool<Transition> PoolProxy<Transition>::pool("worldcelltransition pool", WORLDCELL_TRANSITION_LIMIT);
static Hashmap<Transition*> transition_list("transition list", WORLDCELL_TRANSITION_LIMIT);

/// Finds a transition with the given name.
/// Pointer to the transition or a nullptr if wasn't found.
Transition* Transition::Find(name_t name) {
    return transition_list.Find(name);
}

/// Creates a new transition.
/// @param name         Name of the transition. Can be set to "none" if the
///                     transition doesn't need a name.
/// @param cell_into    Pointer to the WorldCell into which the transition
///                     will be leading into.
Transition* Transition::Make(name_t name, WorldCell* cell_into) {
    Transition* transition = PoolProxy<Transition>::New(name, cell_into);
    
    if (name) {
        if (transition_list.Find(name)) {
            Log(Severity::WARNING, System::CORE, "Transition named {} already exists!", name);
        }
        
        transition_list.Insert(name, transition);
    }
    
    return transition;
}

Transition::Transition(name_t name, WorldCell* cell_into) {
    this->cell_into = cell_into;
    this->name = name;
    
    if (!cell_into) {
        Log(Severity::ERROR, System::CORE, "Transition '{}' not leading to anywhere", name);
    }
}

/// Adds a point to the transition volume.
/// @note After adding points make sure to call Transition::GeneratePlanes().
void Transition::AddPoint(vec3 point) {
    points.push_back(point);
}

/// Checks whether a point is inside the transition volume.
bool Transition::IsInside(vec3 point) {
    for(size_t i = 0; i < planes.size(); i++)
        if(glm::dot(planes[i], vec4(point, 1.0f)) < 0.0f) return false;    
    return true;
}

/// Finalizes the transition after adding points to it.
/// @param disp Draws the transition planes for a single frame if set to true.
void Transition::GeneratePlanes(bool disp) {
    assert(points.size() > 3);
    
    planes.clear();
    
    // this hull generation algorithm works in O(n^3), but there exist algorithms
    // that can do the same thing in linear time
    
    // also the code is really bad, but don't touch it!! it works!
    
    // if we replace this with a faster algorithm, we can tidy this up
    
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