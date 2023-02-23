// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_CONTROLLERCOMPONENT_H
#define COMPONENTS_CONTROLLERCOMPONENT_H

#include <framework/core.h>
#include <framework/entitycomponent.h>

// TODO: refactor so that this include can be replaced with forward declaration
#include <components/armaturecomponent.h>
#include <components/triggercomponent.h>

namespace Core {
    class PhysicsComponent;
    //class ArmatureComponent;
    
    
    class ControllerComponent : public EntityComponent {
    public:
        enum Action : uint8_t {
            ACTION_IDLE,
            ACTION_WALK,
            ACTION_RUN,
            ACTION_JUMP,
            ACTION_CROUCH,
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
        
        void Init () {}
        void Start () {}
        void EventHandler (Event &event) {}
        
        void Move (glm::vec3& direction);
        
        void Act (Action action, ActionModifier modifier, uint32_t magnitude);
        
        inline Action GetCurrentAction () { return current_action; }
        inline ActionModifier GetCurrentActionModifier () { return current_modifier; }
        inline uint32_t GetMagnitude () { return current_magnitude; }
        
        inline void SetPhysicsComponent (PhysicsComponent* comp) { physcomp = comp; }
        inline void SetArmatureComponent (ArmatureComponent* comp) { armcomp = comp; }
        inline void SetTriggerComponent (TriggerComponent* comp) { triggercomp = comp; }

        static void Update();
    private:
        void Perform();
        
        PhysicsComponent* physcomp = nullptr;
        ArmatureComponent* armcomp = nullptr;
        TriggerComponent* triggercomp = nullptr;

        vec3 velocity = vec3 (0.0f, 0.0f, 0.0f);

        Action current_action = ACTION_IDLE;
        ActionModifier current_modifier = ACTIONMODIFIER_NONE;
        uint32_t current_magnitude = 0;
        bool action_updated = false;
    protected:
        ControllerComponent() = default;
        ~ControllerComponent() = default;
        
        template <typename> friend class Pool;
    };
}

#endif // COMPONENTS_CONTROLLERCOMPONENT_H