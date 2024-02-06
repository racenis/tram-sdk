// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_PHYSICS_PHYSICS_H
#define TRAM_SDK_PHYSICS_PHYSICS_H

#include <framework/core.h>
#include <framework/math.h>

#include <vector>

namespace tram {
    class PhysicsComponent;  
}

namespace tram::Physics {

void Init();
void Update();
    
extern bool DRAW_PHYSICS_DEBUG;

enum CollisionGroup : uint32_t {
    COLL_WORLDOBJ = 2,   //< StaticWorldObject's, etc.
    COLL_DYNAMICOBJ = 4, //< Crates
    COLL_VEHICLE = 8,    //< Trams & busses
    COLL_PLAYER = 16,    //< Player
    COLL_TRIGGER = 32
};

struct Collision {
    PhysicsComponent* collider;
    vec3 point;
    vec3 normal;
    float distance;
};

class CollisionShape;

Collision Raycast (const vec3& from, const vec3& to, uint32_t collision_mask = -1);
std::vector<Collision> Shapecast (const CollisionShape& shape, const vec3& from, const vec3& to, uint32_t collision_mask = -1);

}

#endif // TRAM_SDK_PHYSICS_PHYSICS_H
