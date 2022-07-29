// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// ENTITYCOMPONENTS.H -- Core entity components
// Basic components for the basic entities

// TODO: refactor component implementations out of this headerino

#ifndef ENTITYCOMPONENTS_H
#define ENTITYCOMPONENTS_H

// for the rendercomp
#include <render.h>

// for the playercomp
#include <ui.h>


namespace Core {
    class PhysicsComponent;

    class RenderComponent : public EntityComponent {
    public:
        RenderComponent() : model(this), lightmap(this){}
        inline name_t GetModel(){return model->GetName();};

        inline name_t GetLightmap(){return (lightmap.GetResource() == nullptr) ? 0 : lightmap->GetName();};

        void SetModel(uint64_t name){
            model.SetResource(Render::Model::Find(name));
        };

        void SetLightmap(uint64_t name){
            lightmap.SetResource(Render::Material::Find(name));
        };

        void SetPose(Render::PoseListObject* newPose){
            pose = newPose == nullptr ? Render::poseList.begin() : newPose;
        };

        void Init(){
            if(resources_waiting == 0) Start();
        };

        void Uninit(){
            is_ready = true;
            pose = nullptr;

            for (size_t i = 0; i < 7; i++){
                if(rLsObjPtr[i] == nullptr) continue;

                Render::renderList.Remove(rLsObjPtr[i]);

                rLsObjPtr[i] = nullptr;
            }
        };

        void Start(){
            if(is_ready) return;


            uint32_t pointers = 0;
            for(uint32_t i = 0; i < 6; i++){
                if (model->IsEBOEmpty(i)) continue;
                rLsObjPtr[pointers] = Render::renderList.AddNew();
                rLsObjPtr[pointers]->FillFromModel(model.GetResource(), i);
                if(lightmap.GetResource() != nullptr) rLsObjPtr[pointers]->lightmap = lightmap->GetTexture();
                rLsObjPtr[pointers]->pose = pose;
                pointers++;

            }

            is_ready = true;

            UpdateRenderListObjs();

            return;
        }

        void UpdateRenderListObjs(){
            if (!is_ready) return;

            for(uint32_t i = 0; rLsObjPtr[i] != nullptr; i++){
                rLsObjPtr[i]->location = location;
                rLsObjPtr[i]->rotation = rotation;

                rLsObjPtr[i]->pose = pose;

                rLsObjPtr[i]->flags &= -1 ^ Render::FLAG_INTERIOR_LIGHTING;
                rLsObjPtr[i]->flags |= Render::FLAG_INTERIOR_LIGHTING * isInterior;

                rLsObjPtr[i]->lights[0] = 0;
                rLsObjPtr[i]->lights[1] = 0;
                rLsObjPtr[i]->lights[2] = 0;
                rLsObjPtr[i]->lights[3] = 0;
                Render::lightTree.FindNearest(rLsObjPtr[i]->lights, rLsObjPtr[i]->location.x, rLsObjPtr[i]->location.y, rLsObjPtr[i]->location.z);
                //CalcLights(rLsObjPtr[i]);
            }

        }

        void UpdateLocation(glm::vec3 nlocation){
            location = nlocation;
            UpdateRenderListObjs();
        };

        void UpdateRotation(glm::quat nrotation){
            rotation = nrotation;
            UpdateRenderListObjs();
        };

        void SetCellParams (bool isInteriorLight){
            isInterior = isInteriorLight;
        }

        void EventHandler(Event &event){return;}

    protected:
        ResourceProxy<Render::Model> model;
        ResourceProxy<Render::Material> lightmap;

        Render::RenderListObject* rLsObjPtr[7] = {nullptr};

        Render::PoseListObject* pose = nullptr;

        glm::vec3 location;

        glm::quat rotation;

        bool isInterior = false;
    };



