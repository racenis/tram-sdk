// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_ENTITIES_LAMP_H
#define TRAM_SDK_ENTITIES_LAMP_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serialization.h>

namespace tram {

class LightComponent;

class Lamp : public Entity {
public:
    Lamp(const SharedEntityData&, const SerializedFieldArray&);
    void UpdateParameters();
    void SetParameters();
    void Load();
    void Unload();
    void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<LightComponent> light;
    
    float color_r;
    float color_g;
    float color_b;
    float distance;
};

}

#endif // TRAM_SDK_ENTITIES_LAMP_H