// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_LIGHT_H
#define TRAM_SDK_RENDER_LIGHT_H

#include <render/render.h>
#include <framework/resource.h>

#include <vector>

namespace tram::Render {
    
class Environment;
    
class LightGraph : public Resource {
public:
    LightGraph(name_t name) : Resource(name) {}

    void LoadFromDisk();
    void LoadFromMemory() {}
    
    void Unload();
    
    std::vector<std::pair<uint32_t, vec3>> GetEnvironmentProbes() const;
    
    static bool ContainsEntity(id_t entity);
    static SphericalHarmonic LookupHarmonic(vec3 position, uint32_t layers);
    static Environment* LookupEnvironmentMap(vec3 position);
    static LightGraph* Find(name_t name);
protected:
    struct Node {
        vec3 position;
        bool has_light = false;
        bool has_reflection = false;
        Environment* environment_map = nullptr;
        std::vector<SphericalHarmonic> constants;
    };
    
    std::vector<id_t> entities;
    std::vector<Node> nodes;
    std::vector<std::pair<uint32_t, uint32_t>> edges;
};

}

#endif // TRAM_SDK_RENDER_LIGHT_H