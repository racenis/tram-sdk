// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_ENTITIES_TRIGGER_H
#define TRAM_SDK_ENTITIES_TRIGGER_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serialization.h>

namespace tram {

class RenderComponent;
class TriggerComponent;

class Trigger : public Entity {
public:
    Trigger(const SharedEntityData&, const ValueArray&);
    void UpdateParameters();
    void SetParameters();
    void Load();
    void Unload();
    void Serialize();
    void MessageHandler(Message& msg);
    void Activate();
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<TriggerComponent> triggercomponent;
    
    name_t model;
    uint32_t flags;
    uint32_t collision_mask;
};

}

#endif // TRAM_SDK_ENTITIES_TRIGGER_H