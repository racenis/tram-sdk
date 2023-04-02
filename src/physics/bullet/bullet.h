// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef PHYSICS_BULLET_BULLET_H
#define PHYSICS_BULLET_BULLET_H

#include <physics/physics.h>
#include <physics/collisionshape.h>

#include <btBulletDynamicsCommon.h>

namespace tram::Physics {
    
enum UserIndex : int {
    USERINDEX_PHYSICSCOMPONENT = 1,
    USERINDEX_TRIGGERCOMPONENT = 2,
};

btConvexShape* CollisionShapeToConvexShape (CollisionShape shape);

void InitBullet();
void StepPhysics();
void DrawDebug();

extern btBroadphaseInterface* BULLET_BROADPHASE_INTERFACE;
extern btDefaultCollisionConfiguration* BULLET_COLLISION_CONFIGURATION;
extern btCollisionDispatcher* BULLET_COLLIISION_DISPATCHER;
extern btSequentialImpulseConstraintSolver* BULLET_CONSTRAINT_SOLVER;
extern btDiscreteDynamicsWorld* BULLET_DYNAMICS_WORLD;
extern btIDebugDraw* BULLET_DEBUG_DRAWER;
extern btVehicleRaycaster* BULLET_VEHICLE_RAYCASTER;
    
}

#endif // PHYSICS_BULLET_BULLET_H
