// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_ENTITIES_PLAYER_H
#define TRAM_SDK_ENTITIES_PLAYER_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serialization.h>

#include <components/player.h>
#include <components/controller.h>

namespace tram {

class Player : public Entity {
public:
    Player();
    Player(name_t);
    ~Player() {Log(Severity::WARNING, System::MISC, "Player destructor!"); Unload();}
    void Load();
    void Unload();
    void Serialize(ValueArray&);
    void SerializeString(std::string& str);
    void UpdateParameters();
    void SetParameters();
    void MessageHandler(Message& msg);
    name_t GetType();
protected:
    Component<PlayerComponent> plomp;
    Component<PhysicsComponent> physics_body;
public:
    // wait why is this public
    Component<FPSControllerComponent> controllercomponent;
};

}

#endif // TRAM_SDK_ENTITIES_PLAYER_H