// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_ARMATURECOMPONENT_H
#define TRAM_SDK_COMPONENTS_ARMATURECOMPONENT_H

#include <framework/core.h>
#include <framework/entitycomponent.h>
#include <render/render.h>
#include <render/model.h>

namespace tram {

class AnimationComponent : public EntityComponent {
public:
    void Init();
    void Start();
    void SetModel(Render::Model* model) { this->model = model; }
    void SetModel(name_t model) { this->model = Render::Model::Find(model); }
    Render::Model* GetModel() { return model.get(); }
    Render::Pose* GetPose() { return pose; }
    
    void SetKeyframe(name_t bone_name, const Render::Keyframe& keyframe);
    void SetOnAnimationFinishCallback(void (*callback) (AnimationComponent*, name_t)) { anim_finish_callback = callback; }
    
    void Play(name_t animation_name, uint32_t repeats, float weight, float speed, bool interpolate = true, bool pause_on_last_frame = false);
    bool IsPlaying(name_t animation_name);
    void Stop(name_t animation_name);
    void Pause(name_t animation_name);
    void Continue(name_t animation_name);
    
    void SetWeight(name_t animation_name, float weight);
    void SetSpeed(name_t animation_name, float speed);
    void SetRepeats(name_t animation_name, uint32_t repeats);
    
    void FadeIn(name_t animation_name, float length);
    void FadeOut(name_t animation_name, float length);
    
    void SetPause(name_t animation_name, bool pause);
    void SetFade(name_t animation_name, bool fade_in, float fade_length);
    void SetFrame(name_t animation_name, float frame);
    
    void Reparent(name_t bone_name, name_t new_parent);
    
    void Refresh();
    void EventHandler(Event &event) { return; }
    static bool IsDebugInfoDraw();
    static void SetDebugInfoDraw(bool draw);
    static void Update();
protected:
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
        Render::NameCount* keyframe_headers[Render::BONE_COUNT] = {nullptr};
    };

    void FindKeyframePointers(size_t animation_index);

    Render::Keyframe base_pose[Render::BONE_COUNT];
    
    Render::Pose* pose = nullptr;
    ResourceProxy<Render::Model> model;
    
    void (*anim_finish_callback) (AnimationComponent*, name_t) = nullptr;
    
    const Render::Bone* armature_bones = nullptr;
    size_t armature_bone_count = 0;
    int32_t armature_bone_parents[Render::BONE_COUNT] = {-1};
    
    name_t anim_playing[ANIM_COUNT];
    AnimationPlaybackInfo anim_info[ANIM_COUNT];
    
    AnimationComponent() : model(this) {}
    ~AnimationComponent();
    template <typename> friend class Pool;
};

}

#endif // TRAM_SDK_COMPONENTS_ARMATURECOMPONENT_H