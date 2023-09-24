// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_ENTITIES_CRATE_H
#define TRAM_SDK_ENTITIES_CRATE_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serialization.h>

namespace tram {

class RenderComponent;
class PhysicsComponent;

class Crate : public Entity {
public:
    Crate(const SharedEntityData&, const ValueArray&);

    Crate(const char* nname, const char* modelname, const char* collisionmodelname, glm::vec3 pos, glm::vec3 rot);
    void UpdateParameters();
    void SetParameters();
    void Load();
    void Unload();
    void Serialize();
    void MessageHandler(Message& msg);
    void Testingolingo();
    static void Register();
protected:
    Component<RenderComponent> rendercomponent;
    Component<PhysicsComponent> physicscomponent;
    
    name_t model;
    name_t collmodel;
};

}

#endif // TRAM_SDK_ENTITIES_CRATE_H