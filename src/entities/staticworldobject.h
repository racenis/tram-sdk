// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// STATICWORLDOBJECT.H -- Staticworldobject entity

#ifndef STATICWORLDOBJECT_H
#define STATICWORLDOBJECT_H

#include <framework/core.h>
#include <framework/serializeddata.h>

namespace Core {
    class RenderComponent;
    class PhysicsComponent;
    
    class StaticWorldObject : public Entity {
    public:
        StaticWorldObject(std::string_view& str);
        void UpdateParameters();
        void SetParameters();
        void Load();
        void Unload();
        void Serialize();
        void MessageHandler(Message& msg);
    
        struct DataOLD {
            void* padding = nullptr;
            uint64_t model;
            uint64_t model_lod;
            uint64_t lightmap;
        };

        class Data: public SerializedEntityData {
        public:
            Field<name_t> model;
            Field<name_t> lightmap;

            void ToString(std::string& str) {
                model.ToString(str);
                lightmap.ToString(str);
            }

            void FromString(std::string_view& str) {
                model.FromString(str);
                lightmap.FromString(str);
            }
            
            name_t GetEditorModel() {
                return model;
            }
            
            std::vector<FieldInfo> GetEditorFieldInfo() {
                return std::vector<FieldInfo> {
                     { FieldInfo::FIELD_NAME, "Model", "model", &model },
                     { FieldInfo::FIELD_NAME, "Lightmap", "lightmap", &lightmap },
                };
            }
            
            char const* GetDataName() {
                return "staticwobj";
            }
            
            char const* GetEditorName() {
                return "Staticworldobject";
            }
        };
    protected:
        Component<RenderComponent> rendercomponent;
        Component<PhysicsComponent> physicscomponent;
        
        SerializedData<Data> data;
    };
}

#endif //STATICWORLDOBJECT_H