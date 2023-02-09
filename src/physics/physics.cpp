// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <physics/physics.h>
#include <physics/bullet/bullet.h>
#include <physics/bullet/actions.h>


#include <components/triggercomponent.h>


#include <framework/system.h>

namespace Core::Physics {
    /// Option for drawing physics debug lines.
    /// Set to true if you want to see physics debug lines. False if not.
    bool DRAW_PHYSICS_DEBUG = false;
    
    /// Initializes the physics system.
    void Init() {
        assert(!System::IsInitialized(System::SYSTEM_PHYSICS));
        
        InitBullet();
        
        System::SetInitialized(System::SYSTEM_PHYSICS, true);
    }
    
    /// Updates the physics system.
    /// Should only be called once in the update cycle.
    void Update() {
        StepPhysics();
        
        if (DRAW_PHYSICS_DEBUG) DrawDebug();
    }
    
    /// Performs a raycast.
    /// Performs a raycast from from to to.
    /// @return Pointer to the closest PhysicsComponent. If the ray doesn't hit
    ///         hit anything, then a nullptr.
    PhysicsComponent* Raycast(const glm::vec3& from, const glm::vec3& to){
        btVector3 bto, bfrom;

        bto.setValue(to.x, to.y, to.z);
        bfrom.setValue(from.x, from.y, from.z);

        btCollisionWorld::ClosestRayResultCallback callback(bfrom, bto);

        dynamicsWorld->rayTest(bfrom, bto, callback);

        if (callback.hasHit())
            return (PhysicsComponent*)callback.m_collisionObject->getUserPointer();
        else
            return nullptr;
    }




}


#ifdef BENIGONIS
namespace Core {
    // TODO: OKEY, the next thing will be needed to be brapped out of here to buscomponent.cpp
    void BusComponent::EventHandler(Event &event){
        if(event.type == Event::KEYDOWN){
            if(event.subtype & UI::KEY_FORWARD){
                vehicle->applyEngineForce(5000.0f, 2);
                vehicle->applyEngineForce(5000.0f, 3);
            } else if(event.subtype & UI::KEY_BACKWARD){
                vehicle->applyEngineForce(-3000.0f, 2);
                vehicle->applyEngineForce(-3000.0f, 3);
            }

            if(event.subtype & UI::KEY_LEFT){
                vehicle->setSteeringValue(0.5f, 0);
                //vehicle->setSteeringValue(-0.75f, 1);
            } else if(event.subtype & UI::KEY_RIGHT){
                //vehicle->setSteeringValue(0.75f, 0);
                vehicle->setSteeringValue(-0.5f, 1);
            }

            if(event.subtype & UI::KEY_JUMP){
                vehicle->setBrake(1000.0f, 2);
                vehicle->setBrake(1000.0f, 3);
            }
        }


        if(event.type == Event::KEYUP){
            if(event.subtype & UI::KEY_FORWARD || event.subtype & UI::KEY_BACKWARD){
                vehicle->applyEngineForce(0.0f, 2);
                vehicle->applyEngineForce(0.0f, 3);

                vehicle->setBrake(10.0f, 2);
                vehicle->setBrake(10.0f, 3);
            }

            if(event.subtype & UI::KEY_LEFT || event.subtype & UI::KEY_RIGHT){
                vehicle->setSteeringValue(0.0f, 0);
                vehicle->setSteeringValue(0.0f, 1);
            }

            if(event.subtype & UI::KEY_JUMP){
                vehicle->setBrake(10.0f, 2);
                vehicle->setBrake(10.0f, 3);
            }
        }


    };
    bool BusComponent::Start(){
        if(true/*model->GetStatus() == Resource::READY && body_physcomp->IsReady()*/){
            Render::Bone* bone = model->GetArmatureFirstPtr();
            for(size_t i = 0; i < model->GetArmatureSize(); i++){
                if(bone->name == UID("WheelFL")){
                    offset_wheel_fl = bone->head;
                } else if(bone->name == UID("WheelFR")){
                    offset_wheel_fr = bone->head;
                } else if(bone->name == UID("WheelBL")){
                    offset_wheel_bl = bone->head;
                } else if(bone->name == UID("WheelBR")){
                    offset_wheel_br = bone->head;
                }

                bone++;
            }

            //TODO: check if this is safe
            btRaycastVehicle::btVehicleTuning vtuna;

            vehicle = new btRaycastVehicle(vtuna, body_physcomp->rigidBody, vehicleRaycaster);

            dynamicsWorld->addVehicle(vehicle);

            btVector3 wheel_dir(0.0f, -1.0f, 0.0f);
            btVector3 axle(1.0f, 0.0f, 0.0f);

            float v_off = 1.0f; // stupid hack

            float suspension = 0.4f;

            float wheel_radius = 0.3f;


            vehicle->addWheel(btVector3(offset_wheel_fl.x, offset_wheel_fl.y + v_off, offset_wheel_fl.z), wheel_dir, axle, suspension, wheel_radius, vtuna, true);

            vehicle->addWheel(btVector3(offset_wheel_fr.x, offset_wheel_fr.y + v_off, offset_wheel_fr.z), wheel_dir, axle, suspension, wheel_radius, vtuna, true);

            vehicle->addWheel(btVector3(offset_wheel_bl.x, offset_wheel_bl.y + v_off, offset_wheel_bl.z), wheel_dir, axle, suspension, wheel_radius, vtuna, false);

            vehicle->addWheel(btVector3(offset_wheel_br.x, offset_wheel_br.y + v_off, offset_wheel_br.z), wheel_dir, axle, suspension, wheel_radius, vtuna, false);


            body_physcomp->SetLinearFactor(1.0f);

            //std::cout << "action: " << body_physcomp->action << std::endl;

            return true;
        } else {
            return false;
        }
    }
    void BusComponent::Init(){
        model->AddRef();

        if(!FindOffsets())
            AddToReminderQueue(this, (bool (EntityComponent::*)())FindOffsets);

        if(resources_waiting == 0) Start();
    }
    void BusComponent::Uninit(){
        // TODO: better cleanup
        if (vehicle) dynamicsWorld->removeVehicle(vehicle);
    }

void BusComponent::Activate(){
        std::cout << "ACTIVATED!!" << std::endl;

        if(is_controlled){
            Message msg;
            msg.senderID = parent->GetID();
            msg.receiverID = PLAYER_ID;
            msg.type = MESSAGE_GET_OUT;
            SendDIRST(msg);

            body_physcomp->EnableActivation();

            RemoveEventListener(EVENT_KEYPRESS, keypress);
            keypress = nullptr;
            RemoveEventListener(EVENT_KEYDOWN, keydown);
            keydown = nullptr;
            RemoveEventListener(EVENT_KEYUP, keyup);
            keyup = nullptr;




            is_controlled = false;
        } else {
            Message msg;
            msg.senderID = parent->GetID();
            msg.receiverID = PLAYER_ID;
            msg.type = MESSAGE_GET_IN;
            msg.data = body_physcomp;
            SendDIRST(msg);

            body_physcomp->DisableActivation();

            keypress = AddEventListener(EVENT_KEYPRESS);
            keypress->ent = this;
            keydown = AddEventListener(EVENT_KEYDOWN);
            keydown->ent = this;
            keyup = AddEventListener(EVENT_KEYUP);
            keyup->ent = this;


            is_controlled = true;
        }
    }
}
#endif // BENIGONIS

