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
    ~Player() {std::cout << "Player destructor!" << std::endl; Unload();}
    void Load();
    void Unload();
    void Serialize();
    void SerializeString(std::string& str);
    void UpdateParameters();
    void SetParameters();
    void MessageHandler(Message& msg);
    name_t GetType();
protected:
    Component<PlayerComponent> plomp;
public:
    Component<RaycastControllerComponent> controllercomponent;
};

}

#endif // TRAM_SDK_ENTITIES_PLAYER_H