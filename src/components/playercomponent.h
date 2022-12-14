// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_PLAYERCOMPONENT_H
#define COMPONENTS_PLAYERCOMPONENT_H

#include <framework/ui.h>
#include <framework/event.h>
#include <framework/entity.h>
#include <framework/worldcell.h>

#include <components/controllercomponent.h>

#include <framework/logging.h>

namespace Core {
    class PlayerComponent : public EntityComponent {
    public:
        void EventHandler(Event &event){
            using namespace Core::UI;
            using enum Core::ControllerComponent::Action;
            using enum Core::ControllerComponent::ActionModifier;

            if (event.type == Event::CURSORPOS) {
                controller->SetDirection(glm::quat(glm::vec3(0.0f, -glm::radians(CAMERA_YAW), 0.0f)));
                return;
            }

            if (is_move && (event.type == Event::KEYDOWN || event.type == Event::KEYUP)) {
                bool move_value = event.type == Event::KEYDOWN;
                
                if (event.subtype == KEY_ACTION_FORWARD)    move_forward =  move_value;
                if (event.subtype == KEY_ACTION_BACKWARD)   move_backward = move_value;
                if (event.subtype == KEY_ACTION_LEFT)       move_left =     move_value;
                if (event.subtype == KEY_ACTION_RIGHT)      move_right =    move_value;
            }

            // I have literally no idea what is going on in here

            auto action = ACTION_IDLE;
            auto modifier = ACTIONMODIFIER_NONE;

            if (move_forward) {
                action = ACTION_WALK;
                modifier = ACTIONMODIFIER_FORWARD;
            }
            
            if (move_backward) {
                action = ACTION_WALK;
                modifier = ACTIONMODIFIER_BACKWARD;
            }
            
            if (move_left) {
                action = ACTION_WALK;
                modifier = ACTIONMODIFIER_LEFT;
            }
            
            if (move_right) {
                action = ACTION_WALK;
                modifier = ACTIONMODIFIER_RIGHT;
            }
            
            if (move_forward && move_left) {
                action = ACTION_WALK;
                modifier = ACTIONMODIFIER_FORWARD_LEFT;
            }
            
            if (move_forward && move_right) {
                action = ACTION_WALK;
                modifier = ACTIONMODIFIER_FORWARD_RIGHT;
            }
            
            if (move_backward && move_left) {
                action = ACTION_WALK;
                modifier = ACTIONMODIFIER_BACKWARD_LEFT;
            }
            
            if (move_backward && move_right) {
                action = ACTION_WALK;
                modifier = ACTIONMODIFIER_BACKWARD_RIGHT;
            }
        
            if (event.subtype == KEY_ACTION_JUMP) {
                action = ACTION_JUMP;
                modifier = ACTIONMODIFIER_NONE;
            }
            
            controller->Act(action, modifier, 0);
        }
        void Init(){
            listener = Event::AddListener(Event::KEYPRESS, this);
            keydown = Event::AddListener(Event::KEYDOWN, this);
            keyup = Event::AddListener(Event::KEYUP, this);
            mouselistener = Event::AddListener(Event::CURSORPOS, this);
            cell_loader = PoolProxy<WorldCell::Loader>::New();
            cell_loader->SetLocation(parent->GetLocation());
        }
        ~PlayerComponent(){
            Event::RemoveListener(listener);
            listener = 0;
            Event::RemoveListener(keydown);
            keydown = 0;
            Event::RemoveListener(keyup);
            keyup = 0;
            Event::RemoveListener(mouselistener);
            mouselistener = 0;
            PoolProxy<WorldCell::Loader>::Delete(cell_loader);
            cell_loader = nullptr;
        }
        void SetParent(Entity* ent){
            parent = ent;
        }
        void SetControllerComponent (ControllerComponent* comp){controller = comp;};
        void SetVehicle (uint64_t vehicleID){vehicle = vehicleID;};
        void IgnoreMove(){
            is_move = false;
        }
        void StopIgnoreMove(){
            is_move = true;
        }
        void Start(){}
        void MoveUpdate() { assert(cell_loader); cell_loader->UpdateLocation(parent->GetLocation()); }
    private:
        Entity* parent = nullptr;
        listener_t keydown = 0;
        listener_t keyup = 0;
        listener_t listener = 0;
        listener_t mouselistener = 0;
        ControllerComponent* controller = nullptr;
        WorldCell::Loader* cell_loader = nullptr;

        uint64_t vehicle = 0;

        const glm::vec3 forwarddir = glm::vec3(0.0f, 0.0f, -1.0f);
        const glm::vec3 sidedir = glm::vec3(1.0f, 0.0f, 0.0f);
        const glm::vec3 updir = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::quat direction;
        
        bool move_forward = false;
        bool move_backward = false;
        bool move_left = false;
        bool move_right = false;

        bool is_move = true;
    };
    
}

#endif // COMPONENTS_PLAYERCOMPONENT_H