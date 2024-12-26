// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_NAVMESH_H
#define TRAM_SDK_FRAMEWORK_NAVMESH_H

#include <framework/core.h>
#include <framework/uid.h>
#include <framework/math.h>

#include <vector>

namespace tram {
    
typedef uint32_t node_id_t;

struct NavmeshNode {
    node_id_t next, prev, left, right;
    vec3 position;
};

class Navmesh {
public:
    Navmesh (name_t name) : name(name) {}
    ~Navmesh() = delete;

    inline name_t GetName() { return name; }
    inline const std::vector<node_id_t>& GetNodes() { return nodes; }
    
    void LoadFromDisk();
    
    static NavmeshNode GetNavmeshNode (node_id_t node_id);
    static Navmesh* Find (name_t name);
protected:
    name_t name;
    std::vector<node_id_t> nodes;
};

}

#endif // TRAM_SDK_FRAMEWORK_NAVMESH_H