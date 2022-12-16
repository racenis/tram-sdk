// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// PLAYER.H -- Player entity

#ifndef PLAYER_H
#define PLAYER_H

#include <core.h>
#include <serializeddata.h>

namespace Core {
    class PlayerComponent;
    class ControllerComponent;
    class PhysicsComponent;
    
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
        PlayerComponent* plomp;
        ControllerComponent* controllercomponent;
        PhysicsComponent* physicscomponent;
    };
}

#endif //PLAYER_H