// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// ARMATURECOMPONENT.H -- armature/animation component
// This component is for genereting pose matrices from an armature (it's
// sometimes also called a skeleton), as well as playing back animations.

#ifndef ARMATURECOMPONENT_H
#define ARMATURECOMPONENT_H

#include <render/render.h>

namespace Core {
class ArmatureComponent : public EntityComponent {
    protected:
        static const size_t BONE_COUNT = 30;
        static const size_t ANIM_COUNT = 8;
        struct AnimationPlaybackInfo {
            uint32_t repeats;
            float weight;
            float speed;
            float frame;
            Render::NameCount* animation_header = nullptr;
            Render::NameCount* keyframe_headers[BONE_COUNT] = {nullptr};
        };
        Render::PoseListObject* poseobj;
        ResourceProxy<Render::Model> model;
        uint64_t boneCount = 0;
        Render::Bone boneInstance[BONE_COUNT];
        Render::Keyframe animInstance[BONE_COUNT];
        uint64_t animPlaying[8] = {0};
        Render::Keyframe* boneKeyframes[8][BONE_COUNT] = {nullptr};
        uint32_t boneKeyframeCount[8][BONE_COUNT] = {0};
        uint32_t animRepeats[8];
        float animWeight[8];
        float animSpeed[8];
        float animFrame[8];
        float lastUpdate = 0.0f;
        
        Render::Bone* armature_bones;
        size_t armature_bone_count;
        uint32_t armature_bone_parents[BONE_COUNT];
        
        name_t anim_playing[ANIM_COUNT] = {0};
        AnimationPlaybackInfo anim_info[ANIM_COUNT];
    public:
        ArmatureComponent() : model(this){}
        void Init();
        void Uninit();
        void Start();
        void SetModel(uint64_t name){
            model.SetResource(Render::Model::Find(name));
        };
        uint64_t GetModel(){
            return model->GetName();
        }
        Render::PoseListObject* GetPosePtr(){return poseobj;};
        void SetBoneKeyframe (uint64_t boneName, Render::Keyframe& keyframe);
        void PlayAnimation(uint64_t animationName, uint32_t repeats, float weight, float speed);
        void StopAnimation(uint64_t animationName);
        void Update();
        void EventHandler(Event &event){return;}
    };
}

#endif //ARMATURECOMPONENT_H