// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef PHYSICS_BULLET_ACTIONS_H
#define PHYSICS_BULLET_ACTIONS_H

#include <btBulletDynamicsCommon.h>
#include <components/physicscomponent.h>

namespace Core::Physics {
    class PathAction : public btActionInterface {
    public:

        PathAction(PhysicsComponent* physComponent){
            physcomp = physComponent;
        }

        virtual void updateAction(btCollisionWorld* collisionWorld, btScalar deltaTimeStep);

        virtual void debugDraw(btIDebugDraw* debugDrawer){}

    private:
        PhysicsComponent* physcomp = nullptr;
    };
}

#endif // PHYSICS_BULLET_ACTIONS_H