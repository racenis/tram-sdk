// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// LAMP.H -- Lamp entity

#ifndef LAMP_H
#define LAMP_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serializeddata.h>

namespace Core {
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
            
            name_t GetEditorModel() {
                return UID("mongus");
            }
            
            std::vector<FieldInfo> GetEditorFieldInfo() {
                return std::vector<FieldInfo> {
                     { FieldInfo::FIELD_FLOAT, "Color R", "color-r", &color_r },
                     { FieldInfo::FIELD_FLOAT, "Color G", "color-g", &color_g },
                     { FieldInfo::FIELD_FLOAT, "Color B", "color-b", &color_b },
                     { FieldInfo::FIELD_FLOAT, "Distance", "distance", &distance },
                };
            }
            
            char const* GetDataName() {
                return "lamp";
            }
            
            char const* GetEditorName() {
                return "Lamp";
            }
        };
    protected:
        Component<LightComponent> light;
        SerializedData<Data> data;
    };
}

#endif //LAMP_H