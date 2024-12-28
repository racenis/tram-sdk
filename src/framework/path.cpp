// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/path.h>

#include <framework/file.h>

#include <render/render.h>

#include <templates/pool.h>
#include <templates/hashmap.h>

#include <config.h>

#include <unordered_map>
#include <cstring>

/**
 * @class tram::Path framework/path.h
 * 
 * Sequence of line segments.
 * 
 * Can be used to push an object along a path, can be used as a rail, or as a
 * path for guiding agents.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/path.html
 */
 
 /**
 * @class tram::PathFollower framework/path.h
 * 
 * Point constrained to a Path.
 * 
 * When constructed, the PathFollower will project the initial position on the
 * path, i.e. find the nearest point on the path.
 * After that, you can use PathFollower::Advance() to push it back and forth on
 * the path.
 * The position of the PathFollower can be used to position an object, so that
 * it appears to be following the given path.
 */

namespace tram {

template <> Pool<Path> PoolProxy<Path>::pool("Path pool", RESOURCE_LIMIT_PATH);
static Hashmap<Path*> path_list("Path list", RESOURCE_LIMIT_PATH);

/// Finds a path.
/// Finds a path by its name, or creates a new path by that name if not found.
/// @return Always returns a pointer to a Path.
Path* Path::Find(name_t name) {
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
    
    File file (path, File::READ);
    
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

/// Draws the path's line segments.
void Path::Draw() {
    for (auto edge : edges) {
        Render::AddLine(nodes[edge.from].position, nodes[edge.to].position, Render::COLOR_WHITE);
    }
}

/// Creates a new PathFollower for a path.
PathFollower::PathFollower(Path* path, vec3 initial_pos, PathType type) {
    this->prev = 0;
    this->next = 0;
    this->progress = 0.0f;
    this->direction = PATH_STRAIGHT;
    this->path = path;
    this->type = type;
    
    Project(initial_pos);
}

/// Advances the follower along the path.
/// @param distance Distance along the path, in meters.
void PathFollower::Advance(float distance) {
    float segment = glm::distance(path->nodes[prev].position, path->nodes[next].position);
    float dist_left = (1.0f - progress) * segment;
    
    if (dist_left < distance) {
        distance -= dist_left;
        
        // TODO: implement steering
        // also figure out what to do if path just ends there?
        uint32_t next_next = (uint32_t)-1;
        for (uint32_t edge : path->nodes[next].edges) {
            uint32_t candidate_next = path->edges[edge].to;
            if (candidate_next != prev && candidate_next != next) {
                next_next = candidate_next;
            }
        }
        
        // cancel advance if there is no more segments to advance to
        if (next_next == (uint32_t)-1) return;
        
        prev = next;
        next = next_next;
        progress = 0.0f;
        
        Advance(distance);
        //if (distance > 0.01f) Advance(distance);
    } else {
        dist_left -= distance;
        progress = (segment - dist_left) / segment;
    }
}

/// TODO: move these into math.h??
static vec3 nearest_point(vec3 a, vec3 b, vec3 p) {
    vec3 v = b - a;
    vec3 u = a - p;
    float t = - (glm::dot(v, u) / glm::dot(v, v));
    return glm::mix(a, b, glm::clamp(t, 0.0f, 1.0f));
}

static float project_line(vec3 a, vec3 b, vec3 p) {
    vec3 v = b - a;
    vec3 u = a - p;
    float t = - (glm::dot(v, u) / glm::dot(v, v));
    return glm::clamp(t, 0.0f, 1.0f);
}

void PathFollower::Project(vec3 projectable) {
    int nearest_edge = 0;
    float nearest_dist = INFINITY;
    for (size_t i = 0; i < path->edges.size(); i++) {
        vec3 from_point = path->nodes[path->edges[i].from].position;
        vec3 to_point = path->nodes[path->edges[i].to].position;
        vec3 point = nearest_point(from_point, to_point, projectable);
        
        float distance = glm::distance(projectable, point);
        
        if (distance < nearest_dist) {
            nearest_dist = distance;
            nearest_edge = i;
        }
    }
    
    this->prev = path->edges[nearest_edge].from;
    this->next = path->edges[nearest_edge].to;
    
    this->progress = project_line(path->nodes[prev].position, path->nodes[next].position, projectable);
}

void PathFollower::SetOrientation(vec3 orientation) {
    vec3 from_to = glm::normalize(path->nodes[next].position - path->nodes[prev].position);
    vec3 direction = glm::normalize(orientation);
    
    if (glm::dot(from_to, direction) < 0.0f) {
        std::swap(prev, next);
        progress = 1.0f - progress;
    }
}

vec3 PathFollower::GetPosition() {
    return glm::mix(path->nodes[prev].position,
                    path->nodes[next].position,
                    progress);
}

vec3 PathFollower::GetTangent() {
    return glm::normalize(path->nodes[next].position - path->nodes[prev].position);
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