#include <components/triggercomponent.h>
#include <physics/bullet/bullet.h>

using namespace Core::Physics;

namespace Core {
    template <> Pool<TriggerComponent> PoolProxy<TriggerComponent>::pool("trigger component pool", 50, false);
    
    void TriggerComponent::Start(){
        if (!shape && model.get()) {
            shape = (btCollisionShape*) model->GetPtr();
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
        //trigger->setCollisionShape(new btBoxShape(btVector3(btScalar(0.99f),btScalar(0.99f),btScalar(0.99f))));
        trigger->setCollisionShape(shape);
        trigger->setWorldTransform(transform);
        trigger->setUserPointer(this);
        trigger->setUserIndex(USERINDEX_TRIGGERCOMPONENT);
        trigger->setCollisionFlags(trigger->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        
        dynamicsWorld->addCollisionObject(trigger, collisionGroup, collisionMask);
        
        
        //dynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

	
        /*
        if(bone == 0)
            motionState = new EntMotionState(parent, off_set);
            else
                motionState = new ArmMotionState(bone, armature, off_set, parent, this);

        btScalar bmass = mass;
        btVector3 inertia(0.0f, 0.0f, 0.0f);
        shape->calculateLocalInertia(bmass, inertia);
        btRigidBody::btRigidBodyConstructionInfo rigidBodyConstructionInfo(bmass, motionState, shape, inertia);

        rigidBody = new btRigidBody(rigidBodyConstructionInfo);

        dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        rigidBody->setUserPointer(this);

        if(should_asleep) rigidBody->setActivationState(0);
        */
        

        is_ready = true;

        //return true;
    }
    
    TriggerComponent::~TriggerComponent(){
        dynamicsWorld->removeCollisionObject(trigger);
        delete trigger;
        
        if (shape && !model.get()) delete shape;
    };

    void TriggerComponent::SetShape(Physics::CollisionShape shape) {
        this->shape = CollisionShapeToConvexShape(shape);
    }

    void TriggerComponent::SetCollisionMask(uint32_t flags){
        collisionMask = flags;
        
        if (is_ready) {
            dynamicsWorld->removeCollisionObject(trigger);
            dynamicsWorld->addCollisionObject(trigger, collisionGroup, collisionMask);
        }
    }
    
    void TriggerComponent::SetCollisionGroup(uint32_t flags){
        collisionGroup = flags;
        
        if (is_ready) {
            dynamicsWorld->removeCollisionObject(trigger);
            dynamicsWorld->addCollisionObject(trigger, collisionGroup, collisionMask);
        }
    }
    
    void TriggerComponent::SetLocation (glm::vec3 location) {
        this->location = location;
        
        if (is_ready) {
            btTransform trans = trigger->getWorldTransform();
            trans.setOrigin(btVector3 (location.x, location.y, location.z));
            trigger->setWorldTransform(trans);
        }
    }
    
    void TriggerComponent::SetRotation (glm::quat rotation) {
        this->rotation = rotation;
        
        if (is_ready) {
            btTransform trans = trigger->getWorldTransform();
            trans.setRotation(btQuaternion (rotation.x, rotation.y, rotation.z, rotation.w));
            trigger->setWorldTransform(trans);
        }
    }
    
    void TriggerComponent::Collision (const Physics::Collision& collision) {
        if (!filter_callback || filter_callback(this, collision.collider)) {
            if (!is_collided && !was_collided && activation_callback) activation_callback(this);

            is_collided = true;
        }
    }
    
    void TriggerComponent::ResetCollisions() {
        was_collided = is_collided;
        is_collided = false;
    }
    
    bool TriggerComponent::IsTriggered() {
        return true;
    }
    
    
    
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
                collisions.push_back({
                    (PhysicsComponent*) ob1->getUserPointer(),
                    {contact.getX(), contact.getY(), contact.getZ()}
                });
            }
            
            if (ob0->getUserIndex() == USERINDEX_PHYSICSCOMPONENT &&
                ob1->getUserIndex() == USERINDEX_TRIGGERCOMPONENT &&
                ((PhysicsComponent*)ob0->getUserPointer())->GetCollisionGroup() &
                ((TriggerComponent*)ob1->getUserPointer())->GetCollisionMask()) {
                assert(ob0->getUserPointer());
                assert(ob1->getUserPointer());
                auto& contact = cp.getPositionWorldOnB();
                collisions.push_back({
                    (PhysicsComponent*) ob0->getUserPointer(),
                    {contact.getX(), contact.getY(), contact.getZ()}
                });
            }
            
            return 1; // tbh idk what this method is supposed to return
        }
        std::vector<Physics::Collision>& collisions;
    };
    
    std::vector<Physics::Collision> TriggerComponent::Poll () {
        std::vector<Physics::Collision> collisions;
        
        TriggerPollCallback callback (collisions);
        dynamicsWorld->contactTest(trigger, callback);
        
        return collisions;
    }
}