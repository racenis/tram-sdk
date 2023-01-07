// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_CONTROLLERCOMPONENT_H
#define COMPONENTS_CONTROLLERCOMPONENT_H

#include <framework/core.h>
#include <framework/entitycomponent.h>

// TODO: refactor so that this include can be replaced with forward declaration
#include <components/armaturecomponent.h>

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
        void Init(){};
        void Start(){}
        bool Remind(){return true;};
        void EventHandler(Event &event){};
        void Move(glm::vec3& direction);
        void GetLocation(glm::vec3& location);
        
        void Act(Action action, ActionModifier modifier, uint32_t magnitude) {
            current_action = action;
            current_modifier = modifier;
            current_magnitude = magnitude;
            action_updated = true;
        }
        
        void Update();
        
        bool IsInAir();

        void ActivateInFront();

        void SetAnimation(Action action, uint64_t animationName){animations[action] = animationName;}
        void SetPhysicsComponent(PhysicsComponent* comp){physcomp = comp;};
        void SetArmatureComponent(ArmatureComponent* comp){armcomp = comp;};
        void SetDirection(glm::vec3& dir){direction = dir;};
        void SetDirection(const glm::quat& dir);
        
        // maybe create a unified function that updates all of the core components?
        static void UpdateAll() {
            for (auto& component : PoolProxy<ControllerComponent>::GetPool()) component.Update();
        }
        
    private:
        inline void Play (Action action){
            if (animations[action] == 0 || animations[action] == animplaying || armcomp == nullptr){
                return;
            } else {
                armcomp->StopAnimation(animplaying);
                armcomp->PlayAnimation(animations[action], 100, 1.0, 1.0);
                animplaying = animations[action];
            }
        }
        inline void PlayOnce (Action action){
            if (animations[action] == 0 || armcomp == nullptr){
                return;
            } else {
                armcomp->StopAnimation(animplaying);
                armcomp->PlayAnimation(animations[action], 1, 1.0, 1.0);
                animplaying = animations[action];
            }
        }
        PhysicsComponent* physcomp = nullptr;
        ArmatureComponent* armcomp = nullptr;
        glm::vec3 direction = glm::vec3(0.0f);
        glm::quat rot_direction = glm::vec3(0.0f, 0.0f, 0.0f);
        uint64_t animations[30] = {0};
        uint64_t animplaying = 0;
        
        Action current_action = ACTION_IDLE;
        ActionModifier current_modifier = ACTIONMODIFIER_NONE;
        uint32_t current_magnitude = 0;
        bool action_updated = false;
    };
}

#endif // COMPONENTS_CONTROLLERCOMPONENT_H