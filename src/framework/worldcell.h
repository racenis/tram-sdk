// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_WORLDCELL_H
#define TRAM_SDK_FRAMEWORK_WORLDCELL_H

#include <vector>

#include <framework/uid.h>
#include <framework/math.h>

namespace tram {

class Entity;
class Transition;

class WorldCell {
public:
    WorldCell(name_t name) { this->name = name; }

    inline name_t GetName() { return name; }

    inline bool IsLoaded() { return flags & LOADED; }
    inline bool IsInterior() { return flags & INTERIOR; }
    inline bool HasInteriorLighting() { return flags & INTERIOR_LIGHTING; }
    inline bool IsDebugDraw() { return flags & DEBUG_DRAW; }
    inline bool HasAutomaticLoading() { return flags & AUTOMATIC_LOADING; }
    
    inline void SetInterior (bool is) { SetFlag(INTERIOR, is); }
    inline void SetInteriorLights (bool is) { SetFlag(INTERIOR_LIGHTING, is); }
    inline void SetDebugDraw (bool is) { SetFlag(DEBUG_DRAW, is); }
    inline void SetAutomaticLoading (bool is) { SetFlag(AUTOMATIC_LOADING, is); }
    
    void Load();
    void Unload();

    void LoadFromDisk();

    void Add(Entity* entity);
    void Remove(Entity* entity);
    
    size_t GetEntityCount() { return entities.size(); }

    void Add(Transition* transition);
    
    WorldCell* FindTransition(vec3 point);
    
    inline const std::vector<Entity*>& GetEntities() { return entities; }
    inline const std::vector<Transition*>& GetTransitions() { return transitions; }
    inline const std::vector<Transition*>& GetVolume() { return volume; }
    
    
    bool IsInside(vec3 point);

    static WorldCell* Find (vec3 point);
    static WorldCell* Find (name_t name);
    static WorldCell* Make (name_t name);
    
protected:
    enum {
        LOADED = 1,
        INTERIOR = 2,
        INTERIOR_LIGHTING = 4,
        DEBUG_DRAW = 8,
        AUTOMATIC_LOADING = 16,
    };

    inline void SetFlag(uint32_t flag, bool value) { flags = value ? flags | flag : flags & ~flag; };

    name_t name;
    uint32_t flags = AUTOMATIC_LOADING;
    std::vector<Entity*> entities;
    std::vector<Transition*> transitions;
    std::vector<Transition*> volume;
};

}

#endif // TRAM_SDK_FRAMEWORK_WORLDCELL_H