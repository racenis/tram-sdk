// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef ENTITIES_PLAYER_H
#define ENTITIES_PLAYER_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serialization.h>

#include <components/player.h>
#include <components/controller.h>

namespace tram {

class Player : public Entity {
public:
    Player();
    ~Player() {std::cout << "Player destructor!" << std::endl; Unload();}
    void Load();
    void Unload();
    void Serialize();
    void SerializeString(std::string& str);
    void UpdateParameters();
    void SetParameters();
    void MessageHandler(Message& msg);
protected:
    Component<PlayerComponent> plomp;
    Component<ControllerComponent> controllercomponent;
};

}

#endif // ENTITIES_PLAYER_H