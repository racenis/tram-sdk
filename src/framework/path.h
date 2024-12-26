// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_PATH_H
#define TRAM_SDK_FRAMEWORK_PATH_H

#include <framework/graph.h>

#include <framework/uid.h>

namespace tram {

enum PathType {
    PATH_STRAIGHT,  //< Path vertex connection interpreted as a straight line.
    PATH_LEFT,      //< Path vertex connection interpreted as a left turn.
    PATH_RIGHT,     //< Path vertex connection interpreted as a right turn.
    
    PATH_LINEAR,    //< Path will be interpreted as being made of straight line segments.
    PATH_CURVE      //< Not implemented.
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