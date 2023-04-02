// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/path.h>

#include <framework/file.h>

#include <templates/pool.h>
#include <templates/hashmap.h>

#include <unordered_map>
#include <cstring>

namespace tram {

template <> Pool<Path> PoolProxy<Path>::pool ("path pool", 100);
static Hashmap<Path*> path_list ("path list", 200);

static std::unordered_map<uint32_t, node_id_t> raw_node_id_to_id;
static std::vector<PathNode> path_nodes = {{0, 0, 0, 0, {0, 0, 0}}};

static node_id_t GetPathNodeFromRawId (uint32_t raw_id) {
    if (raw_id == 0) {
        return 0;
    }
    
    node_id_t node_id = raw_node_id_to_id[raw_id];
        
    if (!node_id) {
        node_id = path_nodes.size();
        raw_node_id_to_id[raw_id] = node_id;
        path_nodes.push_back({});
    }
    
    return node_id;
}

PathNode Path::GetPathNode (node_id_t node_id) {
    return path_nodes [node_id];
}

Path* Path::Find (name_t name) {
    Path* path = path_list.Find(name);
    
    if (!path) {
        path =  PoolProxy<Path>::New(name);
    }
    
    return path;
}

void Path::LoadFromDisk() {
    char path[100] = "data/paths/";
    strcat(path, name);
    strcat(path, ".path");
    
    File file (path, MODE_READ);
    
    if (!file.is_open()) {
        std::cout << "Can't find path file: " << path << std::endl; return;
    }
    
    if (file.read_name() != "PATHv1") {
        std::cout << "Unrecognized path format in " << path << std::endl; return;
    }
    
    file.read_name(); // skip path name
    
    while (file.is_open()) {
        id_t node_id = GetPathNodeFromRawId(file.read_uint32());
        
        nodes.push_back(node_id);
        
        path_nodes [node_id] = {
            GetPathNodeFromRawId(file.read_uint32()),
            GetPathNodeFromRawId(file.read_uint32()),
            GetPathNodeFromRawId(file.read_uint32()),
            GetPathNodeFromRawId(file.read_uint32()),
            {
                file.read_float32(),
                file.read_float32(),
                file.read_float32(),
            }
        };
    }
}

}