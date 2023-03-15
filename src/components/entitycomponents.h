// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

// TODO: refactor component implementations out of this headerino

#ifndef COMPONENTS_ENTITYCOMPONENTS_H
#define COMPONENTS_ENTITYCOMPONENTS_H

// for the rendercomp
#include <render.h>

// for the playercomp
#include <ui.h>


namespace tram {
    class PhysicsComponent;



    



    
    
    
    #ifdef BEGONISER
    // this AI component implementation is complete shit
    // i think it can be deleted
    class AIComponent : public EntityComponent {
    public:
        enum Flag : uint64_t {
            AI_IDLE = 0,
            AI_FOLLOW = 1,
            AI_RUN_IN_CIRCLES = 2
        };
        virtual void Init(){

        };
        virtual void Uninit(){

        };
        virtual void EventHandler(Event &event){

        };
        void Update(){
            /*if(flags & AI_FOLLOW){
                glm::vec3 self_loc;
                glm::vec3 self_dir;
                glm::vec3 target_loc;

                Entity* target_ptr = FindEntityPtr(target_name);
                if (!target_ptr) return;

                target_ptr->GetLocation(target_loc);
                controller->GetLocation(self_loc);
                controller->

                float horizontal = glm::dot()

            }*/
            updates++;

            if(flags == AI_IDLE){
                // play idle animation
                if(!(updates%300)){
                    controller->Act(ACTION_IDLE);
                }
                return;
            }



            if(flags & AI_RUN_IN_CIRCLES){
                controller->Act(ACTION_MOVE_FORWARD_VERY_FAST);
            }

        };
        void SetController(ControllerComponent* contr){
            controller = contr;
        };
        void SetFlag (aiflagtype flag, bool yesno){
            if(yesno){
                flags = flags | flag;
            } else {
                flags = flags & ~flag;
            }
        };
        void SetTarget(uint64_t name){
            target_name = name;
        };
    protected:
        ControllerComponent* controller = nullptr;
        uint64_t flags = AI_IDLE;
        uint64_t target_name = 0;
        size_t updates = 0;
    };

    extern Pool<PhysicsComponent> physicsCompPool;

    // the old path node system is yeeted, so none of this works anymore
    
