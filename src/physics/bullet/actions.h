// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef PHYSICS_BULLET_ACTIONS_H
#define PHYSICS_BULLET_ACTIONS_H

#include <btBulletDynamicsCommon.h>
#include <components/physics.h>

namespace tram::Physics {
    /// @todo finish!!!
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