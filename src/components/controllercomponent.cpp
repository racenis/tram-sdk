// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// CONTROLLERCOMPONENT.CPP -- Controller component implementation

#include <framework/entity.h>
#include <framework/message.h>

#include <components/controllercomponent.h>
#include <components/physicscomponent.h>

namespace Core {
    void ControllerComponent::Move(glm::vec3& direction){
        if(physcomp == nullptr) return;
        glm::vec3 vel = direction * 7.0f;
        physcomp->Push(vel);
    };
    void ControllerComponent::GetLocation(glm::vec3& location){
        physcomp->GetParent()->GetLocation(location);
    }
    void ControllerComponent::SetDirection(const glm::quat& dir) {
        physcomp->SetRotation(dir);
    }
    void ControllerComponent::Update(){
        if(!physcomp) return;
        
        if(current_action == ACTION_IDLE){
            if (action_updated) PlayOnce(ACTION_IDLE);
            action_updated = false;
        }
        
        if(current_action == ACTION_LIVESEY){
            if (action_updated) Play (ACTION_LIVESEY);
            glm::vec3 fwd = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::quat objrotation;
            physcomp->GetParent()->GetRotation(objrotation);

            fwd = objrotation * fwd;

            fwd = fwd* 2.0f;

            if(physcomp->GetVelocity() < 3.0f)physcomp->Push(fwd);
            action_updated = false;
        }
        
        if(current_action == ACTION_WALK){
            // TODO: fix IsInAir() and then uncomment line below
            float spee = 70.0f;//IsInAir() ?  25.0f : 70.0f;
            float velocity = physcomp->GetVelocity();

            if(velocity > 4.0f) spee *= 1 / ((velocity * 5) + 1);
            
            glm::vec3 move_direction = glm::vec3(0.0f, 0.0f, 0.0f);

            if (current_modifier == ACTIONMODIFIER_FORWARD || current_modifier == ACTIONMODIFIER_FORWARD_LEFT || current_modifier == ACTIONMODIFIER_FORWARD_RIGHT)
                move_direction += DIRECTION_FORWARD;
            if (current_modifier == ACTIONMODIFIER_BACKWARD || current_modifier == ACTIONMODIFIER_BACKWARD_LEFT || current_modifier == ACTIONMODIFIER_BACKWARD_RIGHT)
                move_direction -= DIRECTION_FORWARD;
            if (current_modifier == ACTIONMODIFIER_LEFT || current_modifier == ACTIONMODIFIER_FORWARD_LEFT || current_modifier == ACTIONMODIFIER_BACKWARD_LEFT)
                move_direction += DIRECTION_SIDE;
            if (current_modifier == ACTIONMODIFIER_RIGHT || current_modifier == ACTIONMODIFIER_FORWARD_RIGHT || current_modifier == ACTIONMODIFIER_BACKWARD_RIGHT)
                move_direction -= DIRECTION_SIDE;
            
            glm::vec3 direction_normalized = glm::normalize(glm::vec3(move_direction.x, 0.0f, move_direction.z));
            if (std::isnan(direction_normalized.x) || std::isnan(direction_normalized.y) || std::isnan(direction_normalized.z)) direction_normalized = glm::vec3(0.0f, 0.0f, 0.0f);
            
            physcomp->PushLocal(direction_normalized * spee);
            action_updated = false;
        } else if (current_action == ACTION_JUMP && !IsInAir()){
            physcomp->Push(DIRECTION_UP * 100.0f);
            action_updated = false;
        }/* else if (current_action == ACTION_FORWARD_JUMP && !IsInAir()){
            physcomp->Push(glm::vec3(0.0f, 100.0f, 0.0f) + direction_normalized * 100.0f);
        }*/

    }

    bool ControllerComponent::IsInAir(){
        glm::vec3 f, u, h, c;

        if(!physcomp) return false;
        physcomp->GetParent()->GetLocation(f);
        physcomp->GetParent()->GetLocation(u);
        physcomp->GetAABB(h, c);
        u.y = u.y - 0.2f - ((c.y - h.y) / 2.0f);

        bool inAir = !Physics::Raycast(f, u);
        return inAir;
    }

    void ControllerComponent::ActivateInFront(){
        std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;

        glm::vec3 location;
        physcomp->GetParent()->GetLocation(location);

        location += glm::vec3(0.0f, 0.775f, 0.0f); // stinky hack

        PhysicsComponent* activatable = Physics::Raycast(location, location + direction * 3.0f);
        std::cout << activatable << std::endl;
        if (activatable){
            Message msg;

            msg.type = Message::ACTIVATE;
            msg.receiverID = activatable->GetParent()->GetID();
            msg.senderID = physcomp->GetParent()->GetID();

            Message::Send(msg);
        }
    }
}

