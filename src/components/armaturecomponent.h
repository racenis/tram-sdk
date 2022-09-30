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
            float fade_speed;
            float fade_ammount;
            bool interpolate;
            bool fade_in;
            bool fade_out;
            bool pause;
            bool pause_on_last_frame;
            Render::NameCount* animation_header = nullptr;
            Render::NameCount* keyframe_headers[BONE_COUNT] = {nullptr};
        };

        Render::Keyframe base_pose[BONE_COUNT];
        
        Render::PoseListObject* poseobj = nullptr;
        ResourceProxy<Render::Model> model;
        
        float last_update = 0.0f;
        
        Render::Bone* armature_bones = nullptr;
        size_t armature_bone_count = 0;
        uint32_t armature_bone_parents[BONE_COUNT] = {-1ul};
        
        name_t anim_playing[ANIM_COUNT] = {0};
        AnimationPlaybackInfo anim_info[ANIM_COUNT];
    public:
        ArmatureComponent() : model(this){}
        void Init();
        void Uninit();
        void Start();
        void SetModel(name_t name) { model.SetResource(Render::Model::Find(name)); }
        name_t GetModel() { return model->GetName(); }
        Render::PoseListObject* GetPosePtr(){ return poseobj; };
        void SetBoneKeyframe (name_t bone_name, const Render::Keyframe& keyframe);
        void PlayAnimation(name_t animation_name, uint32_t repeats, float weight, float speed, bool interpolate = true, bool pause_on_last_frame = false);
        bool IsPlayingAnimation(name_t animation_name);
        void StopAnimation(name_t animation_name);
        void PauseAnimation(name_t animation_name, bool pause);
        void FadeAnimation(name_t animation_name, bool fade_in, float fade_speed);
        void SetFrameAnimation(name_t animation_name, float frame);
        void Update();
        void EventHandler(Event &event){ return; }
    };
}

#endif //ARMATURECOMPONENT_H