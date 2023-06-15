// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <entities/player.h>

#include <framework/message.h>

#include <components/controllercomponent.h>
#include <components/playercomponent.h>
#include <components/physicscomponent.h>
#include <components/triggercomponent.h>

namespace tram {

Player::Player() : Entity("player") {
    auto_load = false;
}

void Player::Load(){
    plomp.make();
    controllercomponent.make();
    
    controllercomponent->SetParent(this);

    plomp->SetParent(this);
    plomp->SetControllerComponent(controllercomponent);
    
    plomp->Init();
    controllercomponent->Init();

    is_loaded = true;

}
void Player::Unload() {
    std::cout << "Player is not unloadable!!!" << std::endl;
}

void Player::Serialize() {
    std::cout << "Player not seriaziable" << std::endl;
}

void Player::SerializeString(std::string& str) {
    std::cout << "Player not stringable" << std::endl;
}

void Player::UpdateParameters() {
    if (is_loaded) {
        assert(plomp);
        plomp->MoveUpdate();
    }
}

void Player::SetParameters() {
    if (!is_loaded) return;
    UpdateParameters();
}

void Player::MessageHandler(Message& msg) {
    if (msg.type == Message::PING) {
        Log ("Player was pinged with {}!", (long long) msg.data);
    }
}

}
