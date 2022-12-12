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
        void SetParameters();
        void Load();
        void Unload();
        void Serialize();
        void MessageHandler(Message& msg);
        void Testingolingo();
    
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
                model.FromString(str);
                collmodel.FromString(str);
            }
            
            name_t GetEditorModel() {
                return model;
            }
            
            std::vector<FieldInfo> GetEditorFieldInfo() {
                return std::vector<FieldInfo> {
                     { FieldInfo::FIELD_NAME, "Model", "model", &model },
                     { FieldInfo::FIELD_NAME, "CollisionMdl", "collision-model", &collmodel },
                };
            }
            
            char const* GetDataName() {
                return "crate";
            }
            
            char const* GetEditorName() {
                return "Crate";
            }
        };
    protected:
        Component<RenderComponent> rendercomponent;
        Component<PhysicsComponent> physicscomponent;
        SerializedData<Data> serializeddata;
    };
}

#endif //CRATE_H