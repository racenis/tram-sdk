// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_COMPONENTS_PHYSICSCOMPONENT_H
#define TRAM_SDK_COMPONENTS_PHYSICSCOMPONENT_H

#include <physics/physics.h>
#include <physics/collisionmodel.h>
#include <physics/collisionshape.h>

class btCollisionShape;
class btMotionState;
class btRigidBody;
class btActionInterface;
class btTypedConstraint;

namespace tram {

struct PhysicsConstraint {
    PhysicsComponent* other;
    btTypedConstraint* constraint;
};

// TODO: hide all of BulletPhysics pointers
// it would be a good idea to hide away all of these pointers behind an opaque
// struct pointer

class PhysicsComponent : public EntityComponent {
public:
    PhysicsComponent() : collision_model (this) {}
    ~PhysicsComponent();
    void EventHandler(Event &event) {}
    void Start();

    name_t GetModel();
    void SetModel(name_t model);

    void SetMass(float mass);
    void SetShape(Physics::CollisionShape shape);

    void Push(vec3 direction);
    void Push(vec3 direction, vec3 local_position);
    void Spin(vec3 direction);

    void SetCollisionMask(uint32_t flags);
    void SetCollisionGroup(uint32_t flags);
    
    uint32_t GetCollisionMask();
    uint32_t GetCollisionGroup();

    void SetDebugDrawing(bool drawing);
    void SetKinematic(bool kinematic);
    void SetUpdateParentTransform(bool update);

    void SetRotation(quat rotation);
    void SetLocation(vec3 location);

    void DisableDeactivation();
    
    void Awaken();
    void Sleep();
    
    void SetAngularFactor(vec3 factor);
    void SetLinearFactor(vec3 factor);

    void SetVelocity(const vec3& velocity);
    vec3 GetVelocity();

    void AddPointConstraint(vec3 offset);
    void AddPointConstraint(vec3 offset, PhysicsComponent* other, vec3 other_offset);
    
    void RemoveConstraint(PhysicsComponent* other);
    void RemoveAllConstraints();
private:
    ResourceProxy<Physics::CollisionModel> collision_model;
    btCollisionShape* collision_shape = nullptr;
    btMotionState* motion_state = nullptr;
    btRigidBody* rigidbody = nullptr;
    btActionInterface* action = nullptr;
    std::vector<PhysicsConstraint> constraints;

    float rigidbody_mass = 1.0f;

    bool rigidbody_should_sleep = true;
    bool rigidbody_should_awake = false;
    bool update_parent_transform = true;
    
    vec3 rigidbody_position = {0.0f, 0.0f, 0.0f};
    quat rigidbody_rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    
    vec3 rigidbody_offset = {0.0f, 0.0f, 0.0f};
    vec3 rigidbody_angular_factor = {1.0f, 1.0f, 1.0f};
    vec3 rigidbody_linear_factor = {1.0f, 1.0f, 1.0f};

    uint32_t rigidbody_collision_mask = -1;
    uint32_t rigidbody_collision_group = -1;
    
    uint32_t rigidbody_collision_flags = 0;
};

}

#endif // TRAM_SDK_COMPONENTS_PHYSICSCOMPONENT_H