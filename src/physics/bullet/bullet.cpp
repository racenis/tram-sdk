// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <physics/bullet/bullet.h>
#include <physics/bullet/debugdrawer.h>

#include <components/triggercomponent.h>

#include <btBulletDynamicsCommon.h>

namespace tram::Physics::Bullet {

btBroadphaseInterface* BROADPHASE_INTERFACE = nullptr;
btDefaultCollisionConfiguration* COLLISION_CONFIGURATION = nullptr;
btCollisionDispatcher* COLLIISION_DISPATCHER = nullptr;
btSequentialImpulseConstraintSolver* CONSTRAINT_SOLVER = nullptr;
btDiscreteDynamicsWorld* DYNAMICS_WORLD = nullptr;
btIDebugDraw* DEBUG_DRAWER = nullptr;
btVehicleRaycaster* VEHICLE_RAYCASTER = nullptr;

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

void Init() {
    BROADPHASE_INTERFACE = new btDbvtBroadphase();
    COLLISION_CONFIGURATION = new btDefaultCollisionConfiguration();
    COLLIISION_DISPATCHER = new btCollisionDispatcher(COLLISION_CONFIGURATION);
    CONSTRAINT_SOLVER = new btSequentialImpulseConstraintSolver();

    DYNAMICS_WORLD = new btDiscreteDynamicsWorld(COLLIISION_DISPATCHER, BROADPHASE_INTERFACE, CONSTRAINT_SOLVER, COLLISION_CONFIGURATION);

    DYNAMICS_WORLD->setGravity(btVector3(0.0f, -9.8f, 0.0f));

    DEBUG_DRAWER  = new PhysicsDebugDraw;
    DYNAMICS_WORLD->setDebugDrawer(DEBUG_DRAWER);

    VEHICLE_RAYCASTER = new btDefaultVehicleRaycaster(DYNAMICS_WORLD);
    
    // this will make a plane so that stuff doesn't fall out of the world
    btTransform trans;
    trans.setIdentity();
    btCollisionShape* shape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), -9.0f);
    btDefaultMotionState* motionstate = new btDefaultMotionState(trans);
    btRigidBody::btRigidBodyConstructionInfo constructioninfo (0.0f, motionstate, shape, btVector3(0.0f, 0.0f, 0.0f));
    btRigidBody* rigidbody = new btRigidBody(constructioninfo);

    DYNAMICS_WORLD->addRigidBody(rigidbody);
}

void StepPhysics(){
    DYNAMICS_WORLD->stepSimulation(1.0f/60.0f, 0);
    
    // process the triggers
    for (auto& trigger : PoolProxy<TriggerComponent>::GetPool()) trigger.ResetCollisions();
    
    int numManifolds = DYNAMICS_WORLD->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; i++)
    {
        btPersistentManifold* contactManifold = DYNAMICS_WORLD->getDispatcher()->getManifoldByIndexInternal(i);
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
    DYNAMICS_WORLD->debugDrawWorld();
}

Collision Raycast (const glm::vec3& from, const glm::vec3& to, uint32_t collision_mask) {
    btVector3 bto, bfrom;

    bto.setValue(to.x, to.y, to.z);
    bfrom.setValue(from.x, from.y, from.z);

    btCollisionWorld::ClosestRayResultCallback callback (bfrom, bto);

    callback.m_collisionFilterMask = collision_mask;
    
    DYNAMICS_WORLD->rayTest(bfrom, bto, callback);

    if (callback.hasHit() && callback.m_collisionObject->getUserIndex() == USERINDEX_PHYSICSCOMPONENT) {
        auto& point = callback.m_hitPointWorld;
        auto& normal = callback.m_hitNormalWorld;
        
        return {
            (PhysicsComponent*) callback.m_collisionObject->getUserPointer(),
            vec3 (point.getX(), point.getY(), point.getZ()),
            vec3 (normal.getX(), normal.getY(), normal.getZ())
        };
    } else {
        return {
            nullptr, vec3 (0.0f, 0.0f, 0.0f), vec3 (0.0f, 0.0f, 0.0f)
        };
    }
}

struct ShapecastCallback : public btCollisionWorld::ConvexResultCallback {
    ShapecastCallback(std::vector<Physics::Collision>& collisions, uint32_t collision_mask)
        : collisions(collisions), collision_mask(collision_mask) {}
    btScalar addSingleResult (btCollisionWorld::LocalConvexResult &convexResult, bool normalInWorldSpace) {
        const btCollisionObject* ob = convexResult.m_hitCollisionObject;
        
        // ignore triggers
        if (ob->getUserIndex() != USERINDEX_PHYSICSCOMPONENT) {
            return 1;
        }
        
        /*const*/ PhysicsComponent* phys_comp = (PhysicsComponent*) ob->getUserPointer();

        if (phys_comp->GetCollisionGroup() & collision_mask) {
            assert(phys_comp);
            auto& contact = convexResult.m_hitPointLocal;
            auto& normal = convexResult.m_hitNormalLocal;
            collisions.push_back({
                phys_comp,
                {contact.getX(), contact.getY(), contact.getZ()},
                {-normal.getX(), normal.getY(), normal.getZ()}
            });
        }
        
        return 1; // tbh idk what this method is supposed to return
    }
    std::vector<Physics::Collision>& collisions;
    uint32_t collision_mask;
};

/// I have no idea if this function works.
std::vector<Collision> Shapecast (const CollisionShape& shape, const vec3& from, const vec3& to, uint32_t collision_mask) {
    auto shape_ptr = CollisionShapeToConvexShape(shape);
    btTransform bto, bfrom;
    std::vector<Collision> collisions;
    
    ShapecastCallback callback (collisions, collision_mask);

    bto.setIdentity();
    bto.setOrigin({to.x, to.y, to.z});
    bfrom.setIdentity();
    bfrom.setOrigin({from.x, from.y, from.z});
    
    DYNAMICS_WORLD->convexSweepTest(shape_ptr, bfrom, bto, callback);
    
    delete shape_ptr;
    
    return collisions;
}

}