// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// LAMP.H -- Lamp entity

#ifndef LAMP_H
#define LAMP_H

#include <core.h>
#include <serializeddata.h>

namespace Core {
    class LightComponent;
    
    class Lamp : public Entity {
    public:
        Lamp(std::string_view& str);
        void UpdateParameters();
        void Load();
        void Unload();
        void Serialize();
        void SerializeString(std::string& str);
        void MessageHandler(Message& msg);
    protected:
        struct Data {
            void* padding = nullptr;
            float color[3];
            float distance;
        };

        LightComponent* light;
    };
}

#endif //LAMP_H