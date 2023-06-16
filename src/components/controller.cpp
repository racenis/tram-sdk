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
    
    crouch_collision->SetCollisionMask(-1 ^ Physics::COLL_PLAYER);
    crouch_collision->SetShape(Physics::CollisionShape::Cylinder(collision_width, (collision_height_crouch/2.0f) - step_height_crouch));
    
    physics_body->SetParent(parent);
    physics_body->SetShape(Physics::CollisionShape::Capsule(collision_width, collision_height/2.0f));
    physics_body->SetCollisionGroup(Physics::COLL_PLAYER);
    physics_body->SetKinematic(true);
    physics_body->DisableDeactivation();
    
    walk_collision->Init();
    crouch_collision->Init();
    physics_body->Init();
}

/// Sets the controllers action.
/// When updated, the controller will start performing the action
/// that it is set to.
void ControllerComponent::Act(Action action, ActionModifier modifier) {
    current_action = action;
    current_modifier = modifier;
}

/// Updates the ControllerComponents.
/// Updates all of the ControllerComponents. Should be called once per
/// update cycle.
void ControllerComponent::Update() {
    for (auto& component : PoolProxy<ControllerComponent>::GetPool()) component.Perform();
}

// checks if a normal is a wall normal
bool IsWallNormal(vec3 normal) {
    return normal.y < 0.75f;
}

bool PushOutOfWall(vec3& position, float width, TriggerComponent* triggercomp) {
    triggercomp->SetLocation(position + vec3(0.0f, 0.35f * 0.5f, 0.0f));
    auto wall_collisions = triggercomp->Poll();

    if (!wall_collisions.size()) {
        return true;
    }
        
    float nearest_wall = INFINITY;
    vec3 nearest_point = {0.0f, 0.0f, 0.0f};
    
    for (auto& coll : wall_collisions) {
        float dist = glm::distance(vec2 {position.x, position.z}, vec2 {coll.point.x, coll.point.z});
        
        if (dist < nearest_wall) {
            nearest_wall = dist;
            nearest_point = coll.point;
        }
    }
    
    float penetration = width - nearest_wall;
    vec3 push_dir = glm::normalize((position - nearest_point) * vec3(1.0f, 0.0f, 1.0f));
    
    Render::AddLineMarker(nearest_point, Render::COLOR_YELLOW);
    
    position += push_dir * penetration;
    
    return penetration < 0.01f;
}

/// Performs the action.
/// Performs the action that is setted with Act() method.
void ControllerComponent::Perform() {
    //assert(physcomp);
    assert(parent);
    
    const bool crouching = current_action == ACTION_CROUCH || current_action == ACTION_CROUCH_IDLE;
    
    const float height = crouching ? collision_height_crouch : collision_height;
    const float step = crouching ? step_height_crouch : step_height;
    const float half_height = height / 2.0f;
    const float width = collision_width;
    
    if (current_action == ACTION_WALK || current_action == ACTION_RUN || current_action == ACTION_CROUCH) {
        glm::vec3 move_direction = glm::vec3(0.0f, 0.0f, 0.0f);

        float target_speed = 0.0f;
        
        // select speed
        switch (current_action) {
        case ACTION_RUN:
            target_speed = run_speed;
            break;
        case ACTION_CROUCH:
            target_speed = crouch_speed;
            break;
        default:
            target_speed = walk_speed;
        }

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
        float add_speed = target_speed - current_speed;
        add_speed = add_speed < 0.0f ? 0.0f : add_speed;
        add_speed = add_speed > 0.02f ? 0.02f : add_speed;
        
        // add the move direction to the velocity
        auto add_velocity = wish_dir * add_speed;
        
        velocity.x += add_velocity.x;
        velocity.z += add_velocity.z;
    }
    
    if (current_action == ACTION_JUMP && !is_in_air) {
        velocity.y += 0.119f;
        is_in_air = true;
    }

    if (is_in_air) {
        // add gravity
        if (velocity.y > -0.12f) {
            velocity.y -= 0.0053f;
        }
        Render::AddLineMarker(parent->GetLocation() + vec3(0.0f, 1.5f, 0.0f), Render::COLOR_RED);
    } else {
        // add friction
        velocity *= 0.89f;
        Render::AddLineMarker(parent->GetLocation() + vec3(0.0f, 1.5f, 0.0f), Render::COLOR_GREEN);
    }

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
            
            Render::AddLineMarker(coll.point, Render::COLOR_CYAN);
        }
        
        if (lowest_collision.y != INFINITY) {
            Render::AddLineMarker({new_pos.x, character_bottom_height, new_pos.z}, Render::COLOR_RED);
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
    
    TriggerComponent* collider = crouching ? crouch_collision.get() : walk_collision.get();
    bool push_success = false;
    for (int i = 0; i < 3 && !(push_success = PushOutOfWall(new_pos, width, collider)); i++);// std::cout << "push " << i << " ";
    
    if (!push_success) {
        std::cout << "fucky wucky" << std::endl;
        return;
    }
    
    //std::cout << std::endl;
    //std::cout << new_pos.y << std::endl;
    
    // apply new position to character
    parent->UpdateTransform(new_pos, old_rot);
}

}

