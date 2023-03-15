// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef ENTITIES_CRATE_H
#define ENTITIES_CRATE_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serializeddata.h>

namespace tram {
    class RenderComponent;
    class PhysicsComponent;
    
    class Crate : public Entity {
    public:
        Crate(std::string_view& str);

        Crate(const char* nname, const char* modelname, const char* collisionmodelname, glm::vec3 pos, glm::vec3 rot);
        void UpdateParameters();
        void SetParameters();
        void Load();
        void Unload();
        void Serialize();
        void MessageHandler(Message& msg);
        void Testingolingo();

        class Data: public SerializedEntityData {
        public:
            Field<name_t> model;
            Field<name_t> collmodel;

            void ToString(std::string& str) {
                model.ToString(str);
                collmodel.ToString(str);
            }

            void FromString(std::string_view& str) {
                model.FromString(str);
                collmodel.FromString(str);
            }
            
            std::vector<FieldInfo> GetFieldInfo() {
                return std::vector<FieldInfo> {
                     { FieldInfo::FIELD_MODELNAME, "model" },
                     { FieldInfo::FIELD_STRING, "collision-model" },
                };
            }
            
            char const* GetType() {
                return "crate";
            }
        };
    protected:
        Component<RenderComponent> rendercomponent;
        Component<PhysicsComponent> physicscomponent;
        SerializedData<Data> serializeddata;
    };
}

#endif // ENTITIES_CRATE_H