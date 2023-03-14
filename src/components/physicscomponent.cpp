#include <components/physicscomponent.h>
#include <physics/bullet/bullet.h>

#include <physics/bullet/motionstates.h>

using namespace Core::Physics;

namespace Core {
    template <> Pool<PhysicsComponent> PoolProxy<PhysicsComponent>::pool("physics component pool", 250, false);
    
    void PhysicsComponent::Start(){
        if (collision_model.get()) {
            collision_shape = collision_model->GetShape();
        }

        motion_state = new EntMotionState(parent, rigidbody_offset);

        btScalar bullet_mass = rigidbody_mass;
        btVector3 bullet_inertia (0.0f, 0.0f, 0.0f);
        collision_shape->calculateLocalInertia(bullet_mass, bullet_inertia);
        btRigidBody::btRigidBodyConstructionInfo bullet_construction_info (bullet_mass, motion_state, collision_shape, bullet_inertia);

        rigidbody = new btRigidBody (bullet_construction_info);

        dynamicsWorld->addRigidBody(rigidbody, rigidbody_collision_group, rigidbody_collision_mask);
        
        rigidbody->setUserPointer(this);
        rigidbody->setUserIndex(USERINDEX_PHYSICSCOMPONENT);
        rigidbody->setCollisionFlags(rigidbody_collision_flags);

        if (rigidbody_should_sleep) rigidbody->setActivationState(0);

        is_ready = true;
    }
    
    PhysicsComponent::~PhysicsComponent(){
        dynamicsWorld->removeRigidBody(rigidbody);
        
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
            dynamicsWorld->removeRigidBody(rigidbody);
            dynamicsWorld->addRigidBody(rigidbody, rigidbody_collision_group, rigidbody_collision_mask);
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
            dynamicsWorld->removeRigidBody(rigidbody);
            dynamicsWorld->addRigidBody(rigidbody, rigidbody_collision_group, rigidbody_collision_mask);
        }
    }

    /// Sets the collision shape of the physics object.
    void PhysicsComponent::SetShape (Physics::CollisionShape shape) {
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
    void PhysicsComponent::Push (const glm::vec3& direction){
        if (!is_ready) return;
        
        rigidbody->activate(); // force awake, sleeping objects won't move
        rigidbody->applyCentralImpulse(btVector3(direction.x, direction.y, direction.z));
    }
    
    /// Sets the sleep state of the physics object.
    /// If set to true, then the physics object will be put to sleep, otherwise
    /// it will be awakened. By default they start asleep.
    void PhysicsComponent::SetSleep (bool sleep) {
        if (is_ready && sleep) {
            rigidbody->setActivationState(0);
        }
        
        if (is_ready && !sleep) {
            rigidbody->activate();
        }
        
        rigidbody_should_sleep = sleep;
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
    /// @note SetActivation to true, otherwise will not follow!
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
    
    /// Sets the position of the physics object.
    void PhysicsComponent::SetLocation (const glm::vec3& location) {
        btTransform trans = rigidbody->getWorldTransform();
        trans.setOrigin(btVector3 (location.x, location.y, location.z));
        rigidbody->setWorldTransform(trans);
    }
    
    /// Sets the rotation of the physics object.
    void PhysicsComponent::SetRotation (const glm::quat& rotation) {
        btTransform trans = rigidbody->getWorldTransform();
        trans.setRotation(btQuaternion (rotation.x, rotation.y, rotation.z, rotation.w));
        rigidbody->setWorldTransform(trans);
    }
    
    /// Sets the angular factor of the physics object.
    /// Changes how much the object will rotate around the x, y, z axes. Can be
    /// used to restrict rotation around certain axes.
    void PhysicsComponent::SetAngularFactor (const glm::vec3& factor) {
        rigidbody_angular_factor = factor;
        
        if (is_ready) {
            rigidbody->setAngularFactor({factor.x, factor.y, factor.z});
        }
    }
    
    /// Sets the angular factor of the physics object.
    /// Changes how much the object will move along the x, y, z axes. Can be
    /// used to restrict movement along certain axes.
    void PhysicsComponent::SetLinearFactor (const glm::vec3& factor) {
        rigidbody_linear_factor = factor;
        
        if (is_ready) {
            rigidbody->setLinearFactor({factor.x, factor.y, factor.z});
        }
    }
    
    /// Sets the activation of the physics object.
    /// Set it to true, if you don't want the object to ever fall asleep. Set it
    /// to false, if you want it to fall asleep when it stops moving.
    void PhysicsComponent::SetActivation (bool activation) {
        if (is_ready && activation) {
            rigidbody->setActivationState(DISABLE_DEACTIVATION);
        }

        if (is_ready && !activation) {
            rigidbody->setActivationState(WANTS_DEACTIVATION);
        }
        
        rigidbody_should_awake = activation;
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
}