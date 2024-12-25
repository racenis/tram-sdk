// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <components/trigger.h>
#include <physics/api.h>

#include <config.h>

/**
 * @class TriggerComponent
 * 
 * Trigger wrapper.
 * @see https://racenis.github.io/tram-sdk/documentation/components/trigger.html
 */

using namespace tram::Physics;

namespace tram {

template <> Pool<TriggerComponent> PoolProxy<TriggerComponent>::pool("TriggerComponent pool", COMPONENT_LIMIT_TRIGGER, false);

void TriggerComponent::Start() {
    if (!shape.bt_shape && model) {
        shape = model->GetShape();
    }
    
    assert(shape.bt_shape);

    trigger = API::MakeTrigger(shape, collisionMask, collisionGroup, location, rotation);
    
    API::SetTriggerCollisionCallback(trigger, [](void* obj_a, void* obj_b, API::ObjectCollision collision) {
        TriggerComponent* trigger_component = (TriggerComponent*)obj_a;
        PhysicsComponent* physics_component = (PhysicsComponent*)obj_b;
        
        trigger_component->Collision({physics_component,
                                      collision.point,
                                      collision.normal, 
                                      collision.distance});
    }, this);
    
    is_ready = true;
}

TriggerComponent::~TriggerComponent(){
    API::YeetTrigger(trigger);
    
    if (shape.bt_shape && !model.get()) API::YeetCollisionShape(shape);
};

/// Sets a collision shape for the trigger.
void TriggerComponent::SetShape(Physics::CollisionShape shape) {
    this->shape = API::MakeCollisionShape(shape);
}

/// Sets a collision mask for the trigger.
/// This method works the same as in PhysicsComponent, see
/// PhysicsComponent::SetCollsionMask() for more information.
void TriggerComponent::SetCollisionMask(uint32_t flags){
    collisionMask = flags;
    
    if (is_ready) {
        API::SetTriggerCollisionMask(trigger, flags);
    }
}

/// Sets a collision group for the trigger.
/// This method works the same as in PhysicsComponent, see
/// PhysicsComponent::SetCollisionGroup() for more information.
void TriggerComponent::SetCollisionGroup(uint32_t flags){
    collisionGroup = flags;
    
    if (is_ready) {
        API::SetTriggerCollisionGroup(trigger, flags);
    }
}

/// Sets the location of the component.
void TriggerComponent::SetLocation (vec3 location) {
    this->location = location;
    
    if (is_ready) {
        API::SetTriggerLocation(trigger, location);
    }
}

/// Sets the rotation of the component.
void TriggerComponent::SetRotation (quat rotation) {
    this->rotation = rotation;
    
    if (is_ready) {
        API::SetTriggerRotation(trigger, location);
    }
}

/// Registers a collision.
/// This method is called from Phyics::Update().
void TriggerComponent::Collision(const Physics::Collision& collision) {
    if (!filter_callback || filter_callback(this, collision.collider)) {
        
        bool new_collision = true;
        for (auto& active_collision : active_collisions) {
            if (active_collision.second.collider != collision.collider) continue;
            active_collision.first = GetTick();
            new_collision = false;
            break;
        }
        
        if (store_collisions) {
            stored_collisions.push_back(collision);
        }
        
        if (new_collision) {
            active_collisions.push_back({GetTick(), collision});
            
            if (activation_callback) {
                activation_callback(this, collision);
            }
        }
        
    }
}

/// Resets registered collisions.
/// This method is called from Phyics::Update().
void TriggerComponent::ResetCollisions() {
    for (auto it = active_collisions.begin(); it != active_collisions.end();) {
        if (GetTick() - it->first > 1) {
            if (deactivation_callback) {
                deactivation_callback(this, it->second);
            }

            it = active_collisions.erase(it);
        } else {
            it++;
        }
    }
    
    if (store_collisions) {
        stored_collisions.clear();
    }
}

/// Checks for collisions with the trigger.
std::vector<Physics::Collision> TriggerComponent::Poll() {
    std::vector<Physics::Collision> collisions;
    
    // TODO: move this into -> Physics::API?
    //TriggerPollCallback callback (collisions);
    //DYNAMICS_WORLD->contactTest(trigger.bt_collisionshape, callback);
    
    return collisions;
}

}