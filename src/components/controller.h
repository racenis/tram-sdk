// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_CONTROLLERCOMPONENT_H
#define TRAM_SDK_COMPONENTS_CONTROLLERCOMPONENT_H

#include <framework/core.h>
#include <framework/entitycomponent.h>
#include <physics/physics.h>

namespace tram {

class PhysicsComponent;
class TriggerComponent;

class ControllerComponent : public EntityComponent {
public:
    virtual void Move(vec3 local_direction) = 0;
    
    virtual void Run() = 0;
    virtual void Crouch() = 0;
    virtual void Fly() = 0;
    virtual void Jump() = 0;
    virtual void TurnLeft() = 0;
    virtual void TurnRight() = 0;
    
    virtual void Push(vec3 direction) = 0;

    inline void SetCollisions(bool collide) { this->collide = collide; }

    inline void SetLookDirection(quat direction) { look_direction = direction; }
    inline quat GetLookDirection() const { return look_direction; }
    
    inline void SetCollisionSize(float width, float height, float crouch_height) {
        collision_width = width;
        collision_height = height;
        collision_height_crouch = crouch_height;
    }
    
    inline void SetStepHeight(float height) {
        step_height = height;
    }
    
    inline void SetWallCollisionCallback(void (*callback)(ControllerComponent*, Physics::Collision)) {
        wallbonk_callback = callback;
    }
    
    inline void SetWalkSpeed(float speed) { this->walk_speed = speed; }
    inline void SetRunSpeed(float speed) { this->run_speed = speed; }
    inline void SetCrouchSpeed(float speed) { this->crouch_speed = speed; }
    inline void SetFlySpeed(float speed) { this->fly_speed = speed; }
    inline void SetFriction(float friction) { this->friction = friction; }
    inline void SetAcceleration(float acceleration) { this->acceleration = acceleration; }
    inline void SetCollisionGroup(uint32_t group) { this->collision_group = group; }
    
    static void Update();
protected:
    quat look_direction = {1.0f, 0.0f, 0.0f, 0.0f};
    bool collide = true;
    
    float walk_speed = 4.0f;
    float run_speed = 8.0f;
    float crouch_speed = 2.0f;
    float fly_speed = 16.0f;
    
    float friction = 0.89f;
    float acceleration = 1.0f;
    
    float collision_width = 0.35f;
    float collision_height = 1.85f;
    float collision_height_crouch = 0.75f;
    
    float step_height = 0.35f;
    
    uint32_t collision_group = Physics::COLL_PLAYER;
    void (*wallbonk_callback)(ControllerComponent*, Physics::Collision) = nullptr;
};

class FPSControllerComponent : public ControllerComponent {
public:
    static FPSControllerComponent* Make();
    static void Yeet(FPSControllerComponent* component);
    
    void Push(vec3 direction) override;
    void Move(vec3 local_direction) override;
    
    void Run() override;
    void Crouch() override;
    void Fly() override;
    void Jump() override;
    void TurnLeft() override;
    void TurnRight() override;
    
    void EventHandler(Event &event) override {}
protected:
    FPSControllerComponent() = default;
    ~FPSControllerComponent() = default;
    
    Component<TriggerComponent> walk_collision;
    Component<TriggerComponent> crouch_collision;
    
    void ApplyDynamics();
    void RecoverFromCollisions();
    void ResetMove();
    
    void Start() override;
    
    vec3 velocity = {0.0f, 0.0f, 0.0f};
    vec3 move_direction = {0.0f, 0.0f, 0.0f};
    
    bool is_in_air = false;
    
    bool crouching = false;
    bool squished = false;
    bool running = false;
    bool flying = false;
    
    id_t standing_on = 0;
    id_t standing_on_prev = 0;
    vec3 standing_pos;
    quat standing_rot;
    
    friend class ControllerComponent;
};

class RaycastControllerComponent : public ControllerComponent {
public:
    static RaycastControllerComponent* Make();
    static void Yeet(RaycastControllerComponent* component);

    void Push(vec3 direction) override;
    void Move(vec3 local_direction) override;
    
    void Run() override;
    void Crouch() override;
    void Fly() override;
    void Jump() override;
    void TurnLeft() override;
    void TurnRight() override;
    
    void EventHandler(Event &event) override {}
protected:
    RaycastControllerComponent() = default;
    ~RaycastControllerComponent() = default;
    
    Component<TriggerComponent> wall_collision;
    
    void ApplyDynamics();
    void RecoverFromCollisions();
    void ResetMove();
    
    void Start() override;
    
    vec3 move_magnitude = {0.0f, 0.0f, 0.0f};
    vec3 move_direction = {0.0f, 0.0f, 0.0f};
    
    vec3 air_velocity = {0.0f, 0.0f, 0.0f};

    bool enabled = false;

    bool is_in_air = false;
    
    bool crouching = false;
    bool running = false;
    bool flying = false;
    
    int jump_frames = 0;
    
    id_t standing_on = 0;
    id_t standing_on_prev = 0;
    vec3 standing_pos;
    
    friend class ControllerComponent;
};

}

#endif // TRAM_SDK_COMPONENTS_CONTROLLERCOMPONENT_H