// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// CONTROLLERCOMPONENT.CPP -- Controller component implementation

#include <components/controllercomponent.h>
#include <physics.h>

namespace Core {
    void ControllerComponent::Move(glm::vec3& direction){
        if(physcomp == nullptr) return;
        glm::vec3 vel = direction * 7.0f;
        physcomp->Push(vel);
    };
    void ControllerComponent::GetLocation(glm::vec3& location){
        physcomp->GetParent()->GetLocation(location);
    }

    void ControllerComponent::Act(Action action){
        // this is stinky
        // this is VERY STINKY
        // i am ashame

        if(!physcomp) return;
        if(action == ACTION_IDLE){
            PlayOnce(ACTION_IDLE);
        }
        if(action == ACTION_MOVE_FORWARD_VERY_FAST){
            Play (ACTION_MOVE_FORWARD_VERY_FAST);
            glm::vec3 fwd = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::quat objrotation;
            physcomp->GetParent()->GetRotation(objrotation);

            fwd = objrotation * fwd;

            fwd = fwd* 2.0f;

            if(physcomp->GetVelocity() < 3.0f)physcomp->Push(fwd);
        }



        if(action == ACTION_MOVE_FORWARD || action == ACTION_MOVE_BACKWARD || action == ACTION_MOVE_LEFT || action == ACTION_MOVE_RIGHT){
            float spee = IsInAir() ?  25.0f : 70.0f;
            float velocity = physcomp->GetVelocity();

            if(velocity > 4.0f) spee *= 1 / ((velocity * 5) + 1);

            if (action == ACTION_MOVE_FORWARD)
                physcomp->Push(direction * spee);
            else if (action == ACTION_MOVE_BACKWARD)
                physcomp->Push(-direction * spee);
            else if (action == ACTION_MOVE_LEFT)
                physcomp->Push(glm::vec3(direction.z, direction.y, -direction.x) * spee);
            else if (action == ACTION_MOVE_RIGHT)
                physcomp->Push(glm::vec3(-direction.z, -direction.y, direction.x) * spee);
        } else if (action == ACTION_JUMP && !IsInAir()){
            physcomp->Push(glm::vec3(0.0f, 100.0f, 0.0f));
        } else if (action == ACTION_FORWARD_JUMP && !IsInAir()){
            physcomp->Push(glm::vec3(0.0f, 100.0f, 0.0f) + direction * 100.0f);
        }

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

