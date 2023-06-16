// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_CONTROLLERCOMPONENT_H
#define COMPONENTS_CONTROLLERCOMPONENT_H

#include <framework/core.h>
#include <framework/entitycomponent.h>

// TODO: refactor so that this include can be replaced with forward declaration
#include <components/animation.h>
#include <components/trigger.h>

namespace tram {

class PhysicsComponent;

class ControllerComponent : public EntityComponent {
public:
    enum Action : uint8_t {
        ACTION_IDLE,        //< Standing around, doing nothing.
        ACTION_WALK,        //< Walking, normal speed.
        ACTION_RUN,         //< Walking, fast speed.
        ACTION_JUMP,        //< Increases vertical velocity.
        ACTION_CROUCH,      //< Walking, slow speed, also lower collision box.
        ACTION_CROUCH_IDLE, //< Standing around, doing nothing, also lower collision box.
        ACTION_ACTIVATE,
        ACTION_TURN,
        ACTION_DAB,
        ACTION_STARE,
        ACTION_LIVESEY,
        ACTION_LOOK_BUSY
    };
    
    enum ActionModifier : uint8_t {
        ACTIONMODIFIER_NONE,
        ACTIONMODIFIER_FORWARD,
        ACTIONMODIFIER_BACKWARD,
        ACTIONMODIFIER_LEFT,
        ACTIONMODIFIER_RIGHT,
        ACTIONMODIFIER_FORWARD_RIGHT,
        ACTIONMODIFIER_FORWARD_LEFT,
        ACTIONMODIFIER_BACKWARD_LEFT,
        ACTIONMODIFIER_BACKWARD_RIGHT
    };
    
    void Start();
    void EventHandler(Event &event) {}
    
    void Act(Action action, ActionModifier modifier);
    
    inline Action GetCurrentAction() { return current_action; }
    inline ActionModifier GetCurrentActionModifier() { return current_modifier; }

    inline void SetCollisionSize(float width, float height, float crouch_height) {
        collision_width = width;
        collision_height = height;
        collision_height_crouch = crouch_height;
    }
    
    inline void SetStepHeight(float height, float crouch_height) {
        step_height = height;
        step_height_crouch = crouch_height;
    }

    static void Update();
protected:
    ControllerComponent() = default;
    ~ControllerComponent() = default;
    
    Component<TriggerComponent> walk_collision;
    Component<TriggerComponent> crouch_collision;
    
    Component<PhysicsComponent> physics_body;
    
    void Perform();
    
    vec3 velocity = {0.0f, 0.0f, 0.0f};

    Action current_action = ACTION_IDLE;
    ActionModifier current_modifier = ACTIONMODIFIER_NONE;
    
    bool is_in_air = false;
    
    float collision_width = 0.35f;
    float collision_height = 1.85f;
    float collision_height_crouch = 0.5f;
    
    float step_height = 0.35f;
    float step_height_crouch = 0.15f;
    
    float walk_speed = 0.1f;
    float run_speed = 0.2f;
    float crouch_speed = 0.05f;
    
    template <typename> friend class Pool;
};

}

#endif // COMPONENTS_CONTROLLERCOMPONENT_H