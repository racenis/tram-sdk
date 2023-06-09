// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/entity.h>
#include <framework/message.h>

#include <components/controllercomponent.h>
#include <components/physicscomponent.h>

namespace tram {

template <> Pool<ControllerComponent> PoolProxy<ControllerComponent>::pool ("controller component pool", 25, false);

/// Sets the controllers action.
/// When updated, the controller will start performing the action
/// that it is set to.
void ControllerComponent::Act (Action action, ActionModifier modifier, uint32_t magnitude) {
    current_action = action;
    current_modifier = modifier;
    current_magnitude = magnitude;
    action_updated = true;
}

/// Updates the ControllerComponents.
/// Updates all of the ControllerComponents. Should be called once per
/// update cycle.
void ControllerComponent::Update() {
    for (auto& component : PoolProxy<ControllerComponent>::GetPool()) component.Perform();
}

// checks if a normal is a wall normal
bool IsWallNormal(vec3 normal) {
    float flooriness = glm::dot(normal, DIRECTION_UP);
    
    return flooriness > 0.4f;
}

bool PushOutOfWall(vec3& position, float width, TriggerComponent* triggercomp) {
    triggercomp->SetLocation(position + vec3(0.0f, 0.35f, 0.0f));
    auto wall_collisions = triggercomp->Poll();

    if (!wall_collisions.size()) {
        return false;
    }
        
    float nearest_wall = INFINITY;
    vec3 nearest_point = {0.0f, 0.0f, 0.0f};
    
    for (auto& coll : wall_collisions) {
        float dist = glm::distance(vec2 {position.x, position.z}, vec2 {coll.point.x, coll.point.z});
        
        //Render::AddLineMarker(coll.point, Render::COLOR_YELLOW);
        
        if (dist < nearest_wall) {
            nearest_wall = dist;
            nearest_point = coll.point;
        }
    }
    
    float penetration = width - nearest_wall;
    vec3 push_dir = glm::normalize((position - nearest_point) * vec3(1.0f, 0.0f, 1.0f));
    
    Render::AddLineMarker(nearest_point, Render::COLOR_YELLOW);
    
    Render::AddLine(position, position + penetration * push_dir, Render::COLOR_RED);
    
    std::cout << penetration << std::endl;
    
    position += push_dir * penetration;
    
    return true;
}

/// Performs the action.
/// Performs the action that is setted with Act() method.
void ControllerComponent::Perform() {
    assert(physcomp);
    assert(triggercomp);
    assert(parent);
    
    const float half_height = 1.85f / 2.0f;
    const float width = 0.35f;
    
    (void) width;
    
    if (current_action == ACTION_WALK) {
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
        
        action_updated = true;
    }
    
    if (current_action == ACTION_JUMP) {
        triggercomp->SetLocation(parent->GetLocation() - vec3(0.0f, 0.1f, 0.0f));
        if (triggercomp->Poll().size() > 0) {
            velocity.y += 0.089f;
        }
        action_updated = true;
    }

    //velocity.y -= 0.0037f;

    // add friction
    velocity *= 0.89f;

    // compute character's new position
    vec3 old_pos = parent->GetLocation();
    quat old_rot = parent->GetRotation();
    vec3 new_pos = old_pos + velocity;
    
    // check if new position is in air
    if (!Physics::Raycast(new_pos, new_pos - vec3(0.0f, half_height + 0.1f, 0.0f)).collider) {
        velocity.y -= 0.0137f;
        
        Render::AddLineMarker(new_pos + vec3(0.0f, 1.5f, 0.0f), Render::COLOR_RED);
    } else {
        Render::AddLineMarker(new_pos + vec3(0.0f, 1.5f, 0.0f), Render::COLOR_GREEN);
    }
    

    for (int i = 0; i < 3 && PushOutOfWall(new_pos, width, triggercomp); i++);
    
    

    triggercomp->SetLocation(new_pos);
    auto ground_collisions = triggercomp->Poll();
    
    if (ground_collisions.size()) {
        float highest_collision_height = INFINITY;
        size_t highest_collision = 0;
        
        for (size_t i = 0; i < ground_collisions.size(); i++) {       
            float collision_height = ground_collisions[i].point.y;
            
            if (collision_height < highest_collision_height) {
                highest_collision_height = collision_height;
                highest_collision = i;
            }
            
            (void) highest_collision;
            
            //Render::AddLineMarker({new_pos.x, highest_collision_height, new_pos.z}, Render::COLOR_CYAN);
        }
        
        float character_bottom_height = new_pos.y - half_height;
        float step_height = highest_collision_height - character_bottom_height;
        
        if (step_height > 0.0f && step_height < 0.3f) {
            //Render::AddLineMarker({new_pos.x, highest_collision_height, new_pos.z}, Render::COLOR_CYAN);
            Render::AddLine(ground_collisions[highest_collision].point, ground_collisions[highest_collision].point + ground_collisions[highest_collision].normal, Render::COLOR_WHITE);
            
            new_pos.y = highest_collision_height + half_height + 0.01f;
            
            velocity.y = 0.0f;
        }
    }
    
    Render::AddLine(new_pos, new_pos +10.0f * velocity, Render::COLOR_CYAN);
    Render::AddLineMarker(new_pos, Render::COLOR_PINK);
    
    // apply new position to character
    parent->UpdateTransform(new_pos, old_rot);
}

}

