#include <components/triggercomponent.h>
#include <physics/bullet/bullet.h>

using namespace Core::Physics;

namespace Core {
    template <> Pool<TriggerComponent> PoolProxy<TriggerComponent>::pool("trigger component pool", 50, false);
    
    void TriggerComponent::Start(){
        if (model.get())
            shape = (btCollisionShape*)model->GetPtr();

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
        
        dynamicsWorld->addCollisionObject(trigger);
        
        
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
    };

    void TriggerComponent::SetCollisionMask(uint32_t flags){
        collisionMask = flags;
        /*if (is_ready) {
            dynamicsWorld->removeRigidBody(rigidBody);
            dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        }*/
    }
    
    void TriggerComponent::SetCollisionGroup(uint32_t flags){
        collisionGroup = flags;
        /*if (is_ready) {
            dynamicsWorld->removeRigidBody(rigidBody);
            dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        }*/
    }
    
    void TriggerComponent::Collision (PhysicsComponent* other) {
        if (!filter_callback || filter_callback(this, other)) {
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
}