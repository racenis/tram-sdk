// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

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
    ~Player() {std::cout << "Player destructor!" << std::endl; Unload();}
    void Load();
    void Unload();
    void Serialize();
    void SerializeString(std::string& str);
    void UpdateParameters();
    void SetParameters();
    void MessageHandler(Message& msg);
    value_t Query(query_t query);
protected:
    Component<PlayerComponent> plomp;
public:
    Component<ControllerComponent> controllercomponent;
};

}

#endif // TRAM_SDK_ENTITIES_PLAYER_H