    class LightComponent : public EntityComponent {
    protected:
        Render::LightListObject* lightLObj = nullptr;
        glm::vec3 location;
        glm::vec3 color;
        float distance;
        uint32_t light_tree_id = 0;
    public:
        void Update(){
            if(is_ready){
                lightLObj->location = location;
                lightLObj->color = color;
                lightLObj->distance = distance;
            } else {
                std::cout << "Can't update unloaded light." << std::endl;
            }
        };
        void UpdateLocation(float loc1, float loc2, float loc3){
            location[0] = loc1;
            location[1] = loc2;
            location[2] = loc3;
            Update();
        };
        void UpdateColor(float col1, float col2, float col3){
            color[0] = col1;
            color[1] = col2;
            color[2] = col3;
            Update();
        };
        void UpdateDistance(float dist){
            distance = dist;
            Update();
        }
        void GetColor(float (&cd)[3]){
            cd[0] = color[0];
            cd[1] = color[1];
            cd[2] = color[2];
        }
        void GetDistance(float& dist){
            dist = distance;
        }
        bool Remind(){return true;}// TODO: Yeet this
        void Init(){lightLObj = Render::lightPool.AddNew(); light_tree_id = Render::lightTree.AddLeaf(lightLObj - Render::lightPool.begin(), location.x, location.y, location.z); is_ready = true; Update();};
        void Uninit(){Render::lightPool.Remove(lightLObj); Render::lightTree.RemoveLeaf(light_tree_id); lightLObj = nullptr;};
        void Start(){}
        void EventHandler(Event &event){return;}
    };


    class ArmatureComponent : public EntityComponent {
    protected:
        Render::PoseListObject* poseobj;
        //Render::Model* model;
        ResourceProxy<Render::Model> model;
        uint64_t boneCount = 0;
        Render::Bone boneInstance[30];
        Render::Keyframe animInstance[30];
        uint64_t animPlaying[8] = {0};
        Render::Keyframe* boneKeyframes[8][30] = {nullptr};
        uint32_t boneKeyframeCount[8][30] = {0};
        uint32_t animRepeats[8];
        float animWeight[8];
        float animSpeed[8];
        float animFrame[8];
        float lastUpdate = 0.0f;
        //bool ready = false;
    public:
        ArmatureComponent() : model(this){}
        void Init(){
            poseobj = Render::poseList.AddNew();

            for (size_t i = 0; i < 30; i++) poseobj->pose[i] = glm::mat4(1.0f);

            if (resources_waiting == 0) Start();

        };
        void Uninit(){};
        void Start(){
            boneCount = model->GetArmatureSize();
            Render::Bone* armB = model->GetArmatureFirstPtr();
            for(unsigned int i = 0; i < boneCount; i++){
                boneInstance[i] = *armB;
                armB++;
            }

            is_ready = true;

            return;
        }
        
