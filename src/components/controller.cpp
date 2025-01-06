// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/entity.h>
#include <framework/message.h>
#include <framework/settings.h>

#include <components/controller.h>
#include <components/physics.h>
#include <components/trigger.h>

#include <render/render.h>

#include <config.h>

/**
 * @file components/controller.cpp
 * 
 * Implementations for ControllerComponents.
 */

/**
 * @class tram::ControllerComponent components/controller.h <components/controller.h>
 * 
 * Provides an API for character controllers.
 * @see https://racenis.github.io/tram-sdk/documentation/components/controller.html
 */
 
 /**
 * @class tram::FPSControllerComponent components/controller.h <components/controller.h>
 * 
 * Implementation of an FPS-style character controller.
 * 
 * It is relatively slow, due to the use of Shapecasts, which makes it suitable
 * only for player characters. Consider useing a RaycastControllerComponent for
 * other uses.
 * @see ControllerComponent
 */
 
/**
 * @class tram::RaycastControllerComponent components/controller.h <components/controller.h>
 * 
 * Implementation of a simple character controller.
 * @see ControllerComponent
 */

namespace tram {

template <> Pool<FPSControllerComponent> PoolProxy<FPSControllerComponent>::pool("FPSControllerComponent pool", COMPONENT_LIMIT_CONTROLLER, false);
template <> Pool<RaycastControllerComponent> PoolProxy<RaycastControllerComponent>::pool("RaycastControllerComponent pool", COMPONENT_LIMIT_CONTROLLER, false);
template <> void Component<FPSControllerComponent>::init() { ptr = PoolProxy<FPSControllerComponent>::New(); }
template <> void Component<FPSControllerComponent>::yeet() { PoolProxy<FPSControllerComponent>::Delete(ptr); }
template <> void Component<RaycastControllerComponent>::init() { ptr = PoolProxy<RaycastControllerComponent>::New(); }
template <> void Component<RaycastControllerComponent>::yeet() { PoolProxy<RaycastControllerComponent>::Delete(ptr); }

static Settings::Property<bool> draw_debug = {false, "controller-draw", Settings::NONE};

/// Updates the ControllerComponents.
/// Updates all of the ControllerComponents. Should be called once per update cycle.
/// Best place to do so would be before updating any cameras, otherwise they will
/// lag behind the controller for the rest of the frame.
void ControllerComponent::Update() {
    if (GetTick() < 10) return; // helps with not falling through the floor!!!
    
    // we want to ensure that we get 60 updates/second
    static float update_counter = 0.0f;
    update_counter += GetDeltaTime();

    int needed_updates = floorf(update_counter / (1.0f / 60.0f));
    
    update_counter = fmodf(update_counter, 1.0f / 60.0f);

    for (int i = 0; i < needed_updates; i++) {
        for (auto& component : PoolProxy<FPSControllerComponent>::GetPool()) {
            component.ApplyDynamics();
            component.RecoverFromCollisions();
        }
        
        for (auto& component : PoolProxy<RaycastControllerComponent>::GetPool()) {
            component.ApplyDynamics();
            component.RecoverFromCollisions();
        }
    }

    if (needed_updates) {
        for (auto& component : PoolProxy<FPSControllerComponent>::GetPool()) {
            component.ResetMove();
        }
        
        for (auto& component : PoolProxy<RaycastControllerComponent>::GetPool()) {
            component.ResetMove();
        }
    }
    
    if (!draw_debug) return;
    for (auto& component : PoolProxy<FPSControllerComponent>::GetPool()) {
        char str[100];
        sprintf(str, "Velocity: %.2f %.2f %.2f\nIn air: %s\nRunning: %s\nCrouching: %s\nFlying: %s\nStanding: %s",
                component.velocity.x, component.velocity.y, component.velocity.z,
                component.is_in_air ? "yes" : "no",
                component.running ? "yes" : "no",
                component.crouching ? "yes" : "no",
                component.flying ? "yes" : "no",
                component.standing_on ? (const char*)Entity::Find(component.standing_on)->GetName() : "[not standing]");
        Render::AddText(component.GetParent()->GetLocation(), str);
    }
    
    for (auto& component : PoolProxy<RaycastControllerComponent>::GetPool()) {
        char str[100];
        sprintf(str, "Velocity: %.2f %.2f %.2f\nIn air: %s\nRunning: %s\nCrouching: %s\nFlying: %s\nStanding: %s",
                component.air_velocity.x, component.air_velocity.y, component.air_velocity.z,
                component.is_in_air ? "yes" : "no",
                component.running ? "yes" : "no",
                component.crouching ? "yes" : "no",
                component.flying ? "yes" : "no",
                component.standing_on ? (const char*)Entity::Find(component.standing_on)->GetName() : "[not standing]");
        Render::AddText(component.GetParent()->GetLocation(), str);
    }
}

bool ControllerComponent::IsDebugInfoDraw() {
    return draw_debug;
}

void ControllerComponent::SetDebugInfoDraw(bool draw) {
    draw_debug = draw;
}


// +---------------------------------------------------------------------------+
// |                                                                           |
// |                              FPS CONTROLLER                               |
// |                                                                           |
// +---------------------------------------------------------------------------+

void FPSControllerComponent::Start() {
    walk_collision.make();
    crouch_collision.make();
    
    walk_collision->SetCollisionMask(-1 ^ collision_group);
    walk_collision->SetCollisionGroup(Physics::COLL_TRIGGER);
    walk_collision->SetShape(Physics::CollisionShape::Cylinder(collision_width, (collision_height/2.0f) - step_height));
    walk_collision->SetStoreCollisions(true);
    
    crouch_collision->SetCollisionMask(-1 ^ collision_group);
    crouch_collision->SetCollisionGroup(Physics::COLL_TRIGGER);
    crouch_collision->SetShape(Physics::CollisionShape::Cylinder(collision_width, (collision_height_crouch/2.0f) - step_height_crouch));
    crouch_collision->SetStoreCollisions(true);
    
    walk_collision->Init();
    crouch_collision->Init();
}

void FPSControllerComponent::Push(vec3 direction) {
    // TODO: implement
}

void FPSControllerComponent::Move(vec3 local_direction) {
    move_direction += local_direction;
}

void FPSControllerComponent::Run() {
    running = true;
}

void FPSControllerComponent::Crouch() {
    crouching = true;
}

void FPSControllerComponent::Fly() {
    flying = true;
}

void FPSControllerComponent::Jump() {
    if (!is_in_air) {
        velocity.y += 0.119f;
        is_in_air = true;
    }
}

void FPSControllerComponent::TurnLeft() {
    // TODO: implement
}

void FPSControllerComponent::TurnRight() {
    // TODO: implement
}


void FPSControllerComponent::ApplyDynamics() {
    
    // if in air, apply gravity; otherwise apply ground friction
    if (is_in_air && !flying && collide) {
        // check for terminal velocity
        if (velocity.y > -0.12f) {
            velocity.y -= 0.0053f;
        }
    } else {
        velocity *= friction;
    }
    
    // first we will apply the move direction to the current velocity
    if (glm::length(move_direction) > 0.0f) {
        
        // get the move speed
        float target_speed = walk_speed;
        if (running) target_speed = run_speed;
        if (crouching) target_speed = crouch_speed;

        // helps with floaty feeling
        if (is_in_air) target_speed *= 0.33f;

        // convert local space direction into global space direction and normalize it
        vec3 wish_dir = glm::normalize(parent->GetRotation() * move_direction);
        if (std::isnan(wish_dir.x) || std::isnan(wish_dir.y) || std::isnan(wish_dir.z)) wish_dir = vec3(0.0f, 0.0f, 0.0f);
        
        // clip the move direction so that it doesn't exceed maximum velocity
        float current_speed = glm::dot(velocity, wish_dir); 
        float add_speed = target_speed - current_speed;
        add_speed = add_speed < 0.0f ? 0.0f : add_speed;
        add_speed = add_speed > 0.02f ? 0.02f : add_speed;
        
        // add the move direction to the velocity
        auto add_velocity = wish_dir * add_speed;
        
        velocity.x += add_velocity.x;
        velocity.z += add_velocity.z;
        
        if (flying || !collide) {
            velocity.y += add_velocity.y;
        }
    }
}

void FPSControllerComponent::RecoverFromCollisions() {
    const float height = crouching ? collision_height_crouch : collision_height;
    const float step = crouching ? step_height_crouch : step_height;
    const float half_height = height / 2.0f;
    const float width = collision_width;
    
    // if not colliding, just apply the velocity.
    if (!collide) {
        vec3 new_pos = parent->GetLocation() + velocity;
        parent->UpdateTransform(new_pos, parent->GetRotation());
        return;
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
        -1 ^ collision_group
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
                
                if (coll.collider && coll.collider->GetParent()) {
                    standing_on = coll.collider->GetParent()->GetID();
                }
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
            
            if (step_height < 0.0f && step_height > -0.1f) {
                new_pos.y = highest_collision + half_height + 0.01f;
                velocity.y = 0.0f;
                is_in_air = false;;
            }
        }
    } else {
        standing_on = 0;
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
        
        // add wall slide friction
        if (is_in_air) {
            velocity.x *= 0.5f;
            velocity.z *= 0.5f;
        }
        
        // call the wallbonk callback
        if (wallbonk_callback) {
            wallbonk_callback(this, col);
        }
        
        did_v = true;
    }
        
