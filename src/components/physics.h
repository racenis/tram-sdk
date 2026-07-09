// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_PHYSICSCOMPONENT_H
#define TRAM_SDK_COMPONENTS_PHYSICSCOMPONENT_H

#include <physics/physics.h>
#include <physics/collisionmodel.h>
#include <physics/collisionshape.h>

namespace tram {

class PhysicsComponent : public EntityComponent {
public:
    static PhysicsComponent* Make();
    static void Yeet(PhysicsComponent* component);

    Physics::CollisionModel* GetModel() const;
    [[deprecated]] void SetModel(name_t model);
    void SetModel(Physics::CollisionModel* model);

    void SetMass(float mass);
    void SetShape(Physics::CollisionShape shape);

    void Push(vec3 direction);
    void Push(vec3 direction, vec3 local_position);
    void Spin(vec3 direction);

    void SetCollisionMask(uint32_t flags);
    void SetCollisionGroup(uint32_t flags);
    
    uint32_t GetCollisionMask() const;
    uint32_t GetCollisionGroup() const;

    void SetStoreCollisions(bool store_collisions);

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
    vec3 GetVelocity() const;
    
    const std::vector<Physics::Collision>& GetStoredCollisions() const { return stored_collisions; }
    
    void EventHandler(Event &event) {}
private:
    ResourceProxy<Physics::CollisionModel> model;
    Physics::collisionshape_t shape = {nullptr};
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
    
    bool store_collisions = false;
    
    void Start();
    
    void Collision(const Physics::Collision& collision);
    void ResetCollisions();
    
    friend void Physics::Update();
    
    PhysicsComponent() : model(this) {}
    ~PhysicsComponent();
    
    std::vector<Physics::Collision> stored_collisions;
};

}

#endif // TRAM_SDK_COMPONENTS_PHYSICSCOMPONENT_H