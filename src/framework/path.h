// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_PATH_H
#define TRAM_SDK_FRAMEWORK_PATH_H

#include <framework/core.h>
#include <framework/uid.h>
#include <framework/math.h>

#include <vector>

namespace tram {

typedef uint32_t node_id_t;

struct PathNode {
    node_id_t next, prev, left, right;
    vec3 position;
};

class Path {
public:
    Path (name_t name) : name(name) {}
    ~Path() = delete;

    inline name_t GetName() { return name; }
    inline const std::vector<node_id_t>& GetNodes() { return nodes; }
    
    void LoadFromDisk();

    static PathNode GetPathNode (node_id_t node_id);
    static Path* Find (name_t name);
protected:
    name_t name;
    std::vector<node_id_t> nodes;
};
    
}

#endif // TRAM_SDK_FRAMEWORK_PATH_H