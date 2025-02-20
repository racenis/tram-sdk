// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_AUDIO_GRAPH_H
#define TRAM_SDK_AUDIO_GRAPH_H

#include <framework/math.h>
#include <framework/resource.h>

#include <vector>

namespace tram::Audio {

struct FilterParameters {
    float param1, param2; // TODO: figure out what the parameters should be
};
    
class SoundGraph : public Resource {
public:
    SoundGraph(name_t name) : Resource(name) {}

    void LoadFromDisk();
    void LoadFromMemory() {}
    
    void Unload() {}
    
    static FilterParameters LookupFilter(vec3 position);
    static SoundGraph* Find(name_t name);
protected:
    struct Node {
        vec3 position;
        FilterParameters parameters;
    };
    
    std::vector<Node> nodes;
    std::vector<std::pair<uint32_t, uint32_t>> edges;
};

}

#endif // TRAM_SDK_AUDIO_GRAPH_H