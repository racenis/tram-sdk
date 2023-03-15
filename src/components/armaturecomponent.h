// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_ARMATURECOMPONENT_H
#define COMPONENTS_ARMATURECOMPONENT_H

#include <framework/core.h>
#include <framework/entitycomponent.h>
#include <render/render.h>
#include <render/model.h>

namespace tram {
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

        void FindKeyframePointers(size_t animation_index);

        Render::Keyframe base_pose[BONE_COUNT];
        
        Render::Pose* poseobj = nullptr;
        ResourceProxy<Render::Model> model;
        
        float last_update = 0.0f;
        
        void (*anim_finish_callback) (ArmatureComponent*, name_t) = nullptr;
        
        Render::Bone* armature_bones = nullptr;
        size_t armature_bone_count = 0;
        uint32_t armature_bone_parents[BONE_COUNT] = {-1ul};
        
        name_t anim_playing[ANIM_COUNT];
        AnimationPlaybackInfo anim_info[ANIM_COUNT];
        
        ArmatureComponent() : model(this){}
        ~ArmatureComponent();
        template <typename> friend class Pool;
    public:
        void Init();
        void Start();
        void SetModel(name_t name) { model = Render::Model::Find(name); }
        name_t GetModel() { return model->GetName(); }
        Render::Pose* GetPose(){ return poseobj; };
        void SetBoneKeyframe (name_t bone_name, const Render::Keyframe& keyframe);
        void SetOnAnimationFinishCallback(void (*callback) (ArmatureComponent*, name_t)) { anim_finish_callback = callback; }
        void PlayAnimation(name_t animation_name, uint32_t repeats, float weight, float speed, bool interpolate = true, bool pause_on_last_frame = false);
        bool IsPlayingAnimation(name_t animation_name);
        void StopAnimation(name_t animation_name);
        void PauseAnimation(name_t animation_name, bool pause);
        void FadeAnimation(name_t animation_name, bool fade_in, float fade_speed);
        void SetFrameAnimation(name_t animation_name, float frame);
        void Refresh();
        void EventHandler(Event &event){ return; }
        static void Update();
    };
}

#endif // COMPONENTS_ARMATURECOMPONENT_H