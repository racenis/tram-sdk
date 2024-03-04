// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_ENTITIES_DECORATION_H
#define TRAM_SDK_ENTITIES_DECORATION_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serialization.h>

namespace tram {

class RenderComponent;
class AnimationComponent;
class PhysicsComponent;

class Decoration : public Entity {
public:
    Decoration(const SharedEntityData&, const ValueArray&);
    void UpdateParameters();
    void SetParameters();
    void Load();
    void Unload();
    void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<AnimationComponent> animationcomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t animation;
};

}

#endif // TRAM_SDK_ENTITIES_DECORATION_H