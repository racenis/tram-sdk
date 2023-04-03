// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <physics/physics.h>
#include <physics/bullet/bullet.h>
#include <physics/bullet/actions.h>


#include <components/triggercomponent.h>


#include <framework/system.h>

namespace tram::Physics {
/// Option for drawing physics debug lines.
/// Set to true if you want to see physics debug lines. False if not.
bool DRAW_PHYSICS_DEBUG = false;

/// Initializes the physics system.
void Init() {
    assert(!System::IsInitialized(System::SYSTEM_PHYSICS));
    
    InitBullet();
    
    System::SetInitialized(System::SYSTEM_PHYSICS, true);
}

/// Updates the physics system.
/// Should only be called once in the update cycle.
void Update() {
    StepPhysics();
    
    if (DRAW_PHYSICS_DEBUG) DrawDebug();
}

/// Performs a raycast.
/// Performs a raycast from from to to.
/// @return Collision struct. If there was nothing found, then the pointer
///         in the struct will be set to nullptr.
Collision Raycast (const glm::vec3& from, const glm::vec3& to, uint32_t collision_mask) {
    btVector3 bto, bfrom;

    bto.setValue(to.x, to.y, to.z);
    bfrom.setValue(from.x, from.y, from.z);

    btCollisionWorld::ClosestRayResultCallback callback (bfrom, bto);

    callback.m_collisionFilterMask = collision_mask;
    
    BULLET_DYNAMICS_WORLD->rayTest(bfrom, bto, callback);

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
        
        if (ob->getUserIndex() == USERINDEX_PHYSICSCOMPONENT &&
            ((TriggerComponent*)ob->getUserPointer())->GetCollisionGroup() & collision_mask) {
            assert(ob->getUserPointer());
            auto& contact = convexResult.m_hitPointLocal;
            auto& normal = convexResult.m_hitNormalLocal;
            collisions.push_back({
                (PhysicsComponent*) ob->getUserPointer(),
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
    
    BULLET_DYNAMICS_WORLD->convexSweepTest(shape_ptr, bfrom, bto, callback);
    return collisions;
}

}