    // this probably will need to be rewritten completely
    class TramComponent : public EntityComponent {
    public:
        void EventHandler(Event &event){
            if(event.type == EVENT_KEYPRESS){
                if(event.subtype & KEY_FORWARD){
                    bogey_front_physcomp->PushLocal(glm::vec3(0.0f, 0.0f, -21.0f));
                } else if(event.subtype & KEY_BACKWARD){
                    bogey_front_physcomp->PushLocal(glm::vec3(0.0f, 0.0f, 21.0f));
                }

                if(event.subtype & KEY_LEFT){
                    bogey_back_physcomp->SetPathPreference('l');
                    bogey_front_physcomp->SetPathPreference('l');
                } else if(event.subtype & KEY_RIGHT){
                    bogey_back_physcomp->SetPathPreference('r');
                    bogey_front_physcomp->SetPathPreference('r');
                } else {
                    bogey_back_physcomp->SetPathPreference('s');
                    bogey_front_physcomp->SetPathPreference('s');
                }

                glm::vec3 bbb;
                bogey_back_physcomp->GetParent()->GetLocation(bbb);
                /*if(bbb.z > 50.0f) floppa_quest_trigger();*/
            }


        };
        bool SetPathConstraints(){
            if(bogey_front_physcomp->IsReady() && bogey_back_physcomp->IsReady()){
                glm::vec3 hinge_axis(0.0f, 1.0f, 0.0f);
                bogey_front_physcomp->SetHingeConstraint(body_physcomp, hinge_axis, glm::vec3(0.0f, 0.0f, 0.0f), bogey_front_offset);
                bogey_back_physcomp->SetHingeConstraint(body_physcomp, hinge_axis, glm::vec3(0.0f, 0.0f, 0.0f), bogey_back_offset);

                bogey_front_physcomp->SetPath();
                bogey_back_physcomp->SetPath();
                return true;
            } else {
                return false;
            }
        }
        bool FindOffsets(){
            if(model->GetStatus() == Resource::READY){
                Render::Bone* bone = model->GetArmatureFirstPtr();
                for(size_t i = 0; i < model->GetArmatureSize(); i++){
                    if(bone->name == UID("BogeyFront")){
                        bogey_front_offset = bone->head;
                    } else if(bone->name == UID("BogeyBack")){
                        bogey_back_offset = bone->head;
                    } else if(bone->name == UID("Pantograph")){
                        pantograph_offset = bone->head;
                    }

                    bone++;
                }

                bogey_front_physcomp = physicsCompPool.AddNew();
                bogey_front_physcomp->SetParent(parent);
                bogey_front_physcomp->SetModel(bogey_collmodel);
                bogey_front_physcomp->SetMass(40.0f);
                bogey_front_physcomp->SetOffset(bogey_front_offset);
                bogey_front_physcomp->SetBone(UID("BogeyFront"), armature);
                bogey_front_physcomp->SetCollisionMask(0);

                bogey_back_physcomp = physicsCompPool.AddNew();
                bogey_back_physcomp->SetParent(parent);
                bogey_back_physcomp->SetModel(bogey_collmodel);
                bogey_back_physcomp->SetMass(40.0f);
                bogey_back_physcomp->SetOffset(bogey_back_offset);
                bogey_back_physcomp->SetBone(UID("BogeyBack"), armature);
                bogey_back_physcomp->SetCollisionMask(0);

                bogey_front_physcomp->Init();
                bogey_back_physcomp->Init();

                if(!SetPathConstraints()){
                    AddToReminderQueue(this, (bool (EntityComponent::*)())SetPathConstraints);
                }


                return true;
            } else {
                return false;
            }
        }
        void Init(){
            model->AddRef();

            if(!FindOffsets())
                AddToReminderQueue(this, (bool (EntityComponent::*)())FindOffsets);


        }
        void Uninit(){
            // TODO: write the uninit code
            bogey_back_physcomp->Uninit();
            bogey_front_physcomp->Uninit();
        }
        void SetModel(uint64_t name){
            model = FindModelPtr(name);
        };
        void SetCollisionModels(uint64_t bogey, uint64_t pantograph){
            bogey_collmodel = bogey;
            pantograph_collmodel = pantograph;
        }
        void SetParent(Entity* ent){
            parent = ent;
        }
        void SetPhysicsComponent(PhysicsComponent* comp){
            body_physcomp = comp;
        }
        void SetArmatureComponent(ArmatureComponent* comp){
            armature = comp;
        }
        void Activate(){
            std::cout << "ACTIVATED!!" << std::endl;

            if(is_controlled){
                Message msg;
                msg.senderID = parent->GetID();
                msg.receiverID = PLAYER_ID;
                msg.type = MESSAGE_GET_OUT;
                SendDIRST(msg);

                RemoveEventListener(EVENT_KEYPRESS, keypress);
                keypress = nullptr;


                is_controlled = false;
            } else {
                Message msg;
                msg.senderID = parent->GetID();
                msg.receiverID = PLAYER_ID;
                msg.type = MESSAGE_GET_IN;
                msg.data = body_physcomp;
                SendDIRST(msg);

                keypress = AddEventListener(EVENT_KEYPRESS);
                keypress->ent = this;


                is_controlled = true;
            }


        }
    private:
        Entity* parent = nullptr;
        EventListener* keypress = nullptr;

        ArmatureComponent* armature = nullptr;
        Render::Model* model = nullptr;

        PhysicsComponent* body_physcomp;    //don't delete this one, the entity code will delete it

        PhysicsComponent* bogey_front_physcomp;
        PhysicsComponent* bogey_back_physcomp;

        uint64_t bogey_collmodel;
        uint64_t pantograph_collmodel;


        glm::vec3 bogey_front_offset;
        glm::vec3 bogey_back_offset;
        glm::vec3 pantograph_offset;

        bool is_controlled = false;

    };

    #endif

    void ProcessReminderQueue(); // what does this do ???

}

#endif // COMPONENTS_ENTITYCOMPONENTS_H
