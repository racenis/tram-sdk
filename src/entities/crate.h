// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// CRATE.H -- Crate entity

#ifndef CRATE_H
#define CRATE_H

#include <core.h>
#include <serializeddata.h>

namespace Core {
    class RenderComponent;
    class PhysicsComponent;
    
    class Crate : public Entity {
    public:
        Crate(std::string_view& str);

        Crate(const char* nname, const char* modelname, const char* collisionmodelname, glm::vec3 pos, glm::vec3 rot);
        void UpdateParameters();
        void Load();
        void Unload();
        void Serialize();
        void MessageHandler(Message& msg);
    protected:
        struct DataOLD {
            void* padding = nullptr;
            uint64_t model;
            uint64_t collmodel;
        };

        class Data: public SerializedEntityData {
        public:
            Field<name_t> model;
            Field<name_t> collmodel;

            void ToString(std::string& str) {
                model.ToString(str);
                collmodel.ToString(str);
            }

            void FromString(std::string_view& str) {
                model.FromStringAsName(str);
                collmodel.FromStringAsName(str);
            }
        };

        RenderComponent* rendercomponent;
        PhysicsComponent* physicscomponent;
    };
}

#endif //CRATE_H