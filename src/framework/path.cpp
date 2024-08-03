// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/path.h>

#include <framework/file.h>

#include <render/render.h>

#include <templates/pool.h>
#include <templates/hashmap.h>

#include <unordered_map>
#include <cstring>

namespace tram {

template <> Pool<Path> PoolProxy<Path>::pool("path pool", 100);
static Hashmap<Path*> path_list("path list", 200);


Path* Path::Find (name_t name) {
    Path* path = path_list.Find(name);
    
    if (!path) {
        path = PoolProxy<Path>::New(name);
        path_list.Insert(name, path);
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
    
    if (file.read_name() != "PATHv2") {
        std::cout << "Unrecognized path format in " << path << std::endl; return;
    }
    
    while (file.is_continue()) {
        name_t entry_type = file.read_name();
        
        if (entry_type == "node") {
            Node node = {
                .position = {file.read_float32(),
                             file.read_float32(),
                             file.read_float32()}
            };
            
            this->nodes.push_back(node);
        } else if (entry_type == "edge") {
            uint32_t edge_index = this->edges.size();
            
            Edge edge = {file.read_uint32(), file.read_uint32()};
            
            this->nodes[edge.from].edges.push_back(edge_index);
            
            this->edges.push_back(edge);
            this->edge_infos.push_back({});
        } else if (entry_type == "path") {
            uint32_t edge_index = file.read_uint32();
            name_t path_type = file.read_name();
            
            if (path_type == "left") {
                this->edge_infos[edge_index].type = PATH_LEFT;
            } else if (path_type == "right") {
                this->edge_infos[edge_index].type = PATH_RIGHT;
            }
        }
    }
}

void Path::Draw() {
    for (auto edge : edges) {
        Render::AddLine(nodes[edge.from].position, nodes[edge.to].position, Render::COLOR_WHITE);
        
    }
    
}


PathFollower::PathFollower(Path* path, vec3 initial_pos, PathType type) {
    this->prev = 0;
    this->next = 0;
    this->progress = 0.0f;
    this->direction = PATH_STRAIGHT;
    this->following = path;
    this->type = type;
    
    Project(initial_pos);
}

void PathFollower::Advance(float) {
    // TODO:
    // calculate distance between prev and next
    // convert from advance meters into percentage points
    // advance
    // if pushed beyond next,
}

void PathFollower::Project(vec3) {
    // TODO:
    // basically, bad poopy initial algorithm:
    
    // for each edge in following path edges:
    //      get nearest point on edge
    //      save edge with the nearest point
    // save first and second edge points
    // also calculate progrogress
    
    
}

void SetOrientation(vec3) {
    // TODO:
    
    // parameter is a normal vector
    // if it is pointing towards next point, good
    // otherwise swap next and prev
}

vec3 PathFollower::GetPosition() {
    
    // TODO: implement curve position get
    
    // this next one is only linear
    
    return glm::mix(following->nodes[prev].position,
                    following->nodes[next].position,
                    progress);
}

void PathFollower::PathFollower::TurnLeft() {
    this->direction = PATH_LEFT;
}
void PathFollower::TurnRight() {
    this->direction = PATH_RIGHT;
}
void PathFollower::TurnStraight() {
    this->direction = PATH_STRAIGHT;
}


}