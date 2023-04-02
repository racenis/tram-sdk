// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <physics/bullet/bullet.h>
#include <physics/bullet/debugdrawer.h>

#include <components/triggercomponent.h>

#include <btBulletDynamicsCommon.h>

namespace tram::Physics {

btBroadphaseInterface* BULLET_BROADPHASE_INTERFACE = nullptr;
btDefaultCollisionConfiguration* BULLET_COLLISION_CONFIGURATION = nullptr;
btCollisionDispatcher* BULLET_COLLIISION_DISPATCHER = nullptr;
btSequentialImpulseConstraintSolver* BULLET_CONSTRAINT_SOLVER = nullptr;
btDiscreteDynamicsWorld* BULLET_DYNAMICS_WORLD = nullptr;
btIDebugDraw* BULLET_DEBUG_DRAWER = nullptr;
btVehicleRaycaster* BULLET_VEHICLE_RAYCASTER = nullptr;

btConvexShape* CollisionShapeToConvexShape (CollisionShape shape) {
    switch (shape.type) {
        case SHAPE_SPHERE:
            return new btSphereShape(shape.dimensions.x);
        case SHAPE_CYLINDER:
            return new btCylinderShape(btVector3(shape.dimensions.x, shape.dimensions.y, shape.dimensions.z));
        case SHAPE_CAPSULE:
            return new btCapsuleShape(shape.dimensions.x, shape.dimensions.y);
        case SHAPE_BOX:
            return new btBoxShape(btVector3(shape.dimensions.x, shape.dimensions.y, shape.dimensions.z));
        default:
            return nullptr;
    }
}

void InitBullet(){
    BULLET_BROADPHASE_INTERFACE = new btDbvtBroadphase();
    BULLET_COLLISION_CONFIGURATION = new btDefaultCollisionConfiguration();
    BULLET_COLLIISION_DISPATCHER = new btCollisionDispatcher(BULLET_COLLISION_CONFIGURATION);
    BULLET_CONSTRAINT_SOLVER = new btSequentialImpulseConstraintSolver();

    BULLET_DYNAMICS_WORLD = new btDiscreteDynamicsWorld(BULLET_COLLIISION_DISPATCHER, BULLET_BROADPHASE_INTERFACE, BULLET_CONSTRAINT_SOLVER, BULLET_COLLISION_CONFIGURATION);

    BULLET_DYNAMICS_WORLD->setGravity(btVector3(0.0f, -9.8f, 0.0f));

    BULLET_DEBUG_DRAWER  = new PhysicsDebugDraw;
    BULLET_DYNAMICS_WORLD->setDebugDrawer(BULLET_DEBUG_DRAWER);

    BULLET_VEHICLE_RAYCASTER = new btDefaultVehicleRaycaster(BULLET_DYNAMICS_WORLD);
    
    // this will make a plane so that stuff doesn't fall out of the world
    btTransform trans;
    trans.setIdentity();
    btCollisionShape* shape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), -9.0f);
    btDefaultMotionState* motionstate = new btDefaultMotionState(trans);
    btRigidBody::btRigidBodyConstructionInfo constructioninfo (0.0f, motionstate, shape, btVector3(0.0f, 0.0f, 0.0f));
    btRigidBody* rigidbody = new btRigidBody(constructioninfo);

    BULLET_DYNAMICS_WORLD->addRigidBody(rigidbody);
}

void StepPhysics(){
    BULLET_DYNAMICS_WORLD->stepSimulation(1.0f/60.0f, 0);
    
    // process the triggers
    for (auto& trigger : PoolProxy<TriggerComponent>::GetPool()) trigger.ResetCollisions();
    
    int numManifolds = BULLET_DYNAMICS_WORLD->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold* contactManifold =  BULLET_DYNAMICS_WORLD->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* obA = contactManifold->getBody0();
        const btCollisionObject* obB = contactManifold->getBody1();
        
        if (obA->getUserIndex() == USERINDEX_TRIGGERCOMPONENT &&
            obB->getUserIndex() == USERINDEX_PHYSICSCOMPONENT &&
            contactManifold->getNumContacts()) {
            assert(obA->getUserPointer());
            assert(obB->getUserPointer());
            auto& contact = contactManifold->getContactPoint(0).getPositionWorldOnA();
            ((TriggerComponent*) obA->getUserPointer())->Collision({
                (PhysicsComponent*) obB->getUserPointer(),
                {contact.getX(), contact.getY(), contact.getZ()}
            });
        }
        
        if (obA->getUserIndex() == USERINDEX_PHYSICSCOMPONENT &&
            obB->getUserIndex() == USERINDEX_TRIGGERCOMPONENT &&
            contactManifold->getNumContacts()) {
            assert(obA->getUserPointer());
            assert(obB->getUserPointer());
            auto& contact = contactManifold->getContactPoint(0).getPositionWorldOnB();
            ((TriggerComponent*) obB->getUserPointer())->Collision({
                (PhysicsComponent*) obA->getUserPointer(),
                {contact.getX(), contact.getY(), contact.getZ()}
            });
        }
    }
}

void DrawDebug() {
    BULLET_DYNAMICS_WORLD->debugDrawWorld();
}

}