#include <components/physicscomponent.h>
#include <physics/bullet/bullet.h>

#include <physics/bullet/motionstates.h>

using namespace Core::Physics;

namespace Core {
    template <> Pool<PhysicsComponent> PoolProxy<PhysicsComponent>::pool("physics component pool", 250, false);
    
    void PhysicsComponent::Start(){
        if (model.get())
            shape = (btCollisionShape*)model->GetPtr();


        //if(bone == 0)
        motionState = new EntMotionState(parent, off_set);
        //else
        //    motionState = new ArmMotionState(bone, armature, off_set, parent, this);

        btScalar bmass = mass;
        btVector3 inertia(0.0f, 0.0f, 0.0f);
        shape->calculateLocalInertia(bmass, inertia);
        btRigidBody::btRigidBodyConstructionInfo rigidBodyConstructionInfo(bmass, motionState, shape, inertia);

        rigidBody = new btRigidBody(rigidBodyConstructionInfo);

        dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        rigidBody->setUserPointer(this);
        rigidBody->setUserIndex(USERINDEX_PHYSICSCOMPONENT);

        if(should_asleep) rigidBody->setActivationState(0);

        is_ready = true;

        //return true;
    }
    PhysicsComponent::~PhysicsComponent(){
        dynamicsWorld->removeRigidBody(rigidBody);
        delete rigidBody;
        delete motionState;
    };


    void PhysicsComponent::SetShape(Physics::CollisionShape shape) {
        this->shape = CollisionShapeToConvexShape(shape);
    }
    
    void PhysicsComponent::GetAABB(glm::vec3& aabb_min, glm::vec3& aabb_max){
        btVector3 b_min, b_max;
        rigidBody->getAabb(b_min, b_max);

        aabb_min.x = b_min.getX();
        aabb_min.y = b_min.getY();
        aabb_min.z = b_min.getZ();

        aabb_max.x = b_max.getX();
        aabb_max.y = b_max.getY();
        aabb_max.z = b_max.getZ();
    }
    
    void PhysicsComponent::SetMass(float nMass){mass = nMass;}
    
    void PhysicsComponent::Push (const glm::vec3& direction){
        if(rigidBody == nullptr) return;
        rigidBody->activate();
        rigidBody->applyCentralImpulse(btVector3(direction.x, direction.y, direction.z));
    }
    
    void PhysicsComponent::PushLocal (const glm::vec3& direction){
        if(rigidBody == nullptr) return;

        btTransform trans = rigidBody->getWorldTransform();
        btQuaternion rot_bt = trans.getRotation();
        glm::quat rot(rot_bt.getX(), rot_bt.getY(), rot_bt.getZ(), rot_bt.getW());

        glm::vec3 p_dir = rot * direction;

        rigidBody->activate();
        rigidBody->applyCentralImpulse(btVector3(p_dir.x, p_dir.y, p_dir.z));
    }
    
    void PhysicsComponent::SetCollisionMask(uint32_t flags){
        collisionMask = flags;
        if(is_ready){
            dynamicsWorld->removeRigidBody(rigidBody);
            dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        }
    }
    
    void PhysicsComponent::SetCollisionGroup(uint32_t flags){
        collisionGroup = flags;
        if(is_ready){
            dynamicsWorld->removeRigidBody(rigidBody);
            dynamicsWorld->addRigidBody(rigidBody, collisionGroup, collisionMask);
        }
    }
    

    void PhysicsComponent::SetSleep(bool sleep) {
        if (sleep) {
            if (rigidBody) rigidBody->setActivationState(0);
            should_asleep = true;
        } else {
            if (rigidBody) rigidBody->activate();
            should_asleep = false;
        }
    }
    
    
    void PhysicsComponent::DisableDebugDrawing(){
        rigidBody->setCollisionFlags(btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    }
  
    void PhysicsComponent::SetLocation (const glm::vec3& location) {
        assert(rigidBody);
        btTransform trans = rigidBody->getWorldTransform();
        trans.setOrigin(btVector3 (location.x, location.y, location.z));
        rigidBody->setWorldTransform(trans);
    }
    
    void PhysicsComponent::SetRotation (const glm::quat& rotation) {
        assert(rigidBody);
        btTransform trans = rigidBody->getWorldTransform();
        trans.setRotation(btQuaternion (rotation.x, rotation.y, rotation.z, rotation.w));
        rigidBody->setWorldTransform(trans);
    }
        
    
    void PhysicsComponent::SetLocalRotation(const glm::quat& rotation){
        glm::quat parent_rotation;
        parent->GetRotation(parent_rotation);

        glm::quat final_rotation = parent_rotation * rotation;

        btTransform trans = rigidBody->getWorldTransform();
        trans.setRotation(btQuaternion (final_rotation.x, final_rotation.y, final_rotation.z, final_rotation.w));
        rigidBody->setWorldTransform(trans);
    }    
    
    void PhysicsComponent::EnableRotation () {
        rigidBody->setAngularFactor(1.0f);
    }
    
    void PhysicsComponent::DisableRotation () {
        rigidBody->setAngularFactor(0.0f);
    }
    
    void PhysicsComponent::EnableMovement () {
        rigidBody->setLinearFactor(btVector3(1.0f, 1.0f, 1.0f));
    }
    
    void PhysicsComponent::DisableMovement () {
        rigidBody->setLinearFactor(btVector3(0.0f, 0.0f, 0.0f));
    }

    
    void PhysicsComponent::SetActivation (bool activation) {
        if (activation) {
            rigidBody->setActivationState(DISABLE_DEACTIVATION);
        } else {
            rigidBody->setActivationState(WANTS_DEACTIVATION);
        }
    }
        
    void PhysicsComponent::SetVelocity (const vec3& velocity){
        if (velocity.x != 0.0f &&
            velocity.y != 0.0f &&
            velocity.z != 0.0f
        ) {
            rigidBody->activate();
        }
        
        rigidBody->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
    }

    vec3 PhysicsComponent::GetVelocity () {
        auto velocity = rigidBody->getLinearVelocity();
        return vec3 (velocity.getX(), velocity.getY(), velocity.getZ());
    }
}