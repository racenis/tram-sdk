// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <components/physics.h>
#include <physics/bullet/bullet.h>

#include <physics/bullet/motionstates.h>

using namespace tram::Physics;
using namespace tram::Physics::Bullet;

namespace tram {

template <> Pool<PhysicsComponent> PoolProxy<PhysicsComponent>::pool("physics component pool", 250, false);

void PhysicsComponent::Start(){
    if (collision_model.get()) {
        collision_shape = collision_model->GetShape();
    }

    if (update_parent_transform) {
        motion_state = new EntMotionState(parent, rigidbody_offset);
    } else {
        btTransform initial_transform;
		initial_transform.setIdentity();
		//initial_transform.setOrigin({rigidbody_position.x, rigidbody_position.y, rigidbody_position.z});
        //initial_transform.setRotation({rigidbody_rotation.x, rigidbody_rotation.y, rigidbody_rotation.z, rigidbody_rotation.w});
        if (parent) {
            quat rot = parent->GetRotation();
            vec3 pos = parent->GetLocation();
            initial_transform.setOrigin({pos.x, pos.y, pos.z});
            initial_transform.setRotation({rot.x, rot.y, rot.z, rot.w});
        }
        motion_state = new btDefaultMotionState(initial_transform);
    }
    

    btScalar bullet_mass = rigidbody_mass;
    btVector3 bullet_inertia (0.0f, 0.0f, 0.0f);
    collision_shape->calculateLocalInertia(bullet_mass, bullet_inertia);
    btRigidBody::btRigidBodyConstructionInfo bullet_construction_info (bullet_mass, motion_state, collision_shape, bullet_inertia);

    rigidbody = new btRigidBody(bullet_construction_info);

    DYNAMICS_WORLD->addRigidBody(rigidbody, rigidbody_collision_group, rigidbody_collision_mask);
    
    rigidbody->setUserPointer(this);
    rigidbody->setUserIndex(USERINDEX_PHYSICSCOMPONENT);
    rigidbody->setCollisionFlags(rigidbody_collision_flags);

    if (rigidbody_should_sleep) rigidbody->setActivationState(0);
    if (rigidbody_should_awake) rigidbody->setActivationState(DISABLE_DEACTIVATION);

    is_ready = true;
}

PhysicsComponent::~PhysicsComponent(){
    RemoveAllConstraints();
    
    DYNAMICS_WORLD->removeRigidBody(rigidbody);
    
    delete rigidbody;
    delete motion_state;
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
        DYNAMICS_WORLD->removeRigidBody(rigidbody);
        DYNAMICS_WORLD->addRigidBody(rigidbody, rigidbody_collision_group, rigidbody_collision_mask);
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
        DYNAMICS_WORLD->removeRigidBody(rigidbody);
        DYNAMICS_WORLD->addRigidBody(rigidbody, rigidbody_collision_group, rigidbody_collision_mask);
    }
}

/// Sets the collision shape of the physics object.
void PhysicsComponent::SetShape(Physics::CollisionShape shape) {
    collision_shape = CollisionShapeToConvexShape(shape);
}

/// Sets the mass of the physics object.
/// Mass specified in kilograms. If the mass is set to 0, then it's assumed
/// to be infinite, and the object will become static and immovable.
/// @note Has no effect after the PhysicsComponent is loaded.
void PhysicsComponent::SetMass(float mass) {
    if (mass == 0) {
        rigidbody_collision_flags |= btCollisionObject::CF_STATIC_OBJECT;
    }
    
    rigidbody_mass = mass;
}

/// Pushes the physics object.
/// I have no idea what the units are. Direction of direction is the
/// direction into which the object will be pushed and the length of
/// the vector is the force of the push.
void PhysicsComponent::Push(vec3 direction) {
    if (!is_ready) return;
    
    rigidbody->activate(); // force awake, sleeping objects won't move
    rigidbody->applyCentralImpulse(btVector3(direction.x, direction.y, direction.z));
}

void PhysicsComponent::Push(vec3 direction, vec3 local) {
    if (!is_ready) return;
    
    rigidbody->activate();
    rigidbody->applyImpulse(btVector3(direction.x, direction.y, direction.z), btVector3(local.x, local.y, local.z));
}

void PhysicsComponent::Spin(vec3 direction) {
    if (!is_ready) return;
    
    rigidbody->activate();
    rigidbody->applyTorqueImpulse(btVector3(direction.x, direction.y, direction.z));
}

/// Awakens the object.
void PhysicsComponent::Awaken() {
    if (is_ready) {
        rigidbody->activate();
    }
    
    rigidbody_should_sleep = false;
}

/// Puts the object to sleep.
void PhysicsComponent::Sleep() {
    if (is_ready) {
        rigidbody->setActivationState(0);
    }
    
    rigidbody_should_sleep = true;
}

