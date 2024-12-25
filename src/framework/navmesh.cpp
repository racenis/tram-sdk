// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#include <framework/navmesh.h>

#include <framework/file.h>

#include <templates/pool.h>
#include <templates/hashmap.h>

#include <unordered_map>
#include <cstring>

namespace tram {

template <> Pool<Navmesh> PoolProxy<Navmesh>::pool ("navmesh pool", 100);
static Hashmap<Navmesh*> navmesh_list ("navmesh list", 200);

static std::unordered_map<uint32_t, node_id_t> raw_node_id_to_id;
static std::vector<NavmeshNode> navmesh_nodes = {{0, 0, 0, 0, {0, 0, 0}}};

static node_id_t GetNavmeshNodeFromRawId (uint32_t raw_id) {
    if (raw_id == 0) {
        return 0;
    }
    
    node_id_t node_id = raw_node_id_to_id[raw_id];
        
    if (!node_id) {
        node_id = navmesh_nodes.size();
        raw_node_id_to_id[raw_id] = node_id;
        navmesh_nodes.push_back({});
    }
    
    return node_id;
}

NavmeshNode Navmesh::GetNavmeshNode (node_id_t node_id) {
    return navmesh_nodes [node_id];
}

Navmesh* Navmesh::Find (name_t name) {
    Navmesh* navmesh = navmesh_list.Find(name);
    
    if (!navmesh) {
        navmesh =  PoolProxy<Navmesh>::New(name);
    }
    
    return navmesh;
}

void Navmesh::LoadFromDisk() {
    char path[100] = "data/navmeshes/";
    strcat(path, name);
    strcat(path, ".navmesh");
    
    File file (path, MODE_READ);
    
    if (!file.is_open()) {
        std::cout << "Can't find path file: " << path << std::endl; return;
    }
    
    if (file.read_name() != "NAVMESHv1") {
        std::cout << "Unrecognized navmesh format in " << path << std::endl; return;
    }
    
    file.read_name(); // skip navmesh name
    
    while (file.is_open()) {
        id_t node_id = GetNavmeshNodeFromRawId(file.read_uint32());
        
        nodes.push_back(node_id);
        
        navmesh_nodes [node_id] = {
            GetNavmeshNodeFromRawId(file.read_uint32()),
            GetNavmeshNodeFromRawId(file.read_uint32()),
            GetNavmeshNodeFromRawId(file.read_uint32()),
            GetNavmeshNodeFromRawId(file.read_uint32()),
            {
                file.read_float32(),
                file.read_float32(),
                file.read_float32(),
            }
        };
    }
}

}