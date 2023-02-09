// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_PHYSICSCOMPONENT_H
#define COMPONENTS_PHYSICSCOMPONENT_H

#include <physics/physics.h>
#include <physics/collisionmodel.h>
#include <physics/collisionshape.h>

class btCollisionShape;
class btMotionState;
class btRigidBody;
class btActionInterface;

namespace Core {
    class PhysicsComponent : public EntityComponent {
    public:
        PhysicsComponent() : model(this){}
        ~PhysicsComponent();
        void EventHandler(Event &event){};
        bool SetUpRigidBody();
        void Start();
        bool IsReady(){return is_ready;};

        name_t GetModel(){return model->GetName();};
        void SetParent(Entity* newparent){parent = newparent;};
        Entity* GetParent(){return parent;};
        void SetModel(name_t modelName){
            model = Physics::CollisionModel::Find(modelName);
        };

        void SetMass(float nMass);
        void SetShape(Physics::CollisionShape shape);
        void GetAABB(glm::vec3& aabb_min, glm::vec3& aabb_max);

        void Push (const glm::vec3& direction);
        void PushLocal (const glm::vec3& direction);

        void SetCollisionMask(uint32_t flags);
        void SetCollisionGroup(uint32_t flags);

        void DisableDebugDrawing();

        void SetSleep(bool sleep);
        
        void SetRotation(const glm::quat& rotation);
        void SetLocation(const glm::vec3& location);
        void SetLocalRotation(const glm::quat& rotation);
        
        
        void EnableRotation();
        void DisableRotation();
        void EnableMovement();
        void DisableMovement();

        void SetActivation(bool activation);

        void SetVelocity(const vec3& velocity);
        vec3 GetVelocity();

    private:
        Entity* parent = nullptr;

        glm::vec3 off_set = glm::vec3(0.0f, 0.0f, 0.0f);
        ResourceProxy<Physics::CollisionModel> model;
        btCollisionShape* shape = nullptr;
        btMotionState* motionState = nullptr;
        btRigidBody* rigidBody = nullptr;

        float mass = 1.0f;

        bool should_asleep = false;
        
        uint32_t collisionMask = -1;
        uint32_t collisionGroup = 1;

        btActionInterface* action = nullptr;
    };
}

#endif // COMPONENTS_PHYSICSCOMPONENT_H