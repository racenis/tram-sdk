// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_COMPONENTS_PHYSICSCOMPONENT_H
#define TRAM_SDK_COMPONENTS_PHYSICSCOMPONENT_H

#include <physics/physics.h>
#include <physics/collisionmodel.h>
#include <physics/collisionshape.h>

namespace tram {

class PhysicsComponent : public EntityComponent {
public:
    PhysicsComponent() : collision_model(this) {}
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
private:
    ResourceProxy<Physics::CollisionModel> collision_model;
    Physics::collisionshape_t collision_shape = {nullptr};
    Physics::rigidbody_t rigidbody = {nullptr};

    float rigidbody_mass = 1.0f;

    bool rigidbody_should_sleep = true;
    bool rigidbody_should_awake = false;
    bool update_parent_transform = true;
    bool rigidbody_kinematic = false;
    bool rigidbody_debug_draw = true;
    
    vec3 rigidbody_position = {0.0f, 0.0f, 0.0f};
    quat rigidbody_rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    
    vec3 rigidbody_offset = {0.0f, 0.0f, 0.0f};
    vec3 rigidbody_angular_factor = {1.0f, 1.0f, 1.0f};
    vec3 rigidbody_linear_factor = {1.0f, 1.0f, 1.0f};

    uint32_t rigidbody_collision_mask = -1;
    uint32_t rigidbody_collision_group = -1;
};

}

#endif // TRAM_SDK_COMPONENTS_PHYSICSCOMPONENT_H