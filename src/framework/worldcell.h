// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_WORLDCELL_H
#define TRAM_SDK_FRAMEWORK_WORLDCELL_H

#include <vector>

#include <framework/uid.h>
#include <framework/math.h>

namespace tram {

class Entity;
class Path;
class Navmesh;
class Transition;
class Loader;

class WorldCell {
public:
    WorldCell (name_t name) { this->name = name; }

    bool IsLoaded() { return loaded; }
    bool IsInterior() { return interior; }
    bool HasInteriorLighting() { return interior_lighting; }
    bool IsDebugDraw() { return debug_draw; }
    
    name_t GetName() { return name; }
    
    void SetInterior (bool interior) { this->interior = interior; }
    void SetInteriorLights (bool interior_lights) { this->interior_lighting = interior_lights; }
    void SetDebugDraw (bool debug_draw) { this->debug_draw = debug_draw; }
    
    void Load();
    void Unload();

    void LoadFromDisk();
    
    void DrawTransitions();
    void DrawNavmeshes();
    void DrawPaths();

    void AddEntity(Entity* entity);
    void RemoveEntity(Entity* entity);
    
    size_t GetEntityCount() { return entities.size(); }

    void AddTransitionInto (Transition* transition) { transitions_into.push_back(transition); }
    void AddTransitionFrom (Transition* transition) { transitions_from.push_back(transition); }

    WorldCell* FindTransition (vec3 point);
    
    bool IsInside(vec3 point);

    static WorldCell* Find (vec3 point);
    static WorldCell* Find (name_t name);
    static WorldCell* Make (name_t name);
    
protected:
    name_t name;
    bool interior = false;
    bool interior_lighting = false;
    bool loaded = false;
    bool debug_draw = false;
    std::vector<Entity*> entities;
    std::vector<Transition*> transitions_into;
    std::vector<Transition*> transitions_from;
    std::vector<Path*> paths;
    std::vector<Navmesh*> navmeshes;
    friend class Loader;
};

}

#endif // TRAM_SDK_FRAMEWORK_WORLDCELL_H