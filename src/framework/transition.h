// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_TRANSITION_H
#define TRAM_SDK_FRAMEWORK_TRANSITION_H

#include <vector>

#include <framework/uid.h>
#include <framework/math.h>

namespace tram {

class WorldCell;

class Transition {
public:
    void AddPoint(vec3 point);
    void GeneratePlanes(bool disp = false);
    bool IsInside(vec3 point) const;
    WorldCell* GetCell() const { return cell_into; }
    static Transition* Find(name_t name);
    static Transition* Make(name_t name, WorldCell* cell_into);
protected:
    Transition(name_t name, WorldCell* cell_into);
    ~Transition();
    name_t name;
    WorldCell* cell_into;
    std::vector<vec3> points;
    std::vector<vec4> planes;
    static void Yeet(Transition* transition);
    friend class WorldCell;
};

}

#endif // TRAM_SDK_FRAMEWORK_TRANSITION_H