    // make controller follow whatever entity it is standing on
    if (standing_on == standing_on_prev && standing_on) {
        vec3 standing_new_pos = Entity::Find(standing_on)->GetLocation();
        quat standing_new_rot = Entity::Find(standing_on)->GetRotation();
        
        vec3 old_delta = standing_pos - parent->GetLocation();
        
        vec3 new_delta = glm::inverse(standing_rot) * old_delta;
        new_delta = standing_new_rot * new_delta;
        
        new_delta = standing_new_pos - new_delta;
        
        
        
        //new_delta += standing_pos - standing_new_pos;
        
        new_pos += new_delta - parent->GetLocation();
        //new_pos += new_delta;
        
        standing_pos = standing_new_pos;
        standing_rot = standing_new_rot;
        
        /*vec3 standing_new_pos = Entity::Find(standing_on)->GetLocation();
        if (standing_pos != standing_new_pos) {
            vec3 delta = standing_new_pos - standing_pos; 
            new_pos += delta;
        }
        standing_pos = standing_new_pos;*/
    } else if (standing_on) {
        standing_pos = Entity::Find(standing_on)->GetLocation();
        standing_rot = Entity::Find(standing_on)->GetRotation();
    }
    standing_on_prev = standing_on;
    
    walk_collision->SetLocation(new_pos + vec3(0.0f, 0.35f * 0.5f, 0.0f));
    crouch_collision->SetLocation(new_pos + vec3(0.0f, 0.35f * 0.5f, 0.0f));
    
