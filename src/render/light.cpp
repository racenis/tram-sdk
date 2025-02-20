// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/light.h>

#include <framework/file.h>
#include <framework/logging.h>

#include <templates/hashmap.h>

#include <config.h>

/**
 * @class tram::Render::LightGraph render/lightgraph.h <render/lightgraph.h>
 * 
 * LightGraph resource.
 */

using namespace tram;
using namespace tram::Render;

static Hashmap<LightGraph*> lightgraph_list("LightGraph name list", RESOURCE_LIMIT_SPRITE);

template <> Pool<Render::LightGraph> PoolProxy<Render::LightGraph>::pool("LightGraph pool", RESOURCE_LIMIT_SPRITE);

// TODO: add an octree here with all of the nodes!!

// TODO: implement
SphericalHarmonic LightGraph::LookupHarmonic(vec3 position, uint32_t layers) {
    return SphericalHarmonic{};
}

LightGraph* LightGraph::Find(name_t name) {
    auto graph = lightgraph_list.Find(name);
    
    if (!graph) {
        graph = PoolProxy<LightGraph>::New(name);
        lightgraph_list.Insert(name, graph);
    }

    return graph;
}

void LightGraph::LoadFromDisk() {
    std::string filename = std::string("data/worldcells/") + std::string(name) + ".light";
    
    File file (filename.c_str(), File::READ);
    
    if (!file.is_open()) {
        Log("Light graph not found: {}", filename);
        return;
    }
    
    name_t header = file.read_name();
    
    if (header != "LIGHTGRAPHv1") {
        Log("Sprite not found: {}", filename);
        return;
    }
    
    while (file.is_continue()) {
        name_t record_type = file.read_name();
        
        if (record_type == "node") {
            Node new_node;
            
            new_node.position = {file.read_float32(),
                                 file.read_float32(),
                                 file.read_float32()};
            
            new_node.has_light = file.read_uint32();
            new_node.has_reflection = file.read_uint32();
            
            nodes.push_back(new_node);
            
        } else if (record_type == "edge") {
            const uint32_t from_node_index = file.read_uint32();
            const uint32_t to_node_index = file.read_uint32();
            
            edges.push_back({from_node_index, to_node_index});
            
        } else if (record_type == "light") {
            uint32_t index = file.read_uint32();
            uint32_t layer = file.read_uint32();
            name_t channel = file.read_name();
            
            nodes[index].constants.resize(layer);
            
            auto& c = nodes[index].constants[layer];
            
            if (channel == "r") {
                c.l00.x = file.read_float32();

                c.l1m1.x = file.read_float32();
                c.l10.x = file.read_float32();
                c.l11.x = file.read_float32();

                c.l2m2.x = file.read_float32();
                c.l2m1.x = file.read_float32();
                c.l20.x = file.read_float32();
                c.l21.x = file.read_float32();
                c.l22.x = file.read_float32();
            } else if (channel == "g") {
                c.l00.y = file.read_float32();

                c.l1m1.y = file.read_float32();
                c.l10.y = file.read_float32();
                c.l11.y = file.read_float32();

                c.l2m2.y = file.read_float32();
                c.l2m1.y = file.read_float32();
                c.l20.y = file.read_float32();
                c.l21.y = file.read_float32();
                c.l22.y = file.read_float32();
            } else if (channel == "b") {
                c.l00.z = file.read_float32();

                c.l1m1.z = file.read_float32();
                c.l10.z = file.read_float32();
                c.l11.z = file.read_float32();

                c.l2m2.z = file.read_float32();
                c.l2m1.z = file.read_float32();
                c.l20.z = file.read_float32();
                c.l21.z = file.read_float32();
                c.l22.z = file.read_float32();
            }
        } else {
            std::cout << "unknown path record: " << record_type << std::endl;
    }
    }
    
    status = READY;
}

