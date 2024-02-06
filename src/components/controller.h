// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_COMPONENTS_CONTROLLERCOMPONENT_H
#define TRAM_SDK_COMPONENTS_CONTROLLERCOMPONENT_H

#include <framework/core.h>
#include <framework/entitycomponent.h>

// TODO: refactor so that this include can be replaced with forward declaration
#include <components/animation.h>
#include <components/trigger.h>

namespace tram {

class PhysicsComponent;

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
    
    inline void SetWallCollisionCallback(void (*callback)(Physics::Collision)) {
        wallbonk_callback = callback;
    }

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
    
    bool crouching = false;
    bool running = false;
    
    id_t standing_on = 0;
    id_t standing_on_prev = 0;
    vec3 standing_pos;
    
    void (*wallbonk_callback)(Physics::Collision) = nullptr;
    
    template <typename> friend class Pool;
};

}

#endif // TRAM_SDK_COMPONENTS_CONTROLLERCOMPONENT_H