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
       
        float boost = 0.0f;
 
        // check if character is in the air
        triggercomp->SetLocation(parent->GetLocation() - vec3(0.0f, 0.1f, 0.0f));
        if (triggercomp->Poll().size() > 0) { // if on the ground            
            velocity.y = 0.0f;
            velocity *= 0.89f;   // friction
        } else if (action_updated) { // if in air
            velocity.y -= 0.0037f; // make the character fall
        }
        
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
            glm::vec3 move_direction = glm::vec3(0.0f, 0.0f, 0.0f);

            // compute the move direction
            if (current_modifier == ACTIONMODIFIER_FORWARD || current_modifier == ACTIONMODIFIER_FORWARD_LEFT || current_modifier == ACTIONMODIFIER_FORWARD_RIGHT)
                move_direction += DIRECTION_FORWARD;
            if (current_modifier == ACTIONMODIFIER_BACKWARD || current_modifier == ACTIONMODIFIER_BACKWARD_LEFT || current_modifier == ACTIONMODIFIER_BACKWARD_RIGHT)
                move_direction -= DIRECTION_FORWARD;
            if (current_modifier == ACTIONMODIFIER_LEFT || current_modifier == ACTIONMODIFIER_FORWARD_LEFT || current_modifier == ACTIONMODIFIER_BACKWARD_LEFT)
                move_direction -= DIRECTION_SIDE;
            if (current_modifier == ACTIONMODIFIER_RIGHT || current_modifier == ACTIONMODIFIER_FORWARD_RIGHT || current_modifier == ACTIONMODIFIER_BACKWARD_RIGHT)
                move_direction += DIRECTION_SIDE;
            
            // clip the move direction so that it doesn't exceed maximum velocity
            glm::vec3 wish_dir = glm::normalize(parent->GetRotation() * move_direction);
            if (std::isnan(wish_dir.x) || std::isnan(wish_dir.y) || std::isnan(wish_dir.z)) wish_dir = glm::vec3(0.0f, 0.0f, 0.0f);
            
            float current_speed = glm::dot(velocity, wish_dir); 
            float add_speed = 0.12f - current_speed;
            add_speed = add_speed < 0.0f ? 0.0f : add_speed;
            add_speed = add_speed > 0.02f ? 0.02f : add_speed;
            
            // add the move direction to the velocity
            auto add_velocity = wish_dir * add_speed;
            
            velocity.x += add_velocity.x;
            velocity.z += add_velocity.z;
            
            
            // check if there is a stair in front of the move direction
            auto poll1_origin = (parent->GetLocation() + (wish_dir * 0.2f)) + vec3(0.0f, 0.3f, 0.0f);
            auto poll2_origin = (parent->GetLocation() + (wish_dir * 0.1f)) + vec3(0.0f, 0.01f, 0.0f);

            triggercomp->SetLocation(poll1_origin);
            auto poll1 = triggercomp->Poll();
            triggercomp->SetLocation(poll2_origin);
            auto poll2 = triggercomp->Poll();
            poll1.insert(poll1.end(), poll2.begin(), poll2.end());
            
            //Render::AddLineMarker(poll1_origin, Render::COLOR_RED);
            //Render::AddLineMarker(poll2_origin, Render::COLOR_RED);
            
            boost = 1.0f;
            bool can_raise = false;
            for (auto& coll : poll1) {
                //Render::AddLineMarker(coll.point, Render::COLOR_BLUE);
                auto rel_point = coll.point - parent->GetLocation() + (1.85f/2.0f);
                if (rel_point.y > 0.005f) can_raise = true;
                if (rel_point.y > 0.4f) {
                    can_raise = false;
                    break;
                }
                if (rel_point.y < boost) boost = rel_point.y;
            }
            
            // if there is a stair, then increase the upward velocity
            //if (can_raise) velocity.y += 0.01f;
            if (!can_raise) boost = 0.0f;
            
            action_updated = true;
        }
        
        if (current_action == ACTION_JUMP){
            triggercomp->SetLocation(parent->GetLocation() - vec3(0.0f, 0.1f, 0.0f));
            if (triggercomp->Poll().size() > 0) {
                velocity.y += 0.089f;
            }
            action_updated = true;
        }

        
        // get the character's orientation
        auto p_loc = parent->GetLocation();
        auto p_rot = parent->GetRotation();
        
        // compute the new position
        auto new_loc = p_loc + velocity;
        
        // check if position is in a wall
        triggercomp->SetLocation(new_loc);
        if (triggercomp->Poll().size() > 0) {
            vec3 average_normal = vec3(0.0f, 0.0f, 0.0f);
            auto poll = triggercomp->Poll();
            for (auto& coll : poll) {
                average_normal += coll.normal;
            }
            average_normal /= poll.size();
            
            auto v_dir = glm::normalize(velocity);
            auto s_dir = glm::normalize(average_normal);

            auto n_dir = glm::normalize(v_dir - s_dir);

            auto n_vel = n_dir * glm::length(velocity) * glm::dot(v_dir, n_dir);

            if (std::isnan(n_vel.x) || std::isnan(n_vel.z)) n_vel = vec3(0.0f, 0.0f, 0.0f);

            // change the velocity, so that the character is no longer in a wall
            velocity = n_vel;
        }
        
        p_loc += velocity;
        
        p_loc.y += boost;
        
        parent->UpdateTransform(p_loc, p_rot);

    }

    bool ControllerComponent::IsInAir(){
        //glm::vec3 f, u, h, c;

        //if(!physcomp) return false;
        //physcomp->GetParent()->GetLocation(f);
        //physcomp->GetParent()->GetLocation(u);
        //physcomp->GetAABB(h, c);
        //u.y = u.y - 0.2f - ((c.y - h.y) / 2.0f);

        //bool inAir = !Physics::Raycast(f, u);
        //return inAir;
        return false;
    }

    void ControllerComponent::ActivateInFront(){
        //std::cout << direction.x << " " << direction.y << " " << direction.z << std::endl;

        glm::vec3 direction;
        glm::vec3 location;
        physcomp->GetParent()->GetLocation(location);

        location += glm::vec3(0.0f, 0.775f, 0.0f); // stinky hack

        PhysicsComponent* activatable = Physics::Raycast(location, location + direction * 3.0f).collider;
        std::cout << activatable << std::endl;
        if (activatable){
            Message msg;

            msg.type = Message::ACTIVATE;
            msg.receiver = activatable->GetParent()->GetID();
            msg.sender = physcomp->GetParent()->GetID();

            Message::Send(msg);
        }
    }
}

