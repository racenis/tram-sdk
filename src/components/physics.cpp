// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <components/physics.h>

#include <physics/api.h>
#include <framework/entity.h>

#include <config.h>

/**
 * @class tram::PhysicsComponent components/physics.h <components/physics.h>
 * 
 * Rigidbody wrapper.
 * @see https://racenis.github.io/tram-sdk/documentation/components/physics.html
 */

using namespace tram::Physics;

namespace tram {

template <> Pool<PhysicsComponent> PoolProxy<PhysicsComponent>::pool("PhysicsComponent pool", COMPONENT_LIMIT_PHYSICS);
template <> void Component<PhysicsComponent>::init() { ptr = PhysicsComponent::Make(); }
template <> void Component<PhysicsComponent>::yeet() { PhysicsComponent::Yeet(ptr); }

void PhysicsComponent::Start() {
    if (!shape.bt_shape && (!model || !model->GetShape().bt_shape)) {
        Log(Severity::CRITICAL_ERROR, System::RENDER, "Trigger component doesn't have either a shape or a model set!");
    }
    
    API::get_trf_callback get_callback = nullptr;
    API::set_trf_callback set_callback = nullptr;
    
    if (update_parent_transform && parent) {
        get_callback = [](void* data) -> std::pair<vec3, quat> {
            Entity* parent = ((PhysicsComponent*)data)->GetParent();
            return {parent->GetLocation(), parent->GetRotation()};
        };
        set_callback = [](void* data, std::pair<vec3, quat> transform) {
            ((PhysicsComponent*)data)->GetParent()->UpdateTransform(transform.first, transform.second);
        };
    } else {
        get_callback = [](void* data) -> std::pair<vec3, quat> {
            PhysicsComponent* component = (PhysicsComponent*)data;
            return {component->rigidbody_position, component->rigidbody_rotation};
        };
        set_callback = [](void* data, std::pair<vec3, quat> transform) {
            ((PhysicsComponent*)data)->rigidbody_position = transform.first;
            ((PhysicsComponent*)data)->rigidbody_rotation = transform.second;
        };
    }
    
    rigidbody = API::MakeRigidbody(model ? model->GetShape() : shape,
                                   rigidbody_mass,
                                   rigidbody_position,
                                   rigidbody_rotation,
                                   rigidbody_collision_mask,
                                   rigidbody_collision_group,
                                   get_callback,
                                   set_callback,
                                   this);

    API::SetRigidbodyCollisionCallback(rigidbody, nullptr, this);

    if (rigidbody_should_sleep) API::SleepRigidbody(rigidbody);
    if (rigidbody_should_awake) API::DisableRigidbodyDeactivation(rigidbody);
    if (!rigidbody_debug_draw)  API::SetRigidbodyDebugDrawing(rigidbody, rigidbody_debug_draw);
    if (rigidbody_kinematic)    API::SetRigidbodyKinematic(rigidbody, rigidbody_kinematic);
        
    is_ready = true;
}

PhysicsComponent::~PhysicsComponent(){
    if (rigidbody.bt_rigidbody) {
        API::YeetRigidbody(rigidbody);
    }
}

/// Returns the name of the collision model.
Physics::CollisionModel* PhysicsComponent::GetModel () {
    return model.get();
}

/// Sets the collision model.
/// @deprecated Use SetModel(Physics::CollisionModel*) method instead.
void PhysicsComponent::SetModel(name_t model) {
    SetModel(Physics::CollisionModel::Find(model));
}

/// Sets the collision model.
void PhysicsComponent::SetModel(Physics::CollisionModel* model) {
    if (is_ready) {
        Log(Severity::WARNING, System::RENDER, "Initialized PhysicsComponents cannot accept models! Ignoring PhysicsComponent::SetModel() call.");
        return;
    }

    if (shape.bt_shape) {
        Log(Severity::WARNING, System::RENDER, "PhysicsComponent already has a shape set! Ignoring PhysicsComponent::SetModel() call.");
        return;
    }

    this->model = model;
}

/// Returns the collision mask.
/// See SetCollisionMask() for more information.
uint32_t PhysicsComponent::GetCollisionMask() {
    return rigidbody_collision_mask;
}

/// Returns the collision group.
/// See SetCollisionGroup() for more information.
uint32_t PhysicsComponent::GetCollisionGroup() {
    return rigidbody_collision_group;
}

/// Sets the collision mask of the physics object.
/// Collision mask is a bitmask. A physics object will only collide with
/// an another object if their collision group and collision mask bitmasks
/// have at least one bit in common, i.e. they will be bitwise and'ed
/// together.
void PhysicsComponent::SetCollisionMask (uint32_t flags) {
    rigidbody_collision_mask = flags;
    
    if (is_ready) {
        API::SetRigidbodyCollisionMask(rigidbody, rigidbody_collision_mask);
    }
}

/// Sets the collision group of the physics object.
/// Collision group is a bitmask. A physics object will only collide with
/// an another object if their collision group and collision mask bitmasks
/// have at least one bit in common, i.e. they will be bitwise and'ed
/// together.
void PhysicsComponent::SetCollisionGroup (uint32_t flags) {
    rigidbody_collision_group = flags;
    
    if (is_ready) {
        API::SetRigidbodyCollisionGroup(rigidbody, rigidbody_collision_group);
    }
}

/// Sets the collision shape of the physics object.
void PhysicsComponent::SetShape(Physics::CollisionShape shape) {
    if (is_ready) {
        Log(Severity::WARNING, System::RENDER, "Initialized PhysicsComponents cannot accept shapes! Ignoring PhysicsComponent::SetShape() call.");
        return;
    }
    
    if (model) {
        Log(Severity::WARNING, System::RENDER, "PhysicsComponent already has a model set! Ignoring PhysicsComponent::SetShape() call.");
        return;
    }
    
    if (this->shape.bt_shape) {
        API::YeetCollisionShape(this->shape);
    }
    
    this->shape = API::MakeCollisionShape(shape);
}

/// Sets the mass of the physics object.
/// Mass specified in kilograms. If the mass is set to 0, then it's assumed
/// to be infinite, and the object will become static and immovable.
/// @note Has no effect after the PhysicsComponent is loaded.
void PhysicsComponent::SetMass(float mass) {
    rigidbody_mass = mass;
}

/// Pushes the physics object.
/// I have no idea what the units are. Direction of direction is the
/// direction into which the object will be pushed and the length of
/// the vector is the force of the push.
void PhysicsComponent::Push(vec3 direction) {
    if (!is_ready) return;
    
    API::PushRigidbody(rigidbody, direction);
}

void PhysicsComponent::Push(vec3 direction, vec3 local) {
    if (!is_ready) return;
    
    API::PushRigidbody(rigidbody, direction, local);
}

void PhysicsComponent::Spin(vec3 direction) {
    if (!is_ready) return;
    
    API::SpinRigidbody(rigidbody, direction);
}

/// Awakens the object.
void PhysicsComponent::Awaken() {
    if (is_ready) {
        API::AwakenRigidbody(rigidbody);
    }
    
    rigidbody_should_sleep = false;
}

/// Puts the object to sleep.
void PhysicsComponent::Sleep() {
    if (is_ready) {
        API::SleepRigidbody(rigidbody);
    }
    
    rigidbody_should_sleep = true;
}

/// Sets the debug drawing of a physics object.
/// Set to false, if you don't want the physics object to show up when
/// drawing physics debug.
void PhysicsComponent::SetDebugDrawing (bool drawing) {
    rigidbody_debug_draw = drawing;
    
    if (is_ready) {
        API::SetRigidbodyDebugDrawing(rigidbody, drawing);
    }
}

/// Sets the physics object to kinematic.
/// If set to kinematic, then the physics object will poll its parent entity
/// for its position, instead of simulated and pushing its position into its
/// parent entity.
/// @note Use DisableDeactivation() if you don't want to constantly wake the object up.
void PhysicsComponent::SetKinematic (bool kinematic) {
    rigidbody_kinematic = kinematic;

    if (is_ready) {
        API::SetRigidbodyKinematic(rigidbody, kinematic);
    }
}

/// Sets whether the movement of the rigibody should update parent Entity's transform.
/// If set to true, then whenever the rigidbody moves, the transform of the parent
/// will be set to the transform of the rigidbody. Otherwise, the parent's transform
/// will remain unaffected.
/// @note This only works if set before the component is initialized.
void PhysicsComponent::SetUpdateParentTransform(bool update) {
    update_parent_transform = update;
}

/// Sets the position of the physics object.
void PhysicsComponent::SetLocation(vec3 position) {
    if (rigidbody.bt_rigidbody) {
        API::SetRigidbodyLocation(rigidbody, position);
    }
    
    rigidbody_position = position;
}

/// Sets the rotation of the physics object.
void PhysicsComponent::SetRotation (quat rotation) {
    if (rigidbody.bt_rigidbody) {
        API::SetRigidbodyRotation(rigidbody, rotation);
    }
    
    rigidbody_rotation = rotation;
}

/// Sets the angular factor of the physics object.
/// Changes how much the object will rotate around the x, y, z axes. Can be
/// used to restrict rotation around certain axes.
void PhysicsComponent::SetAngularFactor(vec3 factor) {
    rigidbody_angular_factor = factor;
    
    if (is_ready) {
        API::SetRigidbodyAngularFactor(rigidbody, factor);
    }
}

/// Sets the angular factor of the physics object.
/// Changes how much the object will move along the x, y, z axes. Can be
/// used to restrict movement along certain axes.
void PhysicsComponent::SetLinearFactor(vec3 factor) {
    rigidbody_linear_factor = factor;
    
    if (is_ready) {
        API::SetRigidbodyLinearFactor(rigidbody, factor);
    }
}

/// Disables deactivation of the object.
/// This makes the object to never fall asleep.
void PhysicsComponent::DisableDeactivation() {
    if (is_ready) {
        API::DisableRigidbodyDeactivation(rigidbody);
    }
    
    rigidbody_should_awake = true;
}

/// Sets the velocity of the physics object.
/// Only works if is component is loaded.
void PhysicsComponent::SetVelocity (const vec3& velocity){
    if (!is_ready) return;
    
    API::SetRigidbodyVelocity(rigidbody, velocity);
}

/// Returns the velocity of the physics object.
/// Always returns zero velocity if component is not loaded.
vec3 PhysicsComponent::GetVelocity () {
    if (is_ready) {
        return API::GetRigidbodyVelocity(rigidbody);
    } else {
        return {0.0f, 0.0f, 0.0f};
    }
}

/// Creates a new PhysicsComponent.
PhysicsComponent* PhysicsComponent::Make() {
    PhysicsComponent* ptr = PoolProxy<PhysicsComponent>::GetPool().allocate();
    new(ptr) PhysicsComponent();
    return ptr;
}

/// Deletes an PhysicsComponent.
void PhysicsComponent::Yeet(PhysicsComponent* component) {
    component->~PhysicsComponent();
    PoolProxy<PhysicsComponent>::GetPool().deallocate(component);
}

}