// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// ARMATURE.CPP -- animation code.
// Implementation of animation loading, armature component.

#include <armature.h>
#include <components/armaturecomponent.h>

#include <fstream>

namespace Core::Render {
    std::unordered_map<name_t, NameCount*> Animation::animationlist;
    Pool<PoseListObject> poseList("pose list", 100, true);

    void Animation::LoadFromDisk(){
        using namespace Render;
        std::ifstream file;

        std::string filename = "data/animations/";
        filename += ReverseUID(name);
        filename += ".anim";

        file.open(filename);

        if(!file.is_open()){std::cout << "Can't open " << filename << "!" << std::endl; return;}

        std::cout << "Loading animation: " << filename << std::endl;

        std::string noot;
        uint64_t ac;
        uint64_t bc;
        uint64_t kc;
        uint64_t a_name;
        NameCount* nameptr = (NameCount*)animationpool.begin();//animationList.last;
        Keyframe* kframe;


        file >> noot;       //not actually used for anything
        file >> ac;         //count of animation entries in the file

        for (uint64_t i = 0; i < ac; i++){
            file >> noot;       //name of animation as string
            file >> bc;         //count of bones used by animation
            a_name = UID(noot);   //uint64 hash of name string

            nameptr->first = a_name;
            nameptr->second = bc;
            animationlist[a_name] = nameptr;
            //animationList.AddAnimationPtr(a_name, nameptr);   //important (NOTT XDDD)

            nameptr++;


            for (uint64_t j = 0; j < bc; j++){
                file >> noot;       //name of bone as string
                file >> kc;         //count of keyframes for the bone
                a_name = UID(noot);   //uint64 hash of bone name

                nameptr->first = a_name;
                nameptr->second = kc;
                nameptr++;


                for (uint64_t k = 0; k < kc; k++){
                    kframe = (Keyframe*)nameptr;
                    file >> kframe->frame;
                    //float r[4];
                    file >> kframe->location.x;
                    file >> kframe->location.y;
                    file >> kframe->location.z;
                    //file >> r[0];
                    //file >> r[1];
                    //file >> r[2];
                    //file >> r[3];
                    //kframe->rotation.x = r[1];
                    //kframe->rotation.y = r[3];
                    //kframe->rotation.z = -r[2];
                    //kframe->rotation.w = r[0];
                    file >> kframe->rotation.x;
                    file >> kframe->rotation.y;
                    file >> kframe->rotation.z;
                    file >> kframe->rotation.w;
                    file >> kframe->scale.x;
                    file >> kframe->scale.y;
                    file >> kframe->scale.z;
                    kframe++;
                    nameptr = (NameCount*)kframe;
                }


            }


        }

        //animationList.last = nameptr;
        //std::cout << "animation length: " << (int)(nameptr - animationList.first) << std::endl;
        //std::cout << "animlist length: " << (int)(animationList.last - animationList.first) << std::endl;

        file.close();
    }

    NameCount* Animation::Find(name_t name){
        std::unordered_map<uint64_t, NameCount*>::iterator ff = animationlist.find(name);

        if(ff == animationlist.end()){
            std::cout << "Can't find animation " << ReverseUID(name) << "!" << std::endl;
            return nullptr;
        } else {
            return ff->second;
        }
    }    
    
    void UpdateArmatures(){
        ArmatureComponent* armobj = PoolProxy<ArmatureComponent>::GetPool().GetFirst();
        ArmatureComponent* armlast = PoolProxy<ArmatureComponent>::GetPool().GetLast();
        for(;armobj < armlast; armobj++){
            if(*((uint64_t*)armobj) == 0)
                continue;
            else
                armobj->Update();
        }
    }
}

namespace Core {
    template <> Pool<ArmatureComponent> PoolProxy<ArmatureComponent>::pool("armature component pool", 50, false);
    
    void ArmatureComponent::Init(){
        poseobj = Render::poseList.AddNew();

        for (size_t i = 0; i < 30; i++) poseobj->pose[i] = glm::mat4(1.0f);

        if (resources_waiting == 0) Start();

    };
        
    void ArmatureComponent::Uninit(){};
    
    void ArmatureComponent::Start(){
        boneCount = model->GetArmatureSize();
        Render::Bone* armB = model->GetArmatureFirstPtr();
        for(unsigned int i = 0; i < boneCount; i++){
            boneInstance[i] = *armB;
            armB++;
        }

        is_ready = true;

        return;
    }
    
    void ArmatureComponent::SetBoneKeyframe (uint64_t boneName, Render::Keyframe& keyframe){
        for (size_t i = 0; i < boneCount; i++){
            if (boneInstance[i].name == boneName){
                animInstance[i] = keyframe;
                return;
            }
        }
    }
        
    void ArmatureComponent::PlayAnimation(uint64_t animationName, uint32_t repeats, float weight, float speed){
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
    }
    
    void ArmatureComponent::StopAnimation(uint64_t animationName){
        for (int i = 0; i < 8; i++){
            if(animPlaying[i] == animationName){
                animPlaying[i] = 0;
                return;
            }
        }
    }
    
    void ArmatureComponent::Update(){
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
            
            auto& head = boneInstance[i].head;
            auto& tail = boneInstance[i].tail;
            auto& roll = boneInstance[i].roll;
            
            glm::vec3 tail_dir = glm::normalize(tail - head);
            
            auto rot = glm::rotation(tail_dir, glm::vec3(0.0f, 0.0f, -1.0f));
            modelToBone = glm::toMat4(rot) * modelToBone;
            auto rolltransf = glm::rotate(glm::mat4(1.0f), -roll, glm::vec3(0.0f, 0.0f, -1.0f));
            modelToBone = rolltransf * modelToBone;


            glm::mat4 boneAnim = glm::mat4(1.0f);
            boneAnim = glm::toMat4(anim[i].rotation) * boneAnim;
            boneAnim = glm::translate(glm::mat4(1.0f), anim[i].location) * boneAnim;

            glm::mat4 boneToModel = glm::inverse(modelToBone);
            
            //auto poz = boneToModel * boneAnim /* rolltransf*/;
            
            if(boneInstance[i].parentIndex == (uint32_t)-1){
                poseobj->pose[i] = boneToModel * boneAnim * modelToBone;
            } else {
                poseobj->pose[i] = poseobj->pose[boneInstance[i].parentIndex] * boneToModel * boneAnim * modelToBone;
            }
            
            /*
            glm::vec3 o(0.0f);
            glm::vec3 x(1.0f, 0.0f, 0.0f);
            glm::vec3 y(0.0f, 0.0f, -1.0f);
            glm::vec3 z(0.0f, 1.0f, 0.0f);
            o = poz * glm::vec4(o, 1.0f);
            Render::AddLine(o, poz * glm::vec4(x, 1.0f), Render::COLOR_RED);
            Render::AddLine(o, poz * glm::vec4(y, 1.0f), Render::COLOR_GREEN);
            Render::AddLine(o, poz * glm::vec4(z, 1.0f), Render::COLOR_BLUE);
            */
        }
    }    
}