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

            if(is_move && event.type == Event::KEYPRESS){
                glm::vec3 vecdir = direction * forwarddir;
                controller->SetDirection(vecdir);


                if(event.subtype & KEY_FORWARD) controller->Act(ACTION_MOVE_FORWARD);
                if(event.subtype & KEY_BACKWARD) controller->Act(ACTION_MOVE_BACKWARD);
                if(event.subtype & KEY_LEFT) controller->Act(ACTION_MOVE_LEFT);
                if(event.subtype & KEY_RIGHT) controller->Act(ACTION_MOVE_RIGHT);
                if(event.subtype & KEY_JUMP && !(event.subtype & KEY_CROUCH)) controller->Act(ACTION_JUMP);
                if(event.subtype & KEY_CROUCH && event.subtype & KEY_JUMP) controller->Act(ACTION_FORWARD_JUMP);


            } else if (event.type == Event::KEYDOWN) {
                if(event.subtype & KEY_ACTIVATE) controller->ActivateInFront();

            } else if (event.type == Event::CURSORPOS){
                direction = glm::quat(glm::vec3(-glm::radians(Render::CAMERA_PITCH), -glm::radians(Render::CAMERA_YAW), 0.0f));

            } else if (!is_move && event.type == Event::KEYPRESS && (event.subtype & KEY_JUMP && event.subtype & KEY_CROUCH)){
                Message msg;
                msg.type = Message::ACTIVATE;
                msg.senderID = parent->GetID();
                msg.receiverID = vehicle;
                Message::Send(msg);
            }


        };
        void Init(){
            listener = Event::AddListener(Event::KEYPRESS);
            listener->ent = this;
            keydown = Event::AddListener(Event::KEYDOWN);
            keydown->ent = this;
            mouselistener = Event::AddListener(Event::CURSORPOS);
            mouselistener->ent = this;
        }
        void Uninit(){
            Event::RemoveListener(Event::KEYPRESS, listener);
            listener = nullptr;
            Event::RemoveListener(Event::KEYDOWN, keydown);
            keydown = nullptr;
            Event::RemoveListener(Event::CURSORPOS, mouselistener);
            mouselistener = nullptr;
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
    private:
        Entity* parent = nullptr;
        Event::Listener* keydown = nullptr;
        Event::Listener* listener = nullptr;
        Event::Listener* mouselistener = nullptr;
        ControllerComponent* controller = nullptr;

        uint64_t vehicle = 0;

        const glm::vec3 forwarddir = glm::vec3(0.0f, 0.0f, -1.0f);
        const glm::vec3 sidedir = glm::vec3(1.0f, 0.0f, 0.0f);
        const glm::vec3 updir = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::quat direction;

        bool is_move = true;
    };
    
}

#endif //PLAYERCOMPONENT_H