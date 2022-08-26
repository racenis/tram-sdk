// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// PHYSICS.H -- Physics library wrapper
// Wrapper for the physics library. It's seperate, so that engine can compile without
// the physics library, because it is a pain in the     to compile.

#ifndef PHYSICS_H
#define PHYSICS_H

#include <core.h>

class btCollisionShape;
class btMotionState;
class btRigidBody;
class btGeneric6DofConstraint;
class btPoint2PointConstraint;
class btHingeConstraint;
class btActionInterface;
class btRaycastVehicle;

namespace Core {
    class PhysicsComponent;
    class ArmatureComponent;    
}

namespace Core::Physics {
    void InitPhysics();
    void StepPhysics(float time);

    enum CollisionGroup { // TODO: specify the underlying type for this enum (i think it's uint32_t)
        COLL_WORLDOBJ = 2,   /// StaticWorldObject's, etc.
        COLL_DYNAMICOBJ = 4, /// Crates
        COLL_VEHICLE = 8,    /// Trams & busses
        COLL_PLAYER = 16,    /// Player
        COLL_MONGUS = 32,    /// Monguses
        COLL_BINGUS = 64,    /// Binguses
        COLL_FLOPPA = 128    /// Floppas
    };

    class CollisionModel : public Resource{
    public:
        CollisionModel(){}
        CollisionModel(uint64_t mName){
            name = mName;
            status = UNLOADED;
            //res_type = RESOURCE_COLLISONMODEL;
        }

        void LoadFromDisk();
        void LoadFromMemory(){}
        inline void* GetPtr(){return model;}

        static CollisionModel* Find(uint64_t modelName);
    protected:
        // TODO: change this to the actual type
        void* model = nullptr;  /// Pointer to the physics library's collision shape.
        static std::unordered_map<uint64_t, CollisionModel*> List;
    };



    PhysicsComponent* Raycast(const glm::vec3& from, const glm::vec3& to);



}

namespace Core {
    /// Wrapper around physics library.
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
    
    #ifdef BENIGONIS
    class BusComponent : public EntityComponent {
    public:
        void EventHandler(Event &event);
        bool FindOffsets();
        void Init();
        void Uninit();
        void SetModel(uint64_t name){
            //model = FindModelPtr(name);
            model.SetResource(Render::Model::Find(name));
        };
        void SetParent(Entity* ent){
            parent = ent;
        }
        void SetPhysicsComponent(PhysicsComponent* comp){
            body_physcomp = comp;
        }
        void SetArmatureComponent(ArmatureComponent* comp){
            armature = comp;
        }
        void Activate();

    private:
        Entity* parent = nullptr;
        EventListener* keypress = nullptr;

        EventListener* keydown = nullptr;
        EventListener* keyup = nullptr;

        ArmatureComponent* armature = nullptr;
        //Render::Model* model = nullptr;
        ResourceProxy<Render::Model> model;

        PhysicsComponent* body_physcomp;    //don't delete this one, the entity code will delete it


        //btRaycastVehicle::btVehicleTuning vtuna;
        btRaycastVehicle* vehicle;


        glm::vec3 offset_wheel_fl;  // front left
        glm::vec3 offset_wheel_fr;  // front right
        glm::vec3 offset_wheel_bl;  // back left
        glm::vec3 offset_wheel_br;  // back right

        bool is_controlled = false;

    };
    #endif // BENIGONIS
}
#endif // PHYSICS_H
