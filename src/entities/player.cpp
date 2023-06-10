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
    plomp = PoolProxy<PlayerComponent>::New();
    auto_load = false;
}

void Player::Load(){
    physicscomponent = PoolProxy<PhysicsComponent>::New();
    physicscomponent->SetParent(this);
    physicscomponent->SetShape(Physics::CollisionShape::Capsule(0.35f, 1.15f));
    physicscomponent->SetMass(85.0f);
    physicscomponent->SetCollisionGroup(Physics::COLL_PLAYER);
    //physicscomponent->SetSleep(true);
    physicscomponent->SetKinematic(true);
    
    triggercomponent = PoolProxy<TriggerComponent>::New();
    //triggercomponent->SetCollisionGroup(Physics::COLL_WORLDOBJ);
    //triggercomponent->SetCollisionMask(Physics::COLL_WORLDOBJ);
    triggercomponent->SetCollisionMask(-1 ^ Physics::COLL_PLAYER);
    triggercomponent->SetShape(Physics::CollisionShape::Cylinder(0.35f, (1.85f/2.0f)-0.35f));
    triggercomponent->SetLocation(location);
    

    controllercomponent = PoolProxy<ControllerComponent>::New();
    controllercomponent->SetParent(this);
    controllercomponent->SetPhysicsComponent(physicscomponent);
    controllercomponent->SetTriggerComponent(triggercomponent);

    plomp->SetParent(this);
    plomp->SetControllerComponent(controllercomponent);
    plomp->Init();

    controllercomponent->Init();
    physicscomponent->Init();
    triggercomponent->Start();
    
    physicscomponent->SetActivation(true);
    //physicscomponent->DisableRotation();
    //physicscomponent->DisableDebugDrawing();

    is_loaded = true;

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
