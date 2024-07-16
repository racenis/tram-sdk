// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_PHYSICS_API_H
#define TRAM_SDK_PHYSICS_API_H

#include <physics/physics.h>
#include <physics/collisionshape.h>

namespace tram::Physics::API {

typedef std::pair<vec3, quat>(*get_trf_callback)(void*);
typedef void(*set_trf_callback)(void*, std::pair<vec3, quat>);
    
struct CollisionShapeTransform {
    CollisionShape shape;
    vec3 position;
    quat rotation;
};

collisionshape_t MakeCollisionShape(CollisionShapeTransform* shapes, size_t shape_count);
collisionshape_t MakeCollisionShape(CollisionShape shape);

rigidbody_t MakeRigidbody(collisionshape_t shape, float mass, vec3 position, quat rotation, uint32_t mask, uint32_t group, get_trf_callback get_callback, set_trf_callback set_callback, void* data);
void YeetRigidbody(rigidbody_t rigidbody);
void SetRigidbodyTransformCallback(rigidbody_t rigidbody, get_trf_callback get_callback, set_trf_callback set_callback, void* data);
void SetRigidbodyCollisionCallback(rigidbody_t rigidbody, void(*callback)(void*), void* data);
void SetRigidbodyCollisionMask(rigidbody_t rigidbody, uint32_t mask);
void SetRigidbodyCollisionGroup(rigidbody_t rigidbody, uint32_t group);
void SetRigidbodyLocation(rigidbody_t rigidbody, vec3 location);
void SetRigidbodyRotation(rigidbody_t rigidbody, quat rotation);
void SetRigidbodyMass(rigidbody_t rigidbody, float mass);
void PushRigidbody(rigidbody_t rigidbody, vec3 direction);
void PushRigidbody(rigidbody_t rigidbody, vec3 direction, vec3 local);
void SpinRigidbody(rigidbody_t rigidbody, vec3 direction);
void SetRigidbodyDebugDrawing(rigidbody_t rigidbody, bool drawing);
void SetRigidbodyKinematic(rigidbody_t rigidbody, bool kinematic);
void SetRigidbodyAngularFactor(rigidbody_t rigidbody, vec3 factor);
void SetRigidbodyLinearFactor(rigidbody_t rigidbody, vec3 factor);
void SetRigidbodyVelocity(rigidbody_t rigidbody, vec3 velocity);
vec3 GetRigidbodyVelocity(rigidbody_t rigidbody);
void AwakenRigidbody(rigidbody_t rigidbody);
void SleepRigidbody(rigidbody_t rigidbody);
void DisableRigidbodyDeactivation(rigidbody_t rigidbody);

trigger_t MakeTrigger(collisionshape_t shape, uint32_t mask, uint32_t group, vec3 position, quat rotation);
void YeetTrigger(trigger_t);
void SetTriggerCollisionCallback(rigidbody_t rigidbody, void(*callback)(void*), void* data);
void SetTriggerCollisionMask(rigidbody_t rigidbody, uint32_t mask);
void SetTriggerCollisionGroup(rigidbody_t rigidbody, uint32_t group);
void SetTriggerLocation(rigidbody_t rigidbody, vec3 location);
void SetTriggerRotation(rigidbody_t rigidbody, vec3 location);
    
}

#endif // TRAM_SDK_PHYSICS_API_H