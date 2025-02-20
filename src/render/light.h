// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_LIGHT_H
#define TRAM_SDK_RENDER_LIGHT_H

#include <framework/math.h>
#include <framework/resource.h>

#include <vector>

namespace tram::Render {

struct SphericalHarmonic {
    vec3 l00, l1m1, l10, l11, l2m2, l2m1, l20, l21, l22;
};
    
class LightGraph : public Resource {
public:
    LightGraph(name_t name) : Resource(name) {}

    void LoadFromDisk();
    void LoadFromMemory() {}
    
    void Unload() {}
    
    static SphericalHarmonic LookupHarmonic(vec3 position, uint32_t layers);
    static LightGraph* Find(name_t name);
protected:
    struct Node {
        vec3 position;
        bool has_light = false;
        bool has_reflection = false;
        std::vector<SphericalHarmonic> constants;
    };
    
    std::vector<Node> nodes;
    std::vector<std::pair<uint32_t, uint32_t>> edges;
};

}

#endif // TRAM_SDK_RENDER_LIGHT_H