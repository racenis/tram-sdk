// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_ENTITIES_LIGHT_H
#define TRAM_SDK_ENTITIES_LIGHT_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serialization.h>

namespace tram {

class LightComponent;

class Light : public Entity {
public:
    Light(const SharedEntityData&, const ValueArray&);
    void UpdateParameters();
    void SetParameters();
    void Load();
    void Unload();
    void Serialize();
    void MessageHandler(Message& msg);
    name_t GetType();
    static void Register();
protected:
    Component<LightComponent> light;
    
    vec3 color;
    float distance;
    vec3 direction;
    float exponent;
};

}

#endif // TRAM_SDK_ENTITIES_LIGHT_H