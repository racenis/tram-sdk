// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <physics/bullet/bullet.h>
#include <physics/bullet/debugdrawer.h>

#include <components/triggercomponent.h>

#include <btBulletDynamicsCommon.h>

namespace tram::Physics {
    btBroadphaseInterface* broadphaseInterface = nullptr;
    btDefaultCollisionConfiguration* collisionConfiguration = nullptr;
    btCollisionDispatcher* collisionDispatcher = nullptr;
    btSequentialImpulseConstraintSolver* constraintSolver = nullptr;
    btDiscreteDynamicsWorld* dynamicsWorld = nullptr;
    btIDebugDraw* debugDrawer = nullptr;
    btVehicleRaycaster* vehicleRaycaster = nullptr;
    
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
        broadphaseInterface = new btDbvtBroadphase();
        collisionConfiguration = new btDefaultCollisionConfiguration();
        collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
        constraintSolver = new btSequentialImpulseConstraintSolver();

        dynamicsWorld = new btDiscreteDynamicsWorld(collisionDispatcher, broadphaseInterface, constraintSolver, collisionConfiguration);

        dynamicsWorld->setGravity(btVector3(0.0f, -9.8f, 0.0f));

        debugDrawer  = new PhysicsDebugDraw;
        dynamicsWorld->setDebugDrawer(debugDrawer);

        vehicleRaycaster = new btDefaultVehicleRaycaster(dynamicsWorld);

        btTransform trans;
        trans.setIdentity();
        btCollisionShape* planeShape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), -9.0f);
        btDefaultMotionState* planeMotionState = new btDefaultMotionState(trans);
        btRigidBody::btRigidBodyConstructionInfo planeConstructionInfo(0.0f, planeMotionState, planeShape, btVector3(0.0f, 0.0f, 0.0f));
        btRigidBody* planeRigidBody = new btRigidBody(planeConstructionInfo);

        dynamicsWorld->addRigidBody(planeRigidBody);
    }
    
    void StepPhysics(){
        dynamicsWorld->stepSimulation(1.0f/60.0f, 0);
        
        // process the triggers
        for (auto& trigger : PoolProxy<TriggerComponent>::GetPool()) trigger.ResetCollisions();
        
        int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
        for (int i = 0; i < numManifolds; i++)
        {
            btPersistentManifold* contactManifold =  dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
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
        dynamicsWorld->debugDrawWorld();
    }
    
}