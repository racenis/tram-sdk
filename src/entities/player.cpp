// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <entities/player.h>

#include <framework/message.h>

#include <components/controller.h>
#include <components/player.h>
#include <components/physics.h>
#include <components/trigger.h>

/**
 * @class tram::Player entities/player.h <entities/player.h>
 * 
 * ControllerComponent with a PlayerComponent attached to it.
 * Can be used to control a player for a video game, or could be used to simply
 * be able to walk around the virtual world that you have created.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/entities/player.html
 */

namespace tram {

// we should make these configurable in the future
const float collision_width = 0.35f;
const float collision_height = 1.85f;
const float collision_height_crouch = 0.5f;

const uint32_t collision_group = Physics::COLL_PLAYER;
    
/// Creates a new player.
Player::Player() : Player("player") {
    
}

/// Creates a player with a custom name.
/// @param name Name of the player.
Player::Player(name_t name) : Entity(name) {
    SetAutoLoad(false);
}

name_t Player::GetType() {
    return "player";
}

void Player::Load(){
    plomp.make();
    controllercomponent.make();
    physics_body.make();
    
    controllercomponent->SetParent(this);
    controllercomponent->SetCollisionGroup(collision_group);
    controllercomponent->SetCollisionSize(collision_width, collision_height, collision_height_crouch);

    plomp->SetParent(this);
    plomp->SetControllerComponent(controllercomponent);
    
    physics_body->SetParent(this);
    physics_body->SetShape(Physics::CollisionShape::Capsule(collision_width, collision_height/2.0f));
    physics_body->SetCollisionGroup(collision_group);
    physics_body->SetKinematic(true);
    physics_body->DisableDeactivation();
    
    plomp->Init();
    controllercomponent->Init();
    physics_body->Init();

    SetFlag(LOADED, true);
}
void Player::Unload() {
    plomp.clear();
    controllercomponent.clear();
    
    SetFlag(LOADED, false);
}

void Player::Serialize() {
    std::cout << "Player not seriaziable" << std::endl;
}

void Player::SerializeString(std::string& str) {
    std::cout << "Player not stringable" << std::endl;
}

void Player::UpdateParameters() {
    
}

void Player::SetParameters() {
    if (!IsLoaded()) return;
    UpdateParameters();
}

void Player::MessageHandler(Message& msg) {
    if (msg.type == Message::PING) {
        Log ("Player was pinged with {}!", (long long) msg.data);
    }
    
    if (msg.type == Message::SET_FLAG_ON && msg.data_value) {
        const name_t flag = *msg.data_value;
        
        if (flag == "noclip") {
            Log("Noclip turned on.");
            plomp->SetNoclip(true);
            
        } else if (flag == "mouselook") {
            Log("Mouselook turned on.");
            plomp->SetKeyboardLook(false);
            
        } else {
            Log ("Player does not have flag {}!", flag);
        }
    }
    
    if (msg.type == Message::SET_FLAG_OFF && msg.data_value) {
        const name_t flag = *msg.data_value;
        
        if (flag == "noclip") {
            Log("Noclip turned off.");
            plomp->SetNoclip(false);
            
        } else if (flag == "mouselook") {
            Log("Mouselook turned of.");
            plomp->SetKeyboardLook(true);
            
        } else {
            Log ("Player does not have flag {}!", flag);
        }
    }
}

}
