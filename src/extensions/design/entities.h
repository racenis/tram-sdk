// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_DESIGN_ENTITIES_H
#define TRAM_SDK_EXTENSIONS_DESIGN_ENTITIES_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serialization.h>

namespace tram {
    class RenderComponent;
    class PhysicsComponent;
}

namespace tram::Ext::Design {

class Button : public Entity {
public:
    Button(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void Update(); void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    void SwitchState(int);
    
    uint32_t flags;
    name_t model;
    name_t collmodel;
    vec3 origin;
    vec3 direction;
    float speed;
    float pause;
    float distance;
    name_t sound;
    name_t parent;
    
    vec3 parent_offset;
    vec3 start_position;
    
    float progress;
    float wait;
    int state;
};

class Breakable : public Entity {
public:
    Breakable(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class Water : public Entity {
public:
    Water(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class Door : public Entity {
public:
    Door(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class Train : public Entity {
public:
    Train(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class Rotating : public Entity {
public:
    Rotating(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class Pickup : public Entity {
public:
    Pickup(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class DebugText : public Entity {
public:
    DebugText(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class DebugShape : public Entity {
public:
    DebugShape(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class TriggerAutosave : public Entity {
public:
    TriggerAutosave(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class TriggerHurt : public Entity {
public:
    TriggerHurt(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class TriggerOnce : public Entity {
public:
    TriggerOnce(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class TriggerMultiple : public Entity {
public:
    TriggerMultiple(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class TriggerPush : public Entity {
public:
    TriggerPush(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class TriggerRemove : public Entity {
public:
    TriggerRemove(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class TriggerTeleport : public Entity {
public:
    TriggerTeleport(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

class TriggerPlayerMovement : public Entity {
public:
    TriggerPlayerMovement(const SharedEntityData&, const ValueArray&);
    void UpdateParameters(); void SetParameters();
    void Load(); void Unload(); void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

}

#endif // TRAM_SDK_EXTENSIONS_DESIGN_ENTITIES_H