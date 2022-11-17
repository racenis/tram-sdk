// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// PLAYERCOMPONENTS.H -- Player component
// This component implements the specific behaviours of the
// player entity.

#ifndef PLAYERCOMPONENT_H
#define PLAYERCOMPONENT_H

#include <components/controllercomponent.h>
#include <ui.h>

namespace Core {
    class PlayerComponent : public EntityComponent {
    public:
        void EventHandler(Event &event){
            using namespace Core::UI;
            using enum Core::ControllerComponent::Action;
            using enum Core::ControllerComponent::ActionModifier;

            if (event.type == Event::CURSORPOS) {
                controller->SetDirection(glm::quat(glm::vec3(0.0f, -glm::radians(Render::CAMERA_YAW), 0.0f)));
                return;
            }

            if (is_move && (event.type == Event::KEYDOWN || event.type == Event::KEYUP)) {
                bool move_value = event.type == Event::KEYDOWN;
                
                if (event.subtype & KEY_FORWARD) move_forward = move_value;
                if (event.subtype & KEY_BACKWARD) move_backward = move_value;
                if (event.subtype & KEY_LEFT) move_left = move_value;
                if (event.subtype & KEY_RIGHT) move_right = move_value;
            }
                
                

            //} else if (event.type == Event::KEYDOWN) {
            //    if(event.subtype & KEY_ACTIVATE) controller->ActivateInFront();



            //glm::vec3 vecdir = direction * forwarddir;
            //controller->SetDirection(vecdir);
            //controller->SetRotDirection(direction);

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
        
            if (event.subtype & KEY_JUMP) {
                action = ACTION_JUMP;
                modifier = ACTIONMODIFIER_NONE;
            }
            
            controller->Act(action, modifier, 0);
            
            //std::cout << "action: " << (uint64_t)action << " modifier: " << (uint64_t)modifier << std::endl;
            //std::cout << "fw " << move_forward << " bw " << move_backward << " l " << move_left << " r " << move_right << std::endl;
        };
        void Init(){
            listener = Event::AddListener(Event::KEYPRESS);
            listener->ent = this;
            keydown = Event::AddListener(Event::KEYDOWN);
            keydown->ent = this;
            keyup = Event::AddListener(Event::KEYUP);
            keyup->ent = this;
            mouselistener = Event::AddListener(Event::CURSORPOS);
            mouselistener->ent = this;
            cell_loader = PoolProxy<WorldCell::Loader>::New();
            cell_loader->SetLocation(parent->GetLocation());
        }
        ~PlayerComponent(){
            Event::RemoveListener(listener);
            listener = nullptr;
            Event::RemoveListener(keydown);
            keydown = nullptr;
            Event::RemoveListener(keyup);
            keyup = nullptr;
            Event::RemoveListener(mouselistener);
            mouselistener = nullptr;
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
        //void UpdateLocation(const glm::vec3& location) { assert(cell_loader); cell_loader->SetLocation(location); }
        void MoveUpdate() { assert(cell_loader); cell_loader->UpdateLocation(parent->GetLocation()); }
    private:
        Entity* parent = nullptr;
        Event::Listener* keydown = nullptr;
        Event::Listener* keyup = nullptr;
        Event::Listener* listener = nullptr;
        Event::Listener* mouselistener = nullptr;
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

#endif //PLAYERCOMPONENT_H