    // apply new position to character
    parent->UpdateTransform(new_pos, old_rot);
}

void FPSControllerComponent::ResetMove() {
    move_direction = {0.0f, 0.0f, 0.0f};
    running = false;
    crouching = false;
    flying = false;
}



// +---------------------------------------------------------------------------+
// |                                                                           |
// |                            RAYCAST CONTROLLER                             |
// |                                                                           |
// +---------------------------------------------------------------------------+

void RaycastControllerComponent::Start() {
    wall_collision.make();
    
    wall_collision->SetCollisionMask(-1 ^ collision_group);
    wall_collision->SetCollisionGroup(Physics::COLL_TRIGGER);
    wall_collision->SetShape(Physics::CollisionShape::Cylinder(collision_width, (collision_height/2.0f) - step_height ));//* 2.0f));
    wall_collision->SetStoreCollisions(true);
    
    wall_collision->Init();
}

void RaycastControllerComponent::Push(vec3 direction) {
    // TODO: implement
}

void RaycastControllerComponent::Move(vec3 local_direction) {
    move_direction += local_direction;
}

void RaycastControllerComponent::Run() {
    running = true;
}

void RaycastControllerComponent::Crouch() {
    crouching = true;
}

void RaycastControllerComponent::Fly() {
    flying = true;
}

