// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// ARMATURE.CPP -- animation code.
// Implementation of animation loading, armature component.

#include <render/armature.h>
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
        NameCount* nameptr = (NameCount*)animationpool.begin();
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
                    file >> kframe->location.x;
                    file >> kframe->location.y;
                    file >> kframe->location.z;
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
        // why is there no 'is_ready' stuff in here?
        poseobj = Render::poseList.AddNew();

        for (size_t i = 0; i < Render::BONE_COUNT; i++) poseobj->pose[i] = glm::mat4(1.0f);

        if (resources_waiting == 0) Start();
    };
        
    void ArmatureComponent::Uninit(){
        assert(poseobj);
        Render::poseList.Remove(poseobj);
        poseobj = nullptr;
        is_ready = false;
    };
    
    void ArmatureComponent::Start(){
        // it's probably not necessary to cache this, but whatever
        armature_bone_count = model->GetArmatureSize();
        armature_bones = model->GetArmature();
        
        // making a copy of bone parents is needed for some fun effects
        for (size_t i = 0; i < armature_bone_count; i++) {
            armature_bone_parents[i] = armature_bones[i].parentIndex;
        }

        is_ready = true;
    }
    
    void ArmatureComponent::SetBoneKeyframe (name_t bone_name, const Render::Keyframe& keyframe){
        // not super efficient, but it works
        // maybe add a method for setting the keyframe based on the index
        for (size_t i = 0; i < armature_bone_count; i++){
            if (armature_bones[i].name == bone_name){
                base_pose[i] = keyframe;
                return;
            }
        }
    }
        
    void ArmatureComponent::PlayAnimation(name_t animation_name, uint32_t repeats, float weight, float speed, bool interpolate, bool pause_on_last_frame){
        // find an empty slot for the animation
        size_t slot;
        for (slot = 0; slot < ANIM_COUNT; slot++) {
            // maybe reset the animation if its already playing, instead of just returning?
            if (anim_playing[slot] == animation_name) return;
            if (anim_playing[slot] == 0) break;
        }
        
        // maybe log an error if all animation slots taken?
        if (slot == ANIM_COUNT) return;
        
        anim_playing[slot] = animation_name;
        
        // maybe add an option to start the animation at a specific frame, instead of the beginning
        anim_info[slot].frame = 0.0f; 
        anim_info[slot].repeats = repeats; 
        anim_info[slot].weight = weight; 
        anim_info[slot].speed = speed; 
        anim_info[slot].interpolate = interpolate; 
        anim_info[slot].pause = false; 
        anim_info[slot].fade_in = false; 
        anim_info[slot].fade_out = false; 
        anim_info[slot].pause_on_last_frame = pause_on_last_frame;
        anim_info[slot].animation_header = nullptr;
        
        for (size_t i; i < BONE_COUNT; i++) anim_info[slot].keyframe_headers[i] = nullptr;
        
        anim_info[slot].animation_header = Render::Animation::Find(animation_name);

        if(anim_info[slot].animation_header == nullptr){
            std::cout << "Animation " << ReverseUID(animation_name) << " not found!" << std::endl;
            anim_playing[slot] = 0;
            return;
        }

        size_t anim_bone_count = anim_info[slot].animation_header->second;
        
        Render::NameCount* keyframe_header = anim_info[slot].animation_header + 1;
        
        // match up bones in the armature to keyframe headers in the animation data
        for (size_t i = 0; i < anim_bone_count; i++) {
            size_t bone_slot = -1llu;

            for(size_t j = 0; j < armature_bone_count; j++){
                if(armature_bones[j].name == keyframe_header->first){
                    bone_slot = j;
                    break;
                }
            }

            if(bone_slot != -1llu){
                anim_info[slot].keyframe_headers[bone_slot] = keyframe_header;
            }

            // skip over all of the keyframes and get to the next header
            keyframe_header = (Render::NameCount*)(((Render::Keyframe*)(keyframe_header + 1)) + keyframe_header->second);
        }        
    }
    
    void ArmatureComponent::StopAnimation(name_t animation_name){
        for (size_t i = 0; i < ANIM_COUNT; i++){
            if(anim_playing[i] == animation_name){
                anim_playing[i] = 0;
                // TODO: reset all of the keyframe pointers
                return;
            }
        }
    }
    
    void ArmatureComponent::PauseAnimation(name_t animation_name, bool pause) {
        for (size_t i = 0; i < ANIM_COUNT; i++){
            if(anim_playing[i] == animation_name){
                anim_info[i].pause = pause;
                return;
            }
        }
    }
    
    bool ArmatureComponent::IsPlayingAnimation(name_t animation_name) {
        for (size_t i = 0; i < ANIM_COUNT; i++) {
            if (anim_playing[i] == animation_name) {
                return true;
            }
        }
        return false;
    }
    
    void ArmatureComponent::FadeAnimation(name_t animation_name, bool fade_in, float fade_speed) {
        for (size_t i = 0; i < ANIM_COUNT; i++){
            if(anim_playing[i] == animation_name){
                anim_info[i].fade_in = fade_in;
                anim_info[i].fade_out = !fade_in;
                anim_info[i].fade_speed = fade_speed;
                anim_info[i].fade_ammount = fade_in ? 0.0f : 1.0f;
                return;
            }
        }
    }
    
    void ArmatureComponent::SetFrameAnimation(name_t animation_name, float frame) {
        for (size_t i = 0; i < ANIM_COUNT; i++){
            if(anim_playing[i] == animation_name){
                anim_info[i].frame = frame;
                return;
            }
        }
    }
    
    void ArmatureComponent::Update(){
        Render::Keyframe anim_mixed[BONE_COUNT];
        for (uint64_t i = 0; i < armature_bone_count; i++) anim_mixed[i] = base_pose[i];
        
        // mix together keyframes
        for (size_t i = 0; i < ANIM_COUNT; i++){
            if(anim_playing[i] == 0) continue;
            
            auto& anim = anim_info[i];
            float frames_since_update = (FRAME_TIME - last_update) * 24.0f;
            if (!anim.pause) anim.frame += frames_since_update * anim_info[i].speed;
            
            for (size_t k = 0; k < armature_bone_count; k++){
                Render::NameCount* keyframe_header = anim_info[i].keyframe_headers[k];
                if (keyframe_header != nullptr) {
                    Render::Keyframe* keyframes = (Render::Keyframe*)(keyframe_header + 1);
                    size_t keyframe_count = keyframe_header->second;
                    
                    // find the first keyframe that happens after the animation's current frame
                    size_t second_keyframe = -1llu;
                    for (size_t i = 0; i < keyframe_count; i++) {
                        if (keyframes[i].frame > anim.frame) {
                            second_keyframe = i;
                            break;
                        }
                    }
                    
                    // if not found, that means that the current frame is past the end of the animation
                    if (second_keyframe == -1llu) {
                        if (anim.pause_on_last_frame && anim.repeats == 1) {
                            second_keyframe = keyframe_count - 1;
                            anim.pause = true;
                        } else {
                            anim.repeats--;
                            anim.frame = 0.0f;
                            
                            second_keyframe = 1;
                            
                            if (anim.repeats == 0) {
                                StopAnimation(anim.animation_header->first);
                                continue;
                            }
                        }
                    }
                    
                    size_t first_keyframe = second_keyframe - 1;
                    
                    // interpolation ratio between keyframes
                    float mix_w = anim.interpolate ? (anim.frame - keyframes[second_keyframe].frame) / (keyframes[first_keyframe].frame - keyframes[second_keyframe].frame) : 0.0f;
                    
                    // total mix weight
                    float total_mix_weight = anim.weight;
                    if (anim.fade_in) {
                        anim.fade_ammount += frames_since_update * anim.fade_speed;
                        total_mix_weight *= anim.fade_ammount;
                        if (anim.fade_ammount > 1.0f) anim.fade_in = false;
                    } else if (anim.fade_out) {
                        anim.fade_ammount -= frames_since_update * anim.fade_speed;
                        total_mix_weight *= anim.fade_ammount;
                        if (anim.fade_ammount < 0.0f) StopAnimation(anim.animation_header->first);
                    }
                    
                    // mix together will all other animations
                    anim_mixed[k].location += glm::mix(keyframes[second_keyframe].location, keyframes[first_keyframe].location, mix_w) * total_mix_weight;
                    anim_mixed[k].rotation *= glm::mix(glm::quat(glm::vec3(0.0f)), glm::mix(keyframes[second_keyframe].rotation, keyframes[first_keyframe].rotation, mix_w), total_mix_weight);
                    anim_mixed[k].scale *= glm::mix(keyframes[second_keyframe].scale, keyframes[first_keyframe].scale, mix_w) * total_mix_weight;                    
                }
            }
        }

        last_update = FRAME_TIME;
        
        // convert mixed keyframes to pose matrices
        for(uint64_t i = 0; i < armature_bone_count; i++){

            poseobj->pose[i] = glm::mat4(1.0f);

            glm::mat4 modelToBone = glm::translate(glm::mat4(1.0f), -armature_bones[i].head);
            
            auto& head = armature_bones[i].head;
            auto& tail = armature_bones[i].tail;
            auto& roll = armature_bones[i].roll;
            
            glm::vec3 tail_dir = glm::normalize(tail - head);
            
            auto rot = glm::rotation(tail_dir, glm::vec3(0.0f, 0.0f, -1.0f));
            modelToBone = glm::toMat4(rot) * modelToBone;
            auto rolltransf = glm::rotate(glm::mat4(1.0f), -roll, glm::vec3(0.0f, 0.0f, -1.0f));
            modelToBone = rolltransf * modelToBone;


            glm::mat4 boneAnim = glm::mat4(1.0f);
            boneAnim = glm::toMat4(anim_mixed[i].rotation) * boneAnim;
            boneAnim = glm::translate(glm::mat4(1.0f), anim_mixed[i].location) * boneAnim;
            //boneAnim = glm::scale(glm::mat4(1.0f), anim_mixed[i].scale) * boneAnim;
            // where's the scale? forgotted?

            glm::mat4 boneToModel = glm::inverse(modelToBone);
            
            //auto poz = boneToModel * boneAnim /* rolltransf*/;
            
            if(armature_bone_parents[i] == (uint32_t)-1){
                poseobj->pose[i] = boneToModel * boneAnim * modelToBone;
            } else {
                poseobj->pose[i] = poseobj->pose[armature_bone_parents[i]] * boneToModel * boneAnim * modelToBone;
            }
            
            /* idk i don't remember what this debugging code is for, but might be useful
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