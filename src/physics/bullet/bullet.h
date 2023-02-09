// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef PHYSICS_BULLET_BULLET_H
#define PHYSICS_BULLET_BULLET_H

#include <physics/physics.h>
#include <physics/collisionshape.h>

#include <btBulletDynamicsCommon.h>

namespace Core::Physics {
    enum UserIndex : int {
        USERINDEX_PHYSICSCOMPONENT = 1,
        USERINDEX_TRIGGERCOMPONENT = 2,
    };
    
    btConvexShape* CollisionShapeToConvexShape (CollisionShape shape);
    
    void InitBullet();
    void StepPhysics();
    void DrawDebug();
    
    extern btBroadphaseInterface* broadphaseInterface;
    extern btDefaultCollisionConfiguration* collisionConfiguration;
    extern btCollisionDispatcher* collisionDispatcher;
    extern btSequentialImpulseConstraintSolver* constraintSolver;
    extern btDiscreteDynamicsWorld* dynamicsWorld;
    extern btIDebugDraw* debugDrawer;
    extern btVehicleRaycaster* vehicleRaycaster;
}

#endif // PHYSICS_BULLET_BULLET_H
