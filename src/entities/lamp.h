// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef ENTITIES_LAMP_H
#define ENTITIES_LAMP_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serializeddata.h>

namespace tram {
    class LightComponent;
    
    class Lamp : public Entity {
    public:
        Lamp(std::string_view& str);
        void UpdateParameters();
        void SetParameters();
        void Load();
        void Unload();
        void Serialize();
        void MessageHandler(Message& msg);
        
        class Data: public SerializedEntityData {
        public:
            Field<float> color_r;
            Field<float> color_g;
            Field<float> color_b;
            Field<float> distance;

            void ToString(std::string& str) {
                color_r.ToString(str);
                color_g.ToString(str);
                color_b.ToString(str);
                distance.ToString(str);
            }

            void FromString(std::string_view& str) {
                color_r.FromString(str);
                color_g.FromString(str);
                color_b.FromString(str);
                distance.FromString(str);
            }
            
            std::vector<FieldInfo> GetFieldInfo() {
                return std::vector<FieldInfo> {
                     { FieldInfo::FIELD_FLOAT, "color-r" },
                     { FieldInfo::FIELD_FLOAT, "color-g" },
                     { FieldInfo::FIELD_FLOAT, "color-b" },
                     { FieldInfo::FIELD_FLOAT, "distance" },
                };
            }
            
            char const* GetType() {
                return "lamp";
            }
        };
    protected:
        Component<LightComponent> light;
        SerializedData<Data> data;
    };
}

#endif // ENTITIES_LAMP_H