// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_NAVMESH_H
#define TRAM_SDK_FRAMEWORK_NAVMESH_H

#include <framework/graph.h>

#include <framework/core.h>
#include <framework/uid.h>
#include <framework/math.h>

#include <vector>

namespace tram {
    
typedef uint32_t node_id_t;

class Navmesh : public Graph {
public:
    Navmesh(name_t name) : name(name) {}
    ~Navmesh() = delete;

    inline name_t GetName() { return name; }
    void Draw();
    
    inline const std::vector<Node>& GetNodes() const { return nodes; }
    inline const std::vector<Edge>& GetEdges() const { return edges; }
    
    void LoadFromDisk();
    
    static Navmesh* Find(name_t name);
protected:
    name_t name;
};

class Navplan {
public:
    void Reset();
    
    vec3 GetNextPoint();
    int32_t GetRemaining();
    void Advance();
    
    void Yeet();
    void Draw();
    
    inline bool IsReady() const { return is_ready; }

    static Navplan* Make(Navmesh* mesh, vec3 from, vec3 to);
protected:
    bool is_ready = false;
    std::vector<vec3> points;
    vec3 from = {0.0f, 0.0f, 0.0f};
    vec3 to = {0.0f, 0.0f, 0.0f};
    Navmesh* mesh = nullptr;

    Navplan(Navmesh* mesh, vec3 from, vec3 to) : from(from), to(to), mesh(mesh) {}
    ~Navplan() = default;
};

}

#endif // TRAM_SDK_FRAMEWORK_NAVMESH_H