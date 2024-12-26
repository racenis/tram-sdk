// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_PHYSICS_API_H
#define TRAM_SDK_PHYSICS_API_H

#include <physics/physics.h>
#include <physics/collisionshape.h>

/**
 * @namespace tram::Physics::API
 * 
 * Physics backend API.
 */

namespace tram::Physics::API {

struct CollisionShapeTransform {
    CollisionShape shape;
    vec3 position;
    quat rotation;
};

struct ObjectCollision {
    vec3 point;
    vec3 normal;
    float distance;
};
    
typedef std::pair<vec3, quat>(*get_trf_callback)(void*);
typedef void(*set_trf_callback)(void*, std::pair<vec3, quat>);
typedef void(*col_callback)(void*, void*, ObjectCollision);

collisionshape_t MakeCollisionShape(CollisionShapeTransform* shapes, size_t shape_count);
collisionshape_t MakeCollisionShape(CollisionShape shape);
void YeetCollisionShape(collisionshape_t shape);

rigidbody_t MakeRigidbody(collisionshape_t shape, float mass, vec3 position, quat rotation, uint32_t mask, uint32_t group, get_trf_callback get_callback, set_trf_callback set_callback, void* data);
void YeetRigidbody(rigidbody_t rigidbody);
void SetRigidbodyTransformCallback(rigidbody_t rigidbody, get_trf_callback get_callback, set_trf_callback set_callback, void* data);
void SetRigidbodyCollisionCallback(rigidbody_t rigidbody, col_callback callback, void* data);
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
void SetTriggerCollisionCallback(trigger_t rigidbody, col_callback callback, void* data);
void SetTriggerCollisionMask(trigger_t rigidbody, uint32_t mask);
void SetTriggerCollisionGroup(trigger_t rigidbody, uint32_t group);
void SetTriggerLocation(trigger_t rigidbody, vec3 location);
void SetTriggerRotation(trigger_t rigidbody, quat location);

std::pair<ObjectCollision, void*> Raycast(vec3 from, vec3 to, uint32_t collision_mask);
std::vector<std::pair<ObjectCollision, void*>> Shapecast(CollisionShape shape, vec3 from, vec3 to, uint32_t collision_mask);

void Init();
void StepPhysics();
void DrawDebug();
void DrawDebug(bool);

}

#endif // TRAM_SDK_PHYSICS_API_H