        void SetModel(uint64_t name){
            model.SetResource(Render::Model::Find(name));
        };
        uint64_t GetModel(){
            return model->GetName();
        }
        void SetBoneKeyframe (uint64_t boneName, Render::Keyframe& keyframe){
            for (size_t i = 0; i < boneCount; i++){
                if (boneInstance[i].name == boneName){
                    animInstance[i] = keyframe;
                    return;
                }
            }
        }
        Render::PoseListObject* GetPosePtr(){return poseobj;};
        void PlayAnimation(uint64_t animationName, uint32_t repeats, float weight, float speed){
            uint64_t slot = 0;
            for (int i = 0; i < 8; i++){
                if(animPlaying[i] == animationName) return;     //do nothing if animation is already playing
            }

            for (; slot < 8; slot++){
                if(animPlaying[slot] == 0) break;               //find empty slot
            }
            if(slot == 8) return;                               //do nothing if all slots are used

            animPlaying[slot] = animationName;
            animRepeats[slot] = repeats;
            animWeight[slot] = weight;
            animSpeed[slot] = speed;
            animFrame[slot] = 0;

            // TODO: add code to empty keyframe pointers in the slot before copying in new ones

            Render::NameCount* nc = Render::Animation::Find(animationName);//Render::animationList.FindAnimationPtr(animationName);
            Render::Keyframe* kf;

            if(nc == nullptr){
                std::cout << "Animation " << animationName << " not found!" << std::endl;
                return;
            }

            uint64_t animBoneCount = nc->second;
            nc++;
            for(uint64_t i = 0; i < animBoneCount; i++){
                uint64_t boneslot = -1llu;

                for(uint64_t j = 0; j < boneCount; j++){
                    if(boneInstance[j].name == nc->first){
                        boneslot = j;
                        break;
                    }
                }

                if(boneslot != -1llu){
                    boneKeyframes[slot][boneslot] = (Render::Keyframe*)(nc + 1);
                    boneKeyframeCount[slot][boneslot] = nc->second;
                }

                uint64_t kfcount = nc->second;
                nc++;
                kf = (Render::Keyframe*)nc;
                kf += kfcount;
                nc = (Render::NameCount*)kf;
            }



        };
        void StopAnimation(uint64_t animationName){
            for (int i = 0; i < 8; i++){
                if(animPlaying[i] == animationName){
                    animPlaying[i] = 0;
                    return;
                }
            }
        };
        void Update(){
            Render::Keyframe anim[30];                          //this is where we store the calculated animation for this frame
            for (uint64_t i = 0; i < boneCount; i++)    //copy in the start animation
                anim[i] = animInstance[i];

            // * do the animations *

            for (uint64_t i = 0; i < 8; i++){
                if(animPlaying[i] == 0) continue;

                animFrame[i] += (FRAME_TIME - lastUpdate) * 24 * animSpeed[i];

                for (uint64_t k = 0; k < boneCount; k++){
                    if(boneKeyframes[i][k] != nullptr){

                        uint64_t kframe = -1llu;
                        for(uint64_t j = 0; j < boneKeyframeCount[i][k]; j++){
                            if((boneKeyframes[i][k] + j)->frame > animFrame[i]){
                                kframe = j;
                                break;
                            }
                        }

                        if(kframe == -1llu){
                            if((boneKeyframes[i][k] + boneKeyframeCount[i][k] - 1)->frame < animFrame[i]){
                                animRepeats[i]--;
                                //std::cout << "repeated " << animRepeats[i] << std::endl;
                                if(animRepeats[i] == 0){
                                    animPlaying[i] = 0;
                                    k = -1;
                                    continue;
                                }
                                animFrame[i] = 0.0f;
                            }
                            kframe = 0;
                        }

                        Render::Keyframe* kptr = boneKeyframes[i][k] + kframe;
                        Render::Keyframe* kptr2;
                        float mixw;

                        if(kframe == 0){
                            kptr2 = kptr;
                            mixw = 1.0f;
                        } else {
                            kptr2 = kptr--;
                            mixw = (animFrame[i] - kptr2->frame) / (kptr->frame - kptr2->frame);
                        }



                        anim[k].location += glm::mix(kptr2->location, kptr->location, mixw);
                        anim[k].rotation *= glm::mix(kptr2->rotation, kptr->rotation, mixw);
                        anim[k].scale *= glm::mix(kptr2->scale, kptr->scale, mixw);

                    }


                }

            }

            lastUpdate = FRAME_TIME;


            //animations done, now do skinning

            for(uint64_t i = 0; i < boneCount; i++){

                poseobj->pose[i] = glm::mat4(1.0f);

                glm::mat4 modelToBone = glm::translate(glm::mat4(1.0f), -boneInstance[i].head);

                glm::mat4 boneAnim = glm::mat4(1.0f);
                boneAnim = glm::toMat4(anim[i].rotation) * boneAnim;
                boneAnim = glm::translate(glm::mat4(1.0f), anim[i].location) * boneAnim;

                glm::mat4 boneToModel = glm::translate(glm::mat4(1.0f), boneInstance[i].head);



                if(boneInstance[i].parentIndex == (uint32_t)-1){
                    poseobj->pose[i] = boneToModel * boneAnim * modelToBone;
                } else {
                    poseobj->pose[i] = poseobj->pose[boneInstance[i].parentIndex] * boneToModel * boneAnim * modelToBone;
                }



            }

        };
        void EventHandler(Event &event){return;}
    };






    /// Player and NPC controller.
    class ControllerComponent : public EntityComponent {
    public:
        enum Action : uint32_t {
            ACTION_IDLE,
            ACTION_MOVE_FORWARD,
            ACTION_MOVE_BACKWARD,
            ACTION_MOVE_LEFT,
            ACTION_MOVE_RIGHT,
            ACTION_TURN_FORWARD,
            ACTION_TURN_BACKWARD,
            ACTION_TURN_LEFT,
            ACTION_TURN_RIGHT,
            ACTION_JUMP,
            ACTION_CROUCH,
            ACTION_FORWARD_JUMP,
            ACTION_ACTIVATE,
            ACTION_MOVE_FORWARD_VERY_FAST,
            ACTION_MONGUS_WALK,
            ACTION_DAB,
            ACTION_STARE,
            ACTION_LOOK_BUSY
        };
        void Init(){};
        void Uninit(){};
        void Start(){}
        bool Remind(){return true;};
        void EventHandler(Event &event){};
        void Move(glm::vec3& direction);
        void GetLocation(glm::vec3& location);
        void Act(Action action);

        bool IsInAir();

        void ActivateInFront();

