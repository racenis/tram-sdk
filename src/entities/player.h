// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef ENTITIES_PLAYER_H
#define ENTITIES_PLAYER_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serializeddata.h>

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

#endif // ENTITIES_PLAYER_H