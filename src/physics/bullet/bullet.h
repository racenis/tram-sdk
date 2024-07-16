// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef PHYSICS_BULLET_BULLET_H
#define PHYSICS_BULLET_BULLET_H

#include <physics/physics.h>
#include <physics/collisionshape.h>

#include <btBulletDynamicsCommon.h>

namespace tram::Physics::Bullet {
    
enum UserIndex : int {
    USERINDEX_PHYSICSCOMPONENT = 1,
    USERINDEX_TRIGGERCOMPONENT = 2,
};

void Init();
void StepPhysics();
void DrawDebug();

Collision Raycast (const vec3& from, const vec3& to, uint32_t collision_mask = -1);
std::vector<Collision> Shapecast (const CollisionShape& shape, const vec3& from, const vec3& to, uint32_t collision_mask = -1);

extern btBroadphaseInterface* BROADPHASE_INTERFACE;
extern btDefaultCollisionConfiguration* COLLISION_CONFIGURATION;
extern btCollisionDispatcher* COLLIISION_DISPATCHER;
extern btSequentialImpulseConstraintSolver* CONSTRAINT_SOLVER;
extern btDiscreteDynamicsWorld* DYNAMICS_WORLD;
extern btIDebugDraw* DEBUG_DRAWER;
extern btVehicleRaycaster* VEHICLE_RAYCASTER;
    
}

#endif // PHYSICS_BULLET_BULLET_H
