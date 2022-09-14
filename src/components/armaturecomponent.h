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
        Render::PoseListObject* poseobj;
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