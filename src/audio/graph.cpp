// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <audio/graph.h>

#include <framework/file.h>
#include <framework/logging.h>

#include <templates/hashmap.h>

#include <config.h>

/**
 * @class tram::Render::SoundGraph audio/graph.h <audio/graph.h>
 * 
 * SoundGraph resource.
 */

using namespace tram;
using namespace tram::Audio;

static Hashmap<SoundGraph*> soundgraph_list("SoundGraph name list", RESOURCE_LIMIT_SOUND_GRAPH);

template <> Pool<Audio::SoundGraph> PoolProxy<Audio::SoundGraph>::pool("SoundGraph pool", RESOURCE_LIMIT_SOUND_GRAPH);

// TODO: add an octree here with all of the nodes!!

// TODO: implement
FilterParameters SoundGraph::LookupFilter(vec3 position) {
    return FilterParameters{};
}

SoundGraph* SoundGraph::Find(name_t name) {
    auto graph = soundgraph_list.Find(name);
    
    if (!graph) {
        graph = PoolProxy<SoundGraph>::New(name);
        soundgraph_list.Insert(name, graph);
    }

    return graph;
}

void SoundGraph::LoadFromDisk() {
    std::string filename = std::string("data/worldcells/") + std::string(name) + ".sound";
    
    File file (filename.c_str(), File::READ);
    
    if (!file.is_open()) {
        Log("Sound graph not found: {}", filename);
        return;
    }
    
    name_t header = file.read_name();
    
    if (header != "SOUNDGRAPHv1") {
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
            
            nodes.push_back(new_node);
            
        } else if (record_type == "edge") {
            const uint32_t from_node_index = file.read_uint32();
            const uint32_t to_node_index = file.read_uint32();
            
            edges.push_back({from_node_index, to_node_index});
            
        } else if (record_type == "sound") {
            // TODO: implement
        } else {
            std::cout << "unknown sound graph record: " << record_type << std::endl;
    }
    }
    
    status = READY;
}

