// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_TRANSITION_H
#define TRAM_SDK_FRAMEWORK_TRANSITION_H

#include <vector>

#include <framework/uid.h>
#include <framework/math.h>

namespace tram {

class WorldCell;

class Transition {
public:
    Transition (name_t name, WorldCell* cell_from, WorldCell* cell_into);
    ~Transition () = delete;
    void AddPoint (vec3 point);
    void GeneratePlanes (bool disp = false);
    bool IsInside (vec3 point);
    WorldCell* GetInto() { return cell_into; }
    WorldCell* GetFrom() { return cell_from; }
    static Transition* Find (name_t name);
    static Transition* Make (name_t name, WorldCell* cell_from, WorldCell* cell_into);
protected:
    name_t name;
    WorldCell* cell_from;
    WorldCell* cell_into;
    std::vector<vec3> points;
    std::vector<vec4> planes;
    friend class WorldCell;
};

}

#endif // TRAM_SDK_FRAMEWORK_TRANSITION_H