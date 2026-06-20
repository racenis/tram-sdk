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

template <> Pool<FPSControllerComponent> PoolProxy<FPSControllerComponent>::pool("FPSControllerComponent pool", COMPONENT_LIMIT_CONTROLLER);
template <> Pool<RaycastControllerComponent> PoolProxy<RaycastControllerComponent>::pool("RaycastControllerComponent pool", COMPONENT_LIMIT_CONTROLLER);
template <> void Component<FPSControllerComponent>::init() { ptr = FPSControllerComponent::Make(); }
template <> void Component<FPSControllerComponent>::yeet() { FPSControllerComponent::Yeet(ptr); }
template <> void Component<RaycastControllerComponent>::init() { ptr = RaycastControllerComponent::Make(); }
template <> void Component<RaycastControllerComponent>::yeet() { RaycastControllerComponent::Yeet(ptr); }

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

    for (int i = 0; i < needed_updates && i < 4; i++) {
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

// +---------------------------------------------------------------------------+
// |                                                                           |
// |                              FPS CONTROLLER                               |
// |                                                                           |
// +---------------------------------------------------------------------------+

void FPSControllerComponent::Start() {
    if (!parent) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Character controller doesn't have its parent set!");
    }
    
    walk_collision.make();
    crouch_collision.make();
    
    walk_collision->SetCollisionMask(-1 ^ collision_group);
    walk_collision->SetCollisionGroup(Physics::COLL_TRIGGER);
    walk_collision->SetShape(Physics::CollisionShape::Cylinder(collision_width, (collision_height/2.0f) - step_height));
    walk_collision->SetStoreCollisions(true);
    
    crouch_collision->SetCollisionMask(-1 ^ collision_group);
    crouch_collision->SetCollisionGroup(Physics::COLL_TRIGGER);
    crouch_collision->SetShape(Physics::CollisionShape::Cylinder(collision_width, (collision_height_crouch/2.0f) - step_height));
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
    squished = true;
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
        if (squished) target_speed = crouch_speed;
        if (flying) target_speed = fly_speed;
        
        // convert m/s into m/f
        target_speed *= 1.0f / 60.0f;
        
        // convert local space direction into global space direction and normalize it
        vec3 wish_dir = glm::normalize(parent->GetRotation() * move_direction);
        if (std::isnan(wish_dir.x) || std::isnan(wish_dir.y) || std::isnan(wish_dir.z)) wish_dir = vec3(0.0f, 0.0f, 0.0f);
        
        float current_speed = glm::dot(velocity, wish_dir); 
        const float acc_cap = target_speed * (1.0f - current_speed / target_speed);
        const float acc_inc = std::pow(0.01f, 1.0f / (acceleration * 60.0f));
        float add_speed = (target_speed * (1.0f - acc_inc) - current_speed * (friction - acc_inc)) / friction;
        add_speed = add_speed < 0.0f ? 0.0f : add_speed;
        add_speed = add_speed > acc_cap ? acc_cap : add_speed;

        // add the move direction to the velocity
        auto add_velocity = wish_dir * add_speed;
        
        // keep entity from flying off
        if (is_in_air) add_velocity *= 0.1f;
        
        velocity.x += add_velocity.x;
        velocity.z += add_velocity.z;
        
        if (flying || !collide) {
            velocity.y += add_velocity.y;
        }
    }
}