        void SetAnimation(Action action, uint64_t animationName){animations[action] = animationName;}
        void SetPhysicsComponent(PhysicsComponent* comp){physcomp = comp;};
        void SetArmatureComponent(ArmatureComponent* comp){armcomp = comp;};
        void SetDirection(glm::vec3& dir){direction = dir;};
    private:
        inline void Play (Action action){
            if (animations[action] == 0 || animations[action] == animplaying || armcomp == nullptr){
                return;
            } else {
                armcomp->StopAnimation(animplaying);
                armcomp->PlayAnimation(animations[action], 100, 1.0, 1.0);
                animplaying = animations[action];
            }
        }
        inline void PlayOnce (Action action){
            if (animations[action] == 0 || armcomp == nullptr){
                return;
            } else {
                armcomp->StopAnimation(animplaying);
                armcomp->PlayAnimation(animations[action], 1, 1.0, 1.0);
                animplaying = animations[action];
            }
        }
        PhysicsComponent* physcomp = nullptr;
        ArmatureComponent* armcomp = nullptr;
        glm::vec3 direction = glm::vec3(0.0f);
        uint64_t animations[30] = {0};
        uint64_t animplaying = 0;
    };



    /// Takes keyboard inputs and feeds them to a ControllerComponent.
    class PlayerComponent : public EntityComponent {
    public:
        /*bool Remind(){};*/
        void EventHandler(Event &event){
            using namespace Core::UI;
            using enum Core::ControllerComponent::Action;

            if(is_move && event.type == Event::KEYPRESS){
                glm::vec3 vecdir = direction * forwarddir;
                controller->SetDirection(vecdir);


                if(event.subtype & KEY_FORWARD) controller->Act(ACTION_MOVE_FORWARD);
                if(event.subtype & KEY_BACKWARD) controller->Act(ACTION_MOVE_BACKWARD);
                if(event.subtype & KEY_LEFT) controller->Act(ACTION_MOVE_LEFT);
                if(event.subtype & KEY_RIGHT) controller->Act(ACTION_MOVE_RIGHT);
                if(event.subtype & KEY_JUMP && !(event.subtype & KEY_CROUCH)) controller->Act(ACTION_JUMP);
                if(event.subtype & KEY_CROUCH && event.subtype & KEY_JUMP) controller->Act(ACTION_FORWARD_JUMP);


            } else if (event.type == Event::KEYDOWN) {
                if(event.subtype & KEY_ACTIVATE) controller->ActivateInFront();

            } else if (event.type == Event::CURSORPOS){
                direction = glm::quat(glm::vec3(-glm::radians(Render::CAMERA_PITCH), -glm::radians(Render::CAMERA_YAW), 0.0f));

            } else if (!is_move && event.type == Event::KEYPRESS && (event.subtype & KEY_JUMP && event.subtype & KEY_CROUCH)){
                Message msg;
                msg.type = Message::ACTIVATE;
                msg.senderID = parent->GetID();
                msg.receiverID = vehicle;
                Message::Send(msg);
            }


        };
        void Init(){
            listener = Event::AddListener(Event::KEYPRESS);
            listener->ent = this;
            keydown = Event::AddListener(Event::KEYDOWN);
            keydown->ent = this;
            mouselistener = Event::AddListener(Event::CURSORPOS);
            mouselistener->ent = this;
        }
        void Uninit(){
            Event::RemoveListener(Event::KEYPRESS, listener);
            listener = nullptr;
            Event::RemoveListener(Event::KEYDOWN, keydown);
            keydown = nullptr;
            Event::RemoveListener(Event::CURSORPOS, mouselistener);
            mouselistener = nullptr;
        }
        void SetParent(Entity* ent){
            parent = ent;
        }
        void SetControllerComponent (ControllerComponent* comp){controller = comp;};
        void SetVehicle (uint64_t vehicleID){vehicle = vehicleID;};
        void IgnoreMove(){
            is_move = false;
        }
        void StopIgnoreMove(){
            is_move = true;
        }
        void Start(){}
    private:
        Entity* parent = nullptr;
        Event::Listener* keydown = nullptr;
        Event::Listener* listener = nullptr;
        Event::Listener* mouselistener = nullptr;
        ControllerComponent* controller = nullptr;

        uint64_t vehicle = 0;

        const glm::vec3 forwarddir = glm::vec3(0.0f, 0.0f, -1.0f);
        const glm::vec3 sidedir = glm::vec3(1.0f, 0.0f, 0.0f);
        const glm::vec3 updir = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::quat direction;

        bool is_move = true;
    };
    
    
    #ifdef BEGONISER
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

#endif // ENTITYCOMPONENTS_H
