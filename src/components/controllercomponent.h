// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// CONTROLLERCOMPONENTS.H -- Creature controller component
// This component controls the movement and physical actions of
// creatures (NPCs, monsters, players, and so on and so forth).

#ifndef CONTROLLERCOMPONENT_H
#define CONTROLLERCOMPONENT_H

#include <core.h>

// TODO: refactor so that this include can be replaced with forward declaration
#include <components/armaturecomponent.h>

namespace Core {
    class PhysicsComponent;
    //class ArmatureComponent;
    
    
    class ControllerComponent : public EntityComponent {
    public:
        enum Action : uint32_t {
            ACTION_IDLE,
            ACTION_MOVE_FORWARD,
            ACTION_MOVE_BACKWARD,
            ACTION_MOVE_LEFT,
            ACTION_MOVE_RIGHT,
            ACTION_TURN_FORWARD,
            ACTION_TURN_BACKWARD,
            ACTION_TURN_LEFT,
            ACTION_TURN_RIGHT,
            ACTION_JUMP,
            ACTION_CROUCH,
            ACTION_FORWARD_JUMP,
            ACTION_ACTIVATE,
            ACTION_MOVE_FORWARD_VERY_FAST,
            ACTION_MONGUS_WALK,
            ACTION_DAB,
            ACTION_STARE,
            ACTION_LOOK_BUSY
        };
        void Init(){};
        void Uninit(){};
        void Start(){}
        bool Remind(){return true;};
        void EventHandler(Event &event){};
        void Move(glm::vec3& direction);
        void GetLocation(glm::vec3& location);
        
        // TODO: change how act works (also add update method)
        void Act(Action action);

        bool IsInAir();

        void ActivateInFront();

        void SetAnimation(Action action, uint64_t animationName){animations[action] = animationName;}
        void SetPhysicsComponent(PhysicsComponent* comp){physcomp = comp;};
        void SetArmatureComponent(ArmatureComponent* comp){armcomp = comp;};
        void SetDirection(glm::vec3& dir){direction = dir;};
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
        uint64_t animations[30] = {0};
        uint64_t animplaying = 0;
    };
}

#endif //CONTROLLERCOMPONENT_H