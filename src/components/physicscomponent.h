// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// PHYSICSCOMPONENT.H -- Core entity components
// Basic components for the basic entities

#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include <physics.h>

namespace Core {
    class PhysicsComponent : public EntityComponent {
    public:
        PhysicsComponent() : model(this){}
        void EventHandler(Event &event){};
        bool SetUpRigidBody();
        void Init();
        void Uninit();
        void Start();
        bool IsReady(){return is_ready;};

        uint64_t GetModel(){return model.GetResource()->GetName();};
        void SetParent(Entity* newparent){parent = newparent;};
        Entity* GetParent(){return parent;};
        void SetModel(uint64_t modelName){
            //collModel = Physics::CollisionModel::Find(modelName);
            model.SetResource(Physics::CollisionModel::Find(modelName));
        };

        void SetMass(float nMass);
        void SetShapeCapsule(float thickness, float length);
        void GetAABB(glm::vec3& aabb_min, glm::vec3& aabb_max);

        void Push (const glm::vec3& direction);
        void PushLocal (const glm::vec3& direction);
        void PushRotate(const glm::vec3& direction);
        void SetCollisionMask(uint32_t flags);
        void SetCollisionGroup(uint32_t flags);
        void PutToSleep();
        void SetStartAsleep();
        void DisableDebugDrawing();
        void SetHingeConstraint(PhysicsComponent* other_component, glm::vec3 axis, glm::vec3 point_this, glm::vec3 point_other);
        void UnsetHingeConstraint();
        void SetHingeAxis(const glm::vec3& axis);
        void SetLocalRotation(const glm::quat& rotation);
        void SetOffset(const glm::vec3& offset);
        void EnableRotation();
        void DisableRotation();
        void EnableMovement();
        void DisableMovement();
        void SetFriction(float f);
        void SetDamping(float f);
        void SetRestitution(float f);
        void SetVelocity(glm::vec3& v);
        void SetLinearFactor(float f);
        void DisableActivation();
        void EnableActivation();
        void SetBone(uint64_t boneName, ArmatureComponent* boneArmature){
            bone = boneName;
            armature = boneArmature;
        }
        float GetVelocity();
        void SetPathPreference (char pathPreference){
            path_pref = pathPreference;
        }
        void SetPath();
        void UpdatePath();

        void UpdatePathRotate(const glm::vec3& loc);

        void Glue(PhysicsComponent* comp);

        void Unglue();

    private:
        Entity* parent = nullptr;
        ArmatureComponent* armature = nullptr;
        uint64_t bone = 0;
        //Physics::CollisionModel* collModel = nullptr;
        ResourceProxy<Physics::CollisionModel> model;
        btCollisionShape* shape = nullptr;
        btMotionState* motionState = nullptr;
        btRigidBody* rigidBody = nullptr;
        btGeneric6DofConstraint* pathConstraint = nullptr;
        btPoint2PointConstraint* glueConstraint = nullptr;
        btHingeConstraint* hingeConstraint = nullptr;
        PathNode* prev_path = nullptr;
        PathNode* next_path = nullptr;
        char path_pref = 's';
        float mass = 1.0f;
        glm::vec3 off_set = glm::vec3(0.0f, 0.0f, 0.0f);
        bool is_ready = false;
        bool should_asleep = false;
        uint32_t collisionMask = -1;
        uint32_t collisionGroup = 1;

        btActionInterface* action = nullptr;

        glm::vec3 p_start;
        glm::vec3 p_found;

        friend class BusComponent;
    };
}

#endif //PHYSICSCOMPONENT_H