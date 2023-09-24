// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef ENTITIES_STATICWORLDOBJECT_H
#define ENTITIES_STATICWORLDOBJECT_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serialization.h>

namespace tram {

class RenderComponent;
class PhysicsComponent;

class StaticWorldObject : public Entity {
public:
    StaticWorldObject(const SharedEntityData&, const ValueArray&);
    void UpdateParameters();
    void SetParameters();
    void Load();
    void Unload();
    void Serialize();
    void MessageHandler(Message& msg);
    static void Register();


protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t lightmap;
};

}

#endif // ENTITIES_STATICWORLDOBJECT_H