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
class btCompoundShape;

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
        }

        void LoadFromDisk();
        void LoadFromMemory(){}
        inline void* GetPtr(){return model;}

        static CollisionModel* Find(uint64_t modelName);
    protected:
        // TODO: change this to the actual type
        btCompoundShape* model = nullptr;  /// Pointer to the physics library's collision shape.
        static std::unordered_map<uint64_t, CollisionModel*> List;
    };



    PhysicsComponent* Raycast(const glm::vec3& from, const glm::vec3& to);



}

namespace Core {
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
