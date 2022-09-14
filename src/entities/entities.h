// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// ENTITIES.H -- Base entities
// Basic entity types.

#ifndef ENTITIES_H
#define ENTITIES_H

//#include <entitycomponents.h>
//#include <serializeddata.h>

namespace Core {
    // TODO: factor out all of these

    // the mongus, bus and tram entities are a bit broken
    // right now, so they're disabled
    #ifdef BENIGONIS
    class Mongus : public Entity {
    public:
        Mongus(std::string_view& str);

        Mongus(const char* nname, glm::vec3 pos, glm::vec3 rot);

        void UpdateParameters();

        void Load();

        void Unload();

        void Serialize();

        void SerializeString(std::string& str);

        void MessageHandler(Message& msg);

    protected:
        enum mongus_type{
            MONGUS_MONGUS,
            MONGUS_BINGUS,
            MONGUS_FLOPPA
        };

        struct DataOLD {
            void* padding = nullptr;
            mongus_type mtype = MONGUS_MONGUS;
        };

        RenderComponent* rendercomponent;
        PhysicsComponent* physicscomponent;
        ControllerComponent* controllercomponent;
        ArmatureComponent* armaturecomponent;
        AIComponent* aicomponent;
    };

    class Tram : public Entity {
    public:
        Tram(std::string_view& str);
        void UpdateParameters();
        void Load();
        void Unload();
        void Serialize();
        void SerializeString(std::string& str);
        void MessageHandler(Message& msg);

    protected:
        struct DataOLD {
            void* padding = nullptr;
            uint64_t model;
            uint64_t collmodel;
            uint64_t bogeycollmodel;
        };

        RenderComponent* rendercomponent;
        ArmatureComponent* armaturecomponent;
        PhysicsComponent* body_pcomp;
        TramComponent* tramcomp;
    };

    

    class Bus : public Entity {
    public:
        Bus(std::string_view& str);
        void UpdateParameters();
        void Load();
        void Unload();
        void Serialize();
        void SerializeString(std::string& str);
        void MessageHandler(Message& msg);
    protected:
        struct DataOLD {
            void* padding = nullptr;
            uint64_t model;
            uint64_t collmodel;
            uint64_t wheel_collmodel;   // unused. delete
        };

        RenderComponent* rendercomponent;
        ArmatureComponent* armaturecomponent;
        PhysicsComponent* body_pcomp;
        BusComponent* buscomp;
    };
    
    #endif
    
}

#endif // ENTITIES_H
