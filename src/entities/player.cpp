// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// PLAYER.CPP -- Player entity.

#include <core.h>
#include <entities.h>

#include <physics.h>

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
    physicscomponent->DisableDebugDrawing();

    isloaded = true;

}
void Player::Unload(){
    std::cout << "Player is not unloadable!!!" << std::endl;
}
void Player::Serialize(){
    std::cout << "Player not seriaziable" << std::endl;
}
void Player::SerializeString(std::string& str){
    std::cout << "Player not stringable" << std::endl;
}

void Player::UpdateParameters() {
    //next_cell = cell->FindTransition(location);
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