void RaycastControllerComponent::Jump() {
    if (!is_in_air) {
        //velocity.y += 0.119f;
        move_magnitude.y += 0.119f;
        //air_velocity.y += 1.119f;
        
        jump_frames = 60;
        
        is_in_air = true;
    }
    
    enabled = true;
}

void RaycastControllerComponent::TurnLeft() {
    // TODO: implement
}

void RaycastControllerComponent::TurnRight() {
    // TODO: implement
}


void RaycastControllerComponent::ApplyDynamics() {
     if (!enabled) return;
    
    // apply jumping
    if (jump_frames > 0) {
        air_velocity.y += 0.0093f;
        
        jump_frames--;
    }
    
    // apply gravity
    if (is_in_air && air_velocity.y > -1.0f) {
        air_velocity.y -= 0.0053f;
    }

    // first we will apply the move direction to the current velocity
    if (glm::length(move_direction) > 0.0f) {
        
        // get the move speed
        float target_speed = walk_speed;
        if (running) target_speed = run_speed;
        if (crouching) target_speed = crouch_speed;

        // helps with floaty feeling
        if (is_in_air) target_speed *= 0.33f;
        
        // makes the speed more consistent with FPSController speeds
        target_speed *= 0.5f;

        // convert local space direction into global space direction and normalize it
        move_magnitude = glm::normalize(parent->GetRotation() * move_direction);
        if (std::isnan(move_magnitude.x) || std::isnan(move_magnitude.y) || std::isnan(move_magnitude.z)) move_magnitude = vec3(0.0f, 0.0f, 0.0f);
        
        move_magnitude *= target_speed;
    } else {
        move_magnitude = {0.0f, 0.0f, 0.0f};
    }
}

void RaycastControllerComponent::RecoverFromCollisions() {
     if (!enabled) return;
    
    // if not colliding, just apply the movement.
    if (!collide) {
        vec3 new_pos = parent->GetLocation() + move_magnitude;
        parent->UpdateTransform(new_pos, parent->GetRotation());
        return;
    }
    
    // compute character's new position
    vec3 old_pos = parent->GetLocation();
    quat old_rot = parent->GetRotation();
    vec3 new_pos = old_pos + move_magnitude + air_velocity;
    
    // find where the ground is
    float distance_to_ground = collision_height / 2.0f;
    float raycast_distance = distance_to_ground + 0.1f;
    
    auto ground_collision = Physics::Raycast(new_pos,
                                             new_pos - vec3(0, raycast_distance, 0),
                                             -1 ^ (collision_group | Physics::COLL_TRIGGER));
                                             
    bool slipping = false;
    // update being in air status
    if (ground_collision.collider && air_velocity.y <= 0.0f) {
        is_in_air = false;
        air_velocity = {0.0f, 0.0f, 0.0f};
        
        slipping = 0.95f > glm::dot(DIRECTION_UP, ground_collision.normal);
    } else {
        is_in_air = true;
    }

    if (slipping) {
        vec3 downhill = glm::cross(glm::cross(ground_collision.normal, -DIRECTION_UP), ground_collision.normal);
        new_pos += 0.1f * downhill;
    }

    // snap controller to ground
    // TODO: make this respect step_height property
    if (ground_collision.collider && air_velocity.y <= 0.0f && !slipping) {
        float actual_ground = glm::distance(parent->GetLocation(), ground_collision.point);
        new_pos.y += distance_to_ground - actual_ground;
    }
    
    
    bool fired_callback = false;
    for (auto& col : wall_collision->GetStoredCollisions()) {
        if (col.distance != 0.0f) {
            new_pos -= col.normal * col.distance;
        }
        
        if (fired_callback) continue;
        
        // call the wallbonk callback
        if (wallbonk_callback) {
            wallbonk_callback(this, col);
        }
        
        fired_callback = true;
    }
    
    wall_collision->SetLocation(new_pos + vec3(0.0f, 0.35f * 0.5f, 0.0f));

    // apply new position to entity
    parent->UpdateTransform(new_pos, old_rot);
}

void RaycastControllerComponent::ResetMove() {
    move_direction = {0.0f, 0.0f, 0.0f};
    running = false;
    crouching = false;
    flying = false;
}

}

