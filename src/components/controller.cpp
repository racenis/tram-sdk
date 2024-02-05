// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/entity.h>
#include <framework/message.h>

#include <components/controller.h>
#include <components/physics.h>

namespace tram {

template <> Pool<ControllerComponent> PoolProxy<ControllerComponent>::pool ("controller component pool", 25, false);

void ControllerComponent::Start() {
    walk_collision.make();
    crouch_collision.make();
    physics_body.make();
    
    walk_collision->SetCollisionMask(-1 ^ Physics::COLL_PLAYER);
    walk_collision->SetShape(Physics::CollisionShape::Cylinder(collision_width, (collision_height/2.0f) - step_height));
    walk_collision->SetStoreCollisions(true);
    
    crouch_collision->SetCollisionMask(-1 ^ Physics::COLL_PLAYER);
    crouch_collision->SetShape(Physics::CollisionShape::Cylinder(collision_width, (collision_height_crouch/2.0f) - step_height_crouch));
    crouch_collision->SetStoreCollisions(true);
    
    physics_body->SetParent(parent);
    physics_body->SetShape(Physics::CollisionShape::Capsule(collision_width, collision_height/2.0f));
    physics_body->SetCollisionGroup(Physics::COLL_PLAYER);
    physics_body->SetKinematic(true);
    physics_body->DisableDeactivation();
    
    walk_collision->Init();
    crouch_collision->Init();
    physics_body->Init();
}

void ControllerComponent::Push(vec3 direction) {
    
}

void ControllerComponent::Move(vec3 local_direction) {
    move_direction += local_direction;
}

void ControllerComponent::Run() {
    running = true;
}

void ControllerComponent::Crouch() {
    crouching = true;
}

void ControllerComponent::Jump() {
    if (!is_in_air) {
        velocity.y += 0.119f;
        is_in_air = true;
    }
}

void ControllerComponent::TurnLeft() {
    // TODO: implement
}

void ControllerComponent::TurnRight() {
    // TODO: implement
}


void ControllerComponent::ApplyDynamics() {
    
    // first we will apply the move direction to the current velocity
    if (glm::length(move_direction) > 0.0f) {
        
        // get the move speed
        float target_speed = walk_speed;
        if (running) target_speed = run_speed;
        if (crouching) target_speed = crouch_speed;

        // convert local space direction into global space direction and normalize it
        glm::vec3 wish_dir = glm::normalize(parent->GetRotation() * move_direction);
        if (std::isnan(wish_dir.x) || std::isnan(wish_dir.y) || std::isnan(wish_dir.z)) wish_dir = glm::vec3(0.0f, 0.0f, 0.0f);
        
        // clip the move direction so that it doesn't exceed maximum velocity
        float current_speed = glm::dot(velocity, wish_dir); 
        float add_speed = target_speed - current_speed;
        add_speed = add_speed < 0.0f ? 0.0f : add_speed;
        add_speed = add_speed > 0.02f ? 0.02f : add_speed;
        
        // add the move direction to the velocity
        auto add_velocity = wish_dir * add_speed;
        
        velocity.x += add_velocity.x;
        velocity.z += add_velocity.z;
    }
    
    // if in air, apply gravity; otherwise apply ground friction
    if (is_in_air) {
        // check for terminal velocity
        if (velocity.y > -0.12f) {
            velocity.y -= 0.0053f;
        }
    } else {
        velocity *= 0.89f;
    }
}

void ControllerComponent::RecoverFromCollisions() {
    const float height = crouching ? collision_height_crouch : collision_height;
    const float step = crouching ? step_height_crouch : step_height;
    const float half_height = height / 2.0f;
    const float width = collision_width;
    
    // compute character's new position
    vec3 old_pos = parent->GetLocation();
    quat old_rot = parent->GetRotation();
    vec3 new_pos = old_pos + velocity;
        
    // check if new position is on the ground
    auto ground_collisions = Physics::Shapecast(
        Physics::CollisionShape::Cylinder(width, half_height),
        new_pos + vec3(0.0f, step, 0.0f),
        new_pos - vec3(0.0f, 0.1f, 0.0f),
        -1 ^ Physics::COLL_PLAYER
    );
    
    if (ground_collisions.size()) {
        vec3 lowest_collision = {INFINITY, INFINITY, INFINITY};
        vec3 lowest_collision_normal;
        
        float character_bottom_height = new_pos.y - half_height;
        
        // find the lowest collision above character's bottom
        for (auto& coll : ground_collisions) {
            if (coll.point.y > character_bottom_height && coll.point.y < lowest_collision.y) {
                lowest_collision = coll.point;
                lowest_collision_normal = coll.normal;
            }
        }
        
        if (lowest_collision.y != INFINITY) {
            //Render::AddLineMarker({new_pos.x, character_bottom_height, new_pos.z}, Render::COLOR_RED);
        }
        
        // if there is such a collision, then put the character at that position
        if (lowest_collision.y != INFINITY) {
            // calcuate step height
            float step_height = lowest_collision.y - character_bottom_height;
            
            // check if stepping up is allowed and then step up
            if (lowest_collision_normal.y > 0.70f && step_height > 0.0f /*&& step_height < 0.35f*/) {
                new_pos.y = lowest_collision.y + half_height + 0.01f;
                velocity.y = 0.0f;
                is_in_air = false;
            } else {
                //std::cout << "failed " << lowest_collision_normal.y << "\t" << step_height << std::endl;
            }
        } else if (!is_in_air) {
            // if character is a certain distance above the ground, we will move the
            // character to the ground. this will help with the floaty feeling when
            // walking down a slope
            float highest_collision = -INFINITY;
            
            for (auto& coll : ground_collisions) {       
                if (coll.point.y > highest_collision) {
                    highest_collision = coll.point.y;
                }
            }
            
            float character_bottom_height = new_pos.y - half_height;
            float step_height = highest_collision - character_bottom_height;
            
            //std::cout << step_height << std::endl;
            if (step_height < 0.0f && step_height > -0.1f) {
                new_pos.y = highest_collision + half_height + 0.01f;
                velocity.y = 0.0f;
                is_in_air = false;;
            }
        }
    } else {
        is_in_air = true;
    }
    
    bool did_v = false;
    
    TriggerComponent* collider = crouching ? crouch_collision.get() : walk_collision.get();
    for (auto& col : collider->GetStoredCollisions()) {
        if (col.distance != 0.0f) {
            new_pos -= col.normal * col.distance;
        }
        
        if (did_v) continue;
        
        // project velocity to collision surface
        auto plane_normal = glm::normalize(-col.normal);
        velocity = velocity - (glm::dot(velocity, plane_normal) * plane_normal);

        if (std::isnan(velocity.x) || std::isnan(velocity.z)) {
            velocity = {0.0f, 0.0f, 0.0f};
        }
        
        // add firienction
        if (is_in_air) {
            velocity.x *= 0.5f;
            velocity.z *= 0.5f;
        }
        
        did_v = true;
        //velocity = vec3(0.0f, 0.0f, 0.0f);
    }
    
    walk_collision->SetLocation(new_pos + vec3(0.0f, 0.35f * 0.5f, 0.0f));
    crouch_collision->SetLocation(new_pos + vec3(0.0f, 0.35f * 0.5f, 0.0f));
    
    // apply new position to character
    parent->UpdateTransform(new_pos, old_rot);
}

void ControllerComponent::ResetMove() {
    move_direction = {0.0f, 0.0f, 0.0f};
    running = false;
    crouching = false;
}


/// Updates the ControllerComponents.
/// Updates all of the ControllerComponents. Should be called once per
/// update cycle.
void ControllerComponent::Update() {
    for (auto& component : PoolProxy<ControllerComponent>::GetPool()) {
        component.ApplyDynamics();
        component.RecoverFromCollisions();
        component.ResetMove();
    }
}

/// Performs the action.
/// Performs the action that is setted with Act() method.
//void ControllerComponent::Perform() {
    //assert(physcomp);
    //assert(parent);

        // compute the move direction
        /*if (current_modifier == ACTIONMODIFIER_FORWARD || current_modifier == ACTIONMODIFIER_FORWARD_LEFT || current_modifier == ACTIONMODIFIER_FORWARD_RIGHT)
            move_direction += DIRECTION_FORWARD;
        if (current_modifier == ACTIONMODIFIER_BACKWARD || current_modifier == ACTIONMODIFIER_BACKWARD_LEFT || current_modifier == ACTIONMODIFIER_BACKWARD_RIGHT)
            move_direction -= DIRECTION_FORWARD;
        if (current_modifier == ACTIONMODIFIER_LEFT || current_modifier == ACTIONMODIFIER_FORWARD_LEFT || current_modifier == ACTIONMODIFIER_BACKWARD_LEFT)
            move_direction -= DIRECTION_SIDE;
        if (current_modifier == ACTIONMODIFIER_RIGHT || current_modifier == ACTIONMODIFIER_FORWARD_RIGHT || current_modifier == ACTIONMODIFIER_BACKWARD_RIGHT)
            move_direction += DIRECTION_SIDE;*/
//}

}

