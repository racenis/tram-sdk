// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// ACTIONS.H -- Custom actions for the bullet physics
// This file is a template for other source files

#ifndef ACTIONS_H
#define ACTIONS_H

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

#endif //ACTIONS_H