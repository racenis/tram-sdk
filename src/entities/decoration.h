// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_ENTITIES_DECORATION_H
#define TRAM_SDK_ENTITIES_DECORATION_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serialization.h>

namespace tram {

class RenderComponent;
class AnimationComponent;
class PhysicsComponent;

// TODO: add a field that lets you set how fast the animation fades
// TODO: don't create animation component if there is no animation?

class Decoration : public Entity {
public:
    Decoration(const SharedEntityData&, const ValueArray&);
    void UpdateParameters();
    void SetParameters();
    void Load();
    void Unload();
    void Serialize();
    void MessageHandler(Message& msg);
    name_t GetType();
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<AnimationComponent> animationcomponent;
    Component<PhysicsComponent> physicscomponent;
    
    void RequestAnimationComponent();
    
    name_t model;
    name_t animation;
    
    uint32_t decoration_flags;
};

}

#endif // TRAM_SDK_ENTITIES_DECORATION_H