/// Sets the debug drawing of a physics object.
/// Set to false, if you don't want the physics object to show up when
/// drawing physics debug.
void PhysicsComponent::SetDebugDrawing (bool drawing) {
    if (drawing) {
        rigidbody_collision_flags &= ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
    } else {
        rigidbody_collision_flags |= btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT;
    }
    
    if (is_ready) {
        rigidbody->setCollisionFlags(rigidbody_collision_flags);
    }
}

/// Sets the physics object to kinematic.
/// If set to kinematic, then the physics object will poll its parent entity
/// for its position, instead of simulated and pushing its position into its
/// parent entity.
/// @note Use DisableDeactivation() if you don't want to constantly wake the object up.
void PhysicsComponent::SetKinematic (bool kinematic) {
    if (kinematic) {
        rigidbody_collision_flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
    } else {
        rigidbody_collision_flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;
    }
    
    if (is_ready) {
        rigidbody->setCollisionFlags(rigidbody_collision_flags);
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
    if (rigidbody) {
        btTransform trans = rigidbody->getWorldTransform();
        trans.setOrigin(btVector3 (position.x, position.y, position.z));
        rigidbody->setWorldTransform(trans);
    }
    
    rigidbody_position = position;
}

/// Sets the rotation of the physics object.
void PhysicsComponent::SetRotation (quat rotation) {
    if (rigidbody) {
        btTransform trans = rigidbody->getWorldTransform();
        trans.setRotation(btQuaternion (rotation.x, rotation.y, rotation.z, rotation.w));
        rigidbody->setWorldTransform(trans);
    }
    
    rigidbody_rotation = rotation;
}

/// Sets the angular factor of the physics object.
/// Changes how much the object will rotate around the x, y, z axes. Can be
/// used to restrict rotation around certain axes.
void PhysicsComponent::SetAngularFactor(vec3 factor) {
    rigidbody_angular_factor = factor;
    
    if (is_ready) {
        rigidbody->setAngularFactor({factor.x, factor.y, factor.z});
    }
}

/// Sets the angular factor of the physics object.
/// Changes how much the object will move along the x, y, z axes. Can be
/// used to restrict movement along certain axes.
void PhysicsComponent::SetLinearFactor(vec3 factor) {
    rigidbody_linear_factor = factor;
    
    if (is_ready) {
        rigidbody->setLinearFactor({factor.x, factor.y, factor.z});
    }
}

/// Disables deactivation of the object.
/// This makes the object to never fall asleep.
void PhysicsComponent::DisableDeactivation() {
    if (is_ready) {
        rigidbody->setActivationState(DISABLE_DEACTIVATION);
    }
    
    rigidbody_should_awake = true;
}

/// Sets the velocity of the physics object.
/// Only works if is component is loaded.
void PhysicsComponent::SetVelocity (const vec3& velocity){
    if (!is_ready) return;
    
    if (velocity.x != 0.0f &&
        velocity.y != 0.0f &&
        velocity.z != 0.0f
    ) {
        rigidbody->activate();
    }
    
    rigidbody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
}

/// Returns the velocity of the physics object.
/// Always returns zero velocity if component is not loaded.
vec3 PhysicsComponent::GetVelocity () {
    if (is_ready) {
        auto velocity = rigidbody->getLinearVelocity();
        return {velocity.getX(), velocity.getY(), velocity.getZ()};
    } else {
        return {0.0f, 0.0f, 0.0f};
    }
}

void PhysicsComponent::AddPointConstraint(vec3 offset) {
    btTypedConstraint* constr = new btPoint2PointConstraint(
        *this->rigidbody,
        {offset.x, offset.y, offset.z}
    );

    this->constraints.push_back({
        nullptr,
        constr
    });
    
    DYNAMICS_WORLD->addConstraint(constr);
}

void PhysicsComponent::AddPointConstraint(vec3 offset, PhysicsComponent* other, vec3 other_offset) {
    // TODO: implement
}

/// Constraint removal helper function.
void RemoveConstraintHelper(std::vector<PhysicsConstraint>& constraints, btTypedConstraint* removable) {
    for (auto it = constraints.begin(); it < constraints.end(); it++) {
        if (it->constraint == removable) {
            constraints.erase(it);
            return;
        }
    }
}

void PhysicsComponent::RemoveConstraint(PhysicsComponent* other) {
    bool try_erasing = true;
    
    while (try_erasing) {
        try_erasing = false;
        
        for (auto it = constraints.begin(); it < constraints.end(); it++) {
            if (it->other == other) {
                if (other) {
                    RemoveConstraintHelper(other->constraints, it->constraint);
                }
                
                DYNAMICS_WORLD->removeConstraint(it->constraint);
                
                delete it->constraint;
                
                constraints.erase(it);
                try_erasing = true;
                
                break;
            }
        }
    }
}

void PhysicsComponent::RemoveAllConstraints() {
    while (constraints.size()) {
        auto it = constraints.begin();
        
        if (it->other) {
            RemoveConstraintHelper(it->other->constraints, it->constraint);
        }
        
        delete it->constraint;
        
        DYNAMICS_WORLD->removeConstraint(it->constraint);
        
        constraints.erase(it);
    }
}

}