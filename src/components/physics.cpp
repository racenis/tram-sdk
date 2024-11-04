// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <components/physics.h>

#include <physics/api.h>
#include <framework/entity.h>

using namespace tram::Physics;

namespace tram {

template <> Pool<PhysicsComponent> PoolProxy<PhysicsComponent>::pool("physics component pool", 500, false);

void PhysicsComponent::Start() {
    if (collision_model.get()) {
        collision_shape = collision_model->GetShape();
    }
    
    API::get_trf_callback get_callback = nullptr;
    API::set_trf_callback set_callback = nullptr;
    
    if (update_parent_transform && parent) {
        get_callback = [](void* data) -> std::pair<vec3, quat> {
            Entity* parent = ((PhysicsComponent*)data)->GetParent();
            //std::cout << "getting parent: " << ((PhysicsComponent*)data)->GetParent()->GetName() << std::endl;
            return {parent->GetLocation(), parent->GetRotation()};
        };
        set_callback = [](void* data, std::pair<vec3, quat> transform) {
            ((PhysicsComponent*)data)->GetParent()->UpdateTransform(transform.first, transform.second);
            //std::cout << "setting parent: " << ((PhysicsComponent*)data)->GetParent()->GetName() << std::endl;
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
    
    rigidbody = API::MakeRigidbody(collision_shape,
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
    //RemoveAllConstraints();
    
    if (rigidbody.bt_rigidbody) {
        //DYNAMICS_WORLD->removeRigidBody(rigidbody.bt_rigidbody);
        API::YeetRigidbody(rigidbody);
        //delete rigidbody.bt_rigidbody;
        //delete motion_state;
    }
}

/// Returns the name of the collision model.
name_t PhysicsComponent::GetModel () {
    return collision_model ? collision_model->GetName() : UID();
}

/// Sets the collision model.
void PhysicsComponent::SetModel (name_t model) {
    collision_model = Physics::CollisionModel::Find(model);
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
        //DYNAMICS_WORLD->removeRigidBody(rigidbody.bt_rigidbody);
        //DYNAMICS_WORLD->addRigidBody(rigidbody.bt_rigidbody, rigidbody_collision_group, rigidbody_collision_mask);
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
        //DYNAMICS_WORLD->removeRigidBody(rigidbody.bt_rigidbody);
        //DYNAMICS_WORLD->addRigidBody(rigidbody.bt_rigidbody, rigidbody_collision_group, rigidbody_collision_mask);
    }
}

/// Sets the collision shape of the physics object.
void PhysicsComponent::SetShape(Physics::CollisionShape shape) {
    collision_shape = API::MakeCollisionShape(shape);
}

/// Sets the mass of the physics object.
/// Mass specified in kilograms. If the mass is set to 0, then it's assumed
/// to be infinite, and the object will become static and immovable.
/// @note Has no effect after the PhysicsComponent is loaded.
void PhysicsComponent::SetMass(float mass) {
    /*if (mass == 0) {
        rigidbody_collision_flags |= btCollisionObject::CF_STATIC_OBJECT;
    }*/
    
    rigidbody_mass = mass;
}

/// Pushes the physics object.
/// I have no idea what the units are. Direction of direction is the
/// direction into which the object will be pushed and the length of
/// the vector is the force of the push.
void PhysicsComponent::Push(vec3 direction) {
    if (!is_ready) return;
    
    API::PushRigidbody(rigidbody, direction);
    
    //rigidbody.bt_rigidbody->activate(); // force awake, sleeping objects won't move
    //rigidbody.bt_rigidbody->applyCentralImpulse(btVector3(direction.x, direction.y, direction.z));
}

void PhysicsComponent::Push(vec3 direction, vec3 local) {
    if (!is_ready) return;
    
    API::PushRigidbody(rigidbody, direction, local);
    //rigidbody.bt_rigidbody->activate();
    //rigidbody.bt_rigidbody->applyImpulse(btVector3(direction.x, direction.y, direction.z), btVector3(local.x, local.y, local.z));
}

void PhysicsComponent::Spin(vec3 direction) {
    if (!is_ready) return;
    
    API::SpinRigidbody(rigidbody, direction);
    
    //rigidbody.bt_rigidbody->activate();
    //rigidbody.bt_rigidbody->applyTorqueImpulse(btVector3(direction.x, direction.y, direction.z));
}

/// Awakens the object.
void PhysicsComponent::Awaken() {
    if (is_ready) {
        API::AwakenRigidbody(rigidbody);
        //rigidbody.bt_rigidbody->activate();
    }
    
    rigidbody_should_sleep = false;
}

/// Puts the object to sleep.
void PhysicsComponent::Sleep() {
    if (is_ready) {
        //rigidbody.bt_rigidbody->setActivationState(0);
        API::SleepRigidbody(rigidbody);
    }
    
    rigidbody_should_sleep = true;
}

/// Sets the debug drawing of a physics object.
/// Set to false, if you don't want the physics object to show up when
/// drawing physics debug.
void PhysicsComponent::SetDebugDrawing (bool drawing) {
    /*if (drawing) {
        rigidbody_collision_flags &= ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
    } else {
        rigidbody_collision_flags |= btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
    }*/
    
    rigidbody_debug_draw = drawing;
    
    if (is_ready) {
        API::SetRigidbodyDebugDrawing(rigidbody, drawing);
        //rigidbody.bt_rigidbody->setCollisionFlags(rigidbody_collision_flags);
    }
}

/// Sets the physics object to kinematic.
/// If set to kinematic, then the physics object will poll its parent entity
/// for its position, instead of simulated and pushing its position into its
/// parent entity.
/// @note Use DisableDeactivation() if you don't want to constantly wake the object up.
void PhysicsComponent::SetKinematic (bool kinematic) {
    rigidbody_kinematic = kinematic;
    
    /*if (kinematic) {
        rigidbody_collision_flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
    } else {
        rigidbody_collision_flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;
    }*/
    
    if (is_ready) {
        API::SetRigidbodyKinematic(rigidbody, kinematic);
        //rigidbody.bt_rigidbody->setCollisionFlags(rigidbody_collision_flags);
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
        //btTransform trans = rigidbody.bt_rigidbody->getWorldTransform();
        //trans.setOrigin(btVector3 (position.x, position.y, position.z));
        //rigidbody.bt_rigidbody->setWorldTransform(trans);
    }
    
    rigidbody_position = position;
}

/// Sets the rotation of the physics object.
void PhysicsComponent::SetRotation (quat rotation) {
    if (rigidbody.bt_rigidbody) {
        API::SetRigidbodyRotation(rigidbody, rotation);
        //btTransform trans = rigidbody.bt_rigidbody->getWorldTransform();
        //trans.setRotation(btQuaternion (rotation.x, rotation.y, rotation.z, rotation.w));
        //rigidbody.bt_rigidbody->setWorldTransform(trans);
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
        //rigidbody.bt_rigidbody->setAngularFactor({factor.x, factor.y, factor.z});
    }
}

/// Sets the angular factor of the physics object.
/// Changes how much the object will move along the x, y, z axes. Can be
/// used to restrict movement along certain axes.
void PhysicsComponent::SetLinearFactor(vec3 factor) {
    rigidbody_linear_factor = factor;
    
    if (is_ready) {
        API::SetRigidbodyLinearFactor(rigidbody, factor);
        //rigidbody.bt_rigidbody->setLinearFactor({factor.x, factor.y, factor.z});
    }
}

/// Disables deactivation of the object.
/// This makes the object to never fall asleep.
void PhysicsComponent::DisableDeactivation() {
    if (is_ready) {
        API::DisableRigidbodyDeactivation(rigidbody);
        //rigidbody.bt_rigidbody->setActivationState(DISABLE_DEACTIVATION);
    }
    
    rigidbody_should_awake = true;
}

/// Sets the velocity of the physics object.
/// Only works if is component is loaded.
void PhysicsComponent::SetVelocity (const vec3& velocity){
    if (!is_ready) return;
    
    /*if (velocity.x != 0.0f &&
        velocity.y != 0.0f &&
        velocity.z != 0.0f
    ) {
        rigidbody.bt_rigidbody->activate();
    }
    
    rigidbody.bt_rigidbody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));*/
    API::SetRigidbodyVelocity(rigidbody, velocity);
}

/// Returns the velocity of the physics object.
/// Always returns zero velocity if component is not loaded.
vec3 PhysicsComponent::GetVelocity () {
    if (is_ready) {
        return API::GetRigidbodyVelocity(rigidbody);
        //auto velocity = rigidbody.bt_rigidbody->getLinearVelocity();
        //return {velocity.getX(), velocity.getY(), velocity.getZ()};
    } else {
        return {0.0f, 0.0f, 0.0f};
    }
}

}