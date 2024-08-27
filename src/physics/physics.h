// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_PHYSICS_PHYSICS_H
#define TRAM_SDK_PHYSICS_PHYSICS_H

#include <framework/core.h>
#include <framework/math.h>

#include <vector>

struct btCollisionShape;
struct btCollisionObject;
struct btRigidBody;

namespace tram {
    class PhysicsComponent;  
}

namespace tram::Physics {

void Init();
void Update();
    
extern bool DRAW_PHYSICS_DEBUG;

enum CollisionGroup : uint32_t {
    COLL_WORLDOBJ = 2,      //< Worldspawns
    COLL_STATICOBJ = 4,     //< Decorations, doors, elevators and platforms
    COLL_DYNAMICOBJ = 8,    //< Crates and other 
    COLL_TRIGGER = 16,      //< Triggers
    COLL_PLAYER  = 32,      //< Players
    COLL_MONSTER = 64,      //< Monsters and other NPCs
    COLL_VEHICLE = 128      //< Trams and microbuses
};

struct Collision {
    PhysicsComponent* collider;
    vec3 point;
    vec3 normal;
    float distance;
};

namespace API { struct RigidbodyMetadata; }

struct collisionshape_t {
    btCollisionShape* bt_shape;
};

struct trigger_t {
    btCollisionObject* bt_collisionshape;
    API::RigidbodyMetadata* bt_metadata;
};

struct rigidbody_t {
    btRigidBody* bt_rigidbody;
    API::RigidbodyMetadata* bt_metadata;
};

class CollisionShape;

Collision Raycast(const vec3& from, const vec3& to, uint32_t collision_mask = -1);
std::vector<Collision> Shapecast(const CollisionShape& shape, const vec3& from, const vec3& to, uint32_t collision_mask = -1);

}

#endif // TRAM_SDK_PHYSICS_PHYSICS_H
