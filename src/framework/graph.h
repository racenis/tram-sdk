// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#ifndef TRAM_SDK_FRAMEWORK_GRAPH_H
#define TRAM_SDK_FRAMEWORK_GRAPH_H

#include <framework/math.h>
#include <vector>

namespace tram {
    
struct Edge {
    uint32_t from;
    uint32_t to;
};

struct Node {
    vec3 position;
    std::vector<uint32_t> edges;
};
    
class Graph {
protected:
    std::vector<Node> nodes;
    std::vector<Edge> edges;
};






}

#endif // TRAM_SDK_FRAMEWORK_GRAPH_H