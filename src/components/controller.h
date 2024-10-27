// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

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
    void Start();
    void EventHandler(Event &event) {}
    
    void Move(vec3 local_direction);
    
    void Run();
    void Crouch();
    void Jump();
    void TurnLeft();
    void TurnRight();
    
    void Push(vec3 direction);
    
    inline void SetLookDirection(quat direction) { look_direction = direction; }
    inline quat GetLookDirection() const { return look_direction; }
    
    inline void SetCollisionSize(float width, float height, float crouch_height) {
        collision_width = width;
        collision_height = height;
        collision_height_crouch = crouch_height;
    }
    
    inline void SetStepHeight(float height, float crouch_height) {
        step_height = height;
        step_height_crouch = crouch_height;
    }
    
    inline void SetWallCollisionCallback(void (*callback)(ControllerComponent*, Physics::Collision)) {
        wallbonk_callback = callback;
    }
    
    inline void SetWalkSpeed(float walk_speed) { this->walk_speed = walk_speed; }
    inline void SetRunSpeed(float run_speed) { this->run_speed = run_speed; }
    inline void SetCrouchSpeed(float crouch_speed) { this->crouch_speed = crouch_speed; }
    inline void SetFriction(float friction) { this->friction = friction; }
    inline void SetCollisionGroup(uint32_t group) { this->collision_group = group; }
    
    static bool IsDebugInfoDraw();
    static void SetDebugInfoDraw(bool);
    static void Update();
protected:
    ControllerComponent() = default;
    ~ControllerComponent() = default;
    
    Component<TriggerComponent> walk_collision;
    Component<TriggerComponent> crouch_collision;
    
    Component<PhysicsComponent> physics_body;
    
    void ApplyDynamics();
    void RecoverFromCollisions();
    void ResetMove();
    
    vec3 velocity = {0.0f, 0.0f, 0.0f};
    vec3 move_direction = {0.0f, 0.0f, 0.0f};
    quat look_direction = {1.0f, 0.0f, 0.0f, 0.0f};

    bool is_in_air = false;
    
    float collision_width = 0.35f;
    float collision_height = 1.85f;
    float collision_height_crouch = 0.5f;
    
    float step_height = 0.35f;
    float step_height_crouch = 0.15f;
    
    float walk_speed = 0.1f;
    float run_speed = 0.2f;
    float crouch_speed = 0.05f;
    
    float friction = 0.89f;
    
    bool crouching = false;
    bool running = false;
    
    id_t standing_on = 0;
    id_t standing_on_prev = 0;
    vec3 standing_pos;
    
    uint32_t collision_group = Physics::COLL_PLAYER;
    
    void (*wallbonk_callback)(ControllerComponent*, Physics::Collision) = nullptr;
    
    template <typename> friend class Pool;
};

}

#endif // TRAM_SDK_COMPONENTS_CONTROLLERCOMPONENT_H