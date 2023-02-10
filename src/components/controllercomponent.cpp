// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

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
    //void ControllerComponent::SetDirection(const glm::quat& dir) {
    //    physcomp->SetRotation(dir);
    //}
    void ControllerComponent::Update() {
        if(!physcomp) return;
        
        if(current_action == ACTION_IDLE){
            if (action_updated) PlayOnce(ACTION_IDLE);
            //action_updated = false;
        }
        
        if(current_action == ACTION_LIVESEY){
            if (action_updated) Play (ACTION_LIVESEY);
            glm::vec3 fwd = glm::vec3(0.0f, 0.0f, -1.0f);
            glm::quat objrotation;
            physcomp->GetParent()->GetRotation(objrotation);

            fwd = objrotation * fwd;

            fwd = fwd* 2.0f;

            if(glm::length(physcomp->GetVelocity()) < 3.0f)physcomp->Push(fwd);
            //action_updated = false;
        }
        
        if(current_action == ACTION_WALK){
            // TODO: fix IsInAir() and then uncomment line below
            //float spee = 70.0f;//IsInAir() ?  25.0f : 70.0f;
            //float velocity = glm::length(physcomp->GetVelocity());

            //if(velocity > 4.0f) spee *= 1 / ((velocity * 5) + 1);
            
            glm::vec3 move_direction = glm::vec3(0.0f, 0.0f, 0.0f);

            if (current_modifier == ACTIONMODIFIER_FORWARD || current_modifier == ACTIONMODIFIER_FORWARD_LEFT || current_modifier == ACTIONMODIFIER_FORWARD_RIGHT)
                move_direction += DIRECTION_FORWARD;
            if (current_modifier == ACTIONMODIFIER_BACKWARD || current_modifier == ACTIONMODIFIER_BACKWARD_LEFT || current_modifier == ACTIONMODIFIER_BACKWARD_RIGHT)
                move_direction -= DIRECTION_FORWARD;
            if (current_modifier == ACTIONMODIFIER_LEFT || current_modifier == ACTIONMODIFIER_FORWARD_LEFT || current_modifier == ACTIONMODIFIER_BACKWARD_LEFT)
                move_direction -= DIRECTION_SIDE;
            if (current_modifier == ACTIONMODIFIER_RIGHT || current_modifier == ACTIONMODIFIER_FORWARD_RIGHT || current_modifier == ACTIONMODIFIER_BACKWARD_RIGHT)
                move_direction += DIRECTION_SIDE;
            
            glm::vec3 wish_dir = glm::normalize(parent->GetRotation() * move_direction);
            if (std::isnan(wish_dir.x) || std::isnan(wish_dir.y) || std::isnan(wish_dir.z)) wish_dir = glm::vec3(0.0f, 0.0f, 0.0f);
            
            float current_speed = glm::dot(velocity, wish_dir); 
            float add_speed = 0.1f - current_speed;
            add_speed = add_speed < 0.0f ? 0.0f : add_speed;
            add_speed = add_speed > 0.02f ? 0.02f : add_speed;
            
            std::cout << current_speed << " : " << add_speed << std::endl;
            
            auto add_velocity = wish_dir * add_speed;
            
            velocity.x += add_velocity.x;
            velocity.z += add_velocity.z;
            
            //physcomp->PushLocal(direction_normalized * spee);
            action_updated = true;
        }
        
        // check if character is in the air
        triggercomp->SetLocation(parent->GetLocation() - vec3(0.0f, 0.1f, 0.0f));
        if (triggercomp->Poll().size() > 0) { // if on the ground
            velocity.y = 0.0f;   // make the character not fall through the ground
            velocity *= 0.95f;   // friction
        } else if (action_updated) { // if in air
            velocity.y -= 0.0037f; // make the character fall
        }
        
        if (current_action == ACTION_JUMP){
            triggercomp->SetLocation(parent->GetLocation() - vec3(0.0f, 0.1f, 0.0f));
            if (triggercomp->Poll().size() > 0) {
                velocity.y += 0.12f;
            }
            action_updated = true;
        }

        

        auto p_loc = parent->GetLocation();
        auto p_rot = parent->GetRotation();
        
        auto new_loc = p_loc + velocity;
        
        triggercomp->SetLocation(new_loc);
        if (triggercomp->Poll().size() > 0) {            
            vec3 average_normal = vec3(0.0f, 0.0f, 0.0f);
            auto poll = triggercomp->Poll();
            for (auto& coll : poll) {
                average_normal += coll.normal;
                Render::AddLine(coll.point, coll.point+coll.normal, Render::COLOR_CYAN);
                
                auto v_dir = glm::normalize(velocity);
                auto s_dir = glm::normalize(coll.normal);
                
                auto n_dir = glm::normalize(v_dir - s_dir);
                
                auto n_vel = n_dir * glm::length(velocity);
                
                //float not_goodness = glm::dot (v_dir, coll.normal);
                //if (std::isnan(not_goodness)) not_goodness = 0.0f;
                
                //std::cout << not_goodness << std::endl;
                
                //Render::AddLine(coll.point, coll.point+glm::normalize(velocity*not_goodness), Render::COLOR_GREEN);
                Render::AddLine(coll.point, coll.point + n_dir, Render::COLOR_GREEN);
                
                if (std::isnan(n_vel.x) || std::isnan(n_vel.z)) n_vel = vec3(0.0f, 0.0f, 0.0f);
                
                //velocity.x = n_vel.x;
                //velocity.z = n_vel.z;
                velocity = n_vel;
                
                //velocity = vec3(0.0f, 0.0f, 0.0f);
                //velocity *= not_goodness;
                break;
            }
            average_normal /= poll.size();
            
            
        }
        
        p_loc += velocity;
        
        parent->UpdateTransform(p_loc, p_rot);

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
        //std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;

        glm::vec3 direction;
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