void FPSControllerComponent::RecoverFromCollisions() {
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
    
    if (!crouching && squished) {
        vec3 mid = parent->GetLocation();
        mid.y -= collision_height_crouch / 2.0f;
        mid.y += collision_height / 2.0f;
        
        auto ceiling_collision = Physics::Raycast(mid - vec3(0, collision_height / 2.0f - 0.1f, 0),
                                                  mid + vec3(0, collision_height / 2.0f + 0.1f, 0),
                                                 -1 ^ (collision_group | Physics::COLL_TRIGGER));
        
        auto ceiling_collisions = Physics::Shapecast(
            Physics::CollisionShape::Cylinder(collision_width, collision_height / 2.0f),
            mid + vec3(0.0f, 0.2f, 0.0f),
            mid + vec3(0.0f, 0.1f, 0.0f),
            -1 ^ collision_group
        );
        
        if (!ceiling_collisions.size() && !ceiling_collision.collider) squished = false;
    }
    
    const float height = squished ? collision_height_crouch : collision_height;
    const float half_height = height / 2.0f;
    const float width = collision_width;
    
    // this should prevent the push from shoving peeps through the
    // ceiling when walking up a slope
    auto ceiling_collision = Physics::Raycast(new_pos,
                                              new_pos + vec3(0, half_height, 0),
                                             -1 ^ (collision_group | Physics::COLL_TRIGGER));
    auto floor_collision = Physics::Raycast(new_pos,
                                              new_pos - vec3(0, half_height + 0.1f, 0),
                                             -1 ^ (collision_group | Physics::COLL_TRIGGER));
    if (ceiling_collision.collider && floor_collision.collider && floor_collision.normal.y < 0.98f) {
        vec3 slope = normalize(DIRECTION_UP - glm::dot(-DIRECTION_UP, floor_collision.normal) * floor_collision.normal);
        velocity = {0.0f, 0.0f, 0.0f};
        velocity += slope * 0.15f;
    }
    
    if (floor_collision.collider && floor_collision.collider->GetParent()) {
        standing_on = floor_collision.collider->GetParent()->GetID();
    } else {
        standing_on = 0;
    }
    
    // check if new position is on the ground
    auto ground_collisions = Physics::Shapecast(
        Physics::CollisionShape::Cylinder(width * 0.9f, half_height),
        new_pos + vec3(0.0f, step_height, 0.0f),
        new_pos - vec3(0.0f, step_height, 0.0f),
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
            }
        }
        
        // if there is such a collision, then put the character at that position
        if (lowest_collision.y != INFINITY) {
            
            // calcuate step height
            float step_height = lowest_collision.y - character_bottom_height;
            
            // check if stepping up is allowed and then step up
            if (lowest_collision_normal.y > 0.70f && step_height > 0.0f) {
                vec3 step_up = new_pos;
                step_up.y = lowest_collision.y + half_height + 0.01f;
            
                new_pos = step_up;
                velocity.y = 0.0f;
                is_in_air = false;
            }
            
            if (lowest_collision_normal.y < 0.70f && lowest_collision_normal.y > 0.02f) {
                is_in_air = true;
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
            float step = highest_collision - character_bottom_height;
            
            if (step < 0.0f && step > -step_height) {
                new_pos.y = highest_collision + half_height + 0.01f;
                velocity.y = 0.0f;
                is_in_air = false;
            }
        }
    } else {
        is_in_air = true;
    }
    
    TriggerComponent* collider = squished ? crouch_collision.get() : walk_collision.get();
    const float squash = collider->GetStoredCollisions().size() > 4 ? 0.25f : 0.5f;
    for (auto& col : collider->GetStoredCollisions()) {
        if (col.distance > 0.0f) {
            continue;
        }
        
        new_pos -= col.normal * col.distance * squash;
        
        // project velocity to collision surface
        auto plane_normal = glm::normalize(-col.normal);
        velocity = velocity - (glm::dot(velocity, plane_normal) * plane_normal);

        if (std::isnan(velocity.x) || std::isnan(velocity.z)) {
            velocity = {0.0f, 0.0f, 0.0f};
        }
        
        // if we don't do this, it looks like you get stuck on the ceiling
        if (col.normal.y < -0.98f) {
            velocity.y = 0.0f;
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
    }
        
    // make controller follow whatever entity it is standing on
    if (standing_on == standing_on_prev && standing_on) {
        vec3 standing_new_pos = Entity::Find(standing_on)->GetLocation();
        quat standing_new_rot = Entity::Find(standing_on)->GetRotation();
        
        vec3 old_delta = standing_pos - parent->GetLocation();
        
        vec3 new_delta = glm::inverse(standing_rot) * old_delta;
        new_delta = standing_new_rot * new_delta;
        
        new_delta = standing_new_pos - new_delta;
        
        new_pos += new_delta - parent->GetLocation();

        standing_pos = standing_new_pos;
        standing_rot = standing_new_rot;
        
    } else if (standing_on) {
        Entity* entity = Entity::Find(standing_on);
        
        if (!entity) {
            standing_on = 0;
        } else {
            standing_pos = entity->GetLocation();
            standing_rot = entity->GetRotation();
        }
    }
    standing_on_prev = standing_on;
    
    walk_collision->SetLocation(new_pos + vec3(0.0f, step_height * 0.5f, 0.0f));
    crouch_collision->SetLocation(new_pos + vec3(0.0f, step_height * 0.5f, 0.0f));
    
    // apply new position to character
    parent->UpdateTransform(new_pos, old_rot);
}

void FPSControllerComponent::ResetMove() {
    move_direction = {0.0f, 0.0f, 0.0f};
    running = false;
    crouching = false;
    flying = false;
}

/// Creates a new FPSControllerComponent.
FPSControllerComponent* FPSControllerComponent::Make() {
    FPSControllerComponent* ptr = PoolProxy<FPSControllerComponent>::GetPool().allocate();
    new(ptr) FPSControllerComponent();
    return ptr;
}

/// Deletes an FPSControllerComponent.
void FPSControllerComponent::Yeet(FPSControllerComponent* component) {
    component->~FPSControllerComponent();
    PoolProxy<FPSControllerComponent>::GetPool().deallocate(component);
}

// +---------------------------------------------------------------------------+
// |                                                                           |
// |                            RAYCAST CONTROLLER                             |
// |                                                                           |
// +---------------------------------------------------------------------------+

void RaycastControllerComponent::Start() {
    if (!parent) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Character controller doesn't have its parent set!");
    }
    
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
        move_magnitude.y += 0.119f;
        
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
    if (ground_collision.collider && air_velocity.y <= 0.0f && !slipping) {
        float actual_ground = glm::distance(parent->GetLocation(), ground_collision.point);
        float step = distance_to_ground - actual_ground;
        if (step < step_height) new_pos.y += step;
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


/// Creates a new RaycastControllerComponent.
RaycastControllerComponent* RaycastControllerComponent::Make() {
    RaycastControllerComponent* ptr = PoolProxy<RaycastControllerComponent>::GetPool().allocate();
    new(ptr) RaycastControllerComponent();
    return ptr;
}

/// Deletes an RaycastControllerComponent.
void RaycastControllerComponent::Yeet(RaycastControllerComponent* component) {
    component->~RaycastControllerComponent();
    PoolProxy<RaycastControllerComponent>::GetPool().deallocate(component);
}

}