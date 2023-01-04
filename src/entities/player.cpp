// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// PLAYER.CPP -- Player entity.

#include <entities/player.h>

#include <framework/message.h>

#include <components/controllercomponent.h>
#include <components/playercomponent.h>
#include <components/physicscomponent.h>

namespace Core {
Player::Player(){
    plomp = PoolProxy<PlayerComponent>::New();
    name = UID("player");
    auto_load = false;
}
void Player::Load(){
    physicscomponent = PoolProxy<PhysicsComponent>::New();
    physicscomponent->SetParent(this);
    physicscomponent->SetShapeCapsule(0.5f, 0.85f);
    physicscomponent->SetMass(85.0f);
    physicscomponent->SetCollisionGroup(Physics::COLL_PLAYER);
    physicscomponent->SetStartAsleep();

    controllercomponent = PoolProxy<ControllerComponent>::New();
    controllercomponent->SetPhysicsComponent(physicscomponent);

    plomp->SetParent(this);
    plomp->SetControllerComponent(controllercomponent);
    plomp->Init();

    controllercomponent->Init();
    physicscomponent->Init();

    physicscomponent->DisableRotation();
    //physicscomponent->DisableDebugDrawing();

    isloaded = true;

}
void Player::Unload(){
    std::cout << "Player is not unloadable!!!" << std::endl;
    PoolProxy<PlayerComponent>::Delete(plomp);
}
void Player::Serialize(){
    std::cout << "Player not seriaziable" << std::endl;
}
void Player::SerializeString(std::string& str){
    std::cout << "Player not stringable" << std::endl;
}

void Player::UpdateParameters() {
    if (isloaded) {
        assert(plomp);
        plomp->MoveUpdate();
    }
}

void Player::SetParameters() {
    if (!isloaded) return;
    UpdateParameters();
    physicscomponent->SetLocation(location);
    physicscomponent->UpdateRotation(rotation);
    // actually we should add a SetUpdate() to the player component and call that
    // TODO: fix
}

void Player::MessageHandler(Message& msg){
    if(msg.type == Message::GET_IN){
        plomp->IgnoreMove();
        plomp->SetVehicle(msg.senderID);
        physicscomponent->Glue((PhysicsComponent*)msg.data);
        physicscomponent->SetCollisionGroup(0);
        //physicscomponent->EnableRotation();
    }

    if(msg.type == Message::GET_OUT){
        plomp->StopIgnoreMove();
        plomp->SetVehicle(0);
        physicscomponent->Unglue();
        physicscomponent->SetCollisionGroup(Physics::COLL_PLAYER);
        // TODO: add some sort of freeze/keep upright method to the physics component
        //physicscomponent->DisableRotation();
    }
}
}
