// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <components/triggercomponent.h>
#include <physics/bullet/bullet.h>

using namespace tram::Physics;

namespace tram {
    template <> Pool<TriggerComponent> PoolProxy<TriggerComponent>::pool("trigger component pool", 50, false);
    
    void TriggerComponent::Start(){
        if (!shape && model) {
            shape = (btCollisionShape*) model->GetShape();
        }
        
        assert(shape);

        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(btVector3(location.x, location.y, location.z));
        
        btQuaternion rotation_quat;
        rotation_quat.setX(rotation.x);
        rotation_quat.setY(rotation.y);
        rotation_quat.setZ(rotation.z);
        rotation_quat.setW(rotation.w);
        transform.setRotation(rotation_quat);
        
        trigger = new btCollisionObject();
        trigger->setCollisionShape(shape);
        trigger->setWorldTransform(transform);
        trigger->setUserPointer(this);
        trigger->setUserIndex(USERINDEX_TRIGGERCOMPONENT);
        trigger->setCollisionFlags(trigger->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        
        dynamicsWorld->addCollisionObject(trigger, collisionGroup, collisionMask);
        
        is_ready = true;
    }
    
    TriggerComponent::~TriggerComponent(){
        dynamicsWorld->removeCollisionObject(trigger);
        delete trigger;
        
        if (shape && !model.get()) delete shape;
    };

    /// Sets a collision shape for the trigger.
    void TriggerComponent::SetShape(Physics::CollisionShape shape) {
        this->shape = CollisionShapeToConvexShape(shape);
    }

    /// Sets a collision mask for the trigger.
    /// This method works the same as in PhysicsComponent, see
    /// PhysicsComponent::SetCollsionMask() for more information.
    void TriggerComponent::SetCollisionMask(uint32_t flags){
        collisionMask = flags;
        
        if (is_ready) {
            dynamicsWorld->removeCollisionObject(trigger);
            dynamicsWorld->addCollisionObject(trigger, collisionGroup, collisionMask);
        }
    }
    
    /// Sets a collision group for the trigger.
    /// This method works the same as in PhysicsComponent, see
    /// PhysicsComponent::SetCollisionGroup() for more information.
    void TriggerComponent::SetCollisionGroup(uint32_t flags){
        collisionGroup = flags;
        
        if (is_ready) {
            dynamicsWorld->removeCollisionObject(trigger);
            dynamicsWorld->addCollisionObject(trigger, collisionGroup, collisionMask);
        }
    }
    
    /// Sets the location of the component.
    void TriggerComponent::SetLocation (glm::vec3 location) {
        this->location = location;
        
        if (is_ready) {
            btTransform trans = trigger->getWorldTransform();
            trans.setOrigin(btVector3 (location.x, location.y, location.z));
            trigger->setWorldTransform(trans);
        }
    }
    
    /// Sets the rotation of the component.
    void TriggerComponent::SetRotation (glm::quat rotation) {
        this->rotation = rotation;
        
        if (is_ready) {
            btTransform trans = trigger->getWorldTransform();
            trans.setRotation(btQuaternion (rotation.x, rotation.y, rotation.z, rotation.w));
            trigger->setWorldTransform(trans);
        }
    }
    
    /// Registers a collision.
    /// This method is called from Phyics::Update().
    void TriggerComponent::Collision (const Physics::Collision& collision) {
        if (!filter_callback || filter_callback(this, collision.collider)) {
            if (!is_collided && !was_collided && activation_callback) {
                activation_callback(this, collision);
                this->collision = collision;
            }

            is_collided = true;
        }
    }
    
    /// Resets registered collisions.
    /// This method is called from Phyics::Update().
    void TriggerComponent::ResetCollisions() {
        if (!is_collided && was_collided && deactivation_callback) {
            deactivation_callback(this, collision);
        }
        
        was_collided = is_collided;
        is_collided = false;
    }
    
    // For the Bullet physics.
    struct TriggerPollCallback : public btCollisionWorld::ContactResultCallback {
        TriggerPollCallback(std::vector<Physics::Collision>& collisions) : collisions(collisions) {}
        btScalar addSingleResult(btManifoldPoint& cp,
            const btCollisionObjectWrapper* coll_obj0, int, int,
            const btCollisionObjectWrapper* coll_obj1, int, int
        ) {
            if (cp.getDistance() > 0.0f) return 1;
            
            const btCollisionObject* ob0 = coll_obj0->getCollisionObject();
            const btCollisionObject* ob1 = coll_obj1->getCollisionObject();
            
            if (ob0->getUserIndex() == USERINDEX_TRIGGERCOMPONENT &&
                ob1->getUserIndex() == USERINDEX_PHYSICSCOMPONENT &&
                ((TriggerComponent*)ob0->getUserPointer())->GetCollisionMask() &
                ((PhysicsComponent*)ob1->getUserPointer())->GetCollisionGroup()) {
                assert(ob0->getUserPointer());
                assert(ob1->getUserPointer());
                auto& contact = cp.getPositionWorldOnA();
                auto& normal = cp.m_normalWorldOnB;
                collisions.push_back({
                    (PhysicsComponent*) ob1->getUserPointer(),
                    {contact.getX(), contact.getY(), contact.getZ()},
                    {-normal.getX(), normal.getY(), normal.getZ()}
                });
            }
            
            if (ob0->getUserIndex() == USERINDEX_PHYSICSCOMPONENT &&
                ob1->getUserIndex() == USERINDEX_TRIGGERCOMPONENT &&
                ((PhysicsComponent*)ob0->getUserPointer())->GetCollisionGroup() &
                ((TriggerComponent*)ob1->getUserPointer())->GetCollisionMask()) {
                assert(ob0->getUserPointer());
                assert(ob1->getUserPointer());
                auto& contact = cp.getPositionWorldOnB();
                auto& normal = cp.m_normalWorldOnB;
                collisions.push_back({
                    (PhysicsComponent*) ob0->getUserPointer(),
                    {contact.getX(), contact.getY(), contact.getZ()},
                    {normal.getX(), normal.getY(), normal.getZ()}
                });
            }
            
            return 1; // tbh idk what this method is supposed to return
        }
        std::vector<Physics::Collision>& collisions;
    };
    
    /// Checks for collisions with the trigger.
    std::vector<Physics::Collision> TriggerComponent::Poll () {
        std::vector<Physics::Collision> collisions;
        
        TriggerPollCallback callback (collisions);
        dynamicsWorld->contactTest(trigger, callback);
        
        return collisions;
    }
}