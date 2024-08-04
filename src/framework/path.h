// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_PATH_H
#define TRAM_SDK_FRAMEWORK_PATH_H

#include <framework/graph.h>

#include <framework/uid.h>

namespace tram {

enum PathType {
    PATH_STRAIGHT,
    PATH_LEFT,
    PATH_RIGHT,
    
    PATH_LINEAR,
    PATH_CURVE
};
    
struct PathEdgeInfo {
    PathType type;
};

class PathFollower;

class Path : public Graph {
public:
    Path(name_t name) : name(name) {}

    void LoadFromDisk();

    void Draw();
    
    static Path* Find(name_t name);
protected:
    ~Path() = delete;

    friend class PathFollower;

    name_t name;
    std::vector<PathEdgeInfo> edge_infos;
};

class PathFollower {
public:
    PathFollower(Path* path, vec3 initial_pos, PathType type);
    
    void Advance(float);
    
    void Project(vec3);
    
    void TurnLeft();
    void TurnRight();
    void TurnStraight();
    
    vec3 GetPosition();
    vec3 GetTangent();
    
    void SetOrientation(vec3);
    
    float progress;
    uint32_t prev;
    uint32_t next;
    PathType type;
    PathType direction;
    Path* path;
};
    
}

#endif // TRAM_SDK_FRAMEWORK_PATH_H