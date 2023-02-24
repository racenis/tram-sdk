// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef ENTITIES_STATICWORLDOBJECT_H
#define ENTITIES_STATICWORLDOBJECT_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
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
            
            std::vector<FieldInfo> GetFieldInfo() {
                return std::vector<FieldInfo> {
                     { FieldInfo::FIELD_MODELNAME, "model", },
                     { FieldInfo::FIELD_STRING, "lightmap", },
                };
            }
            
            char const* GetType() {
                return "staticwobj";
            }
        };
    protected:
        Component<RenderComponent> rendercomponent;
        Component<PhysicsComponent> physicscomponent;
        
        SerializedData<Data> data;
    };
}

#endif // ENTITIES_STATICWORLDOBJECT_H