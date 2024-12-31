// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <components/animation.h>

#include <framework/entity.h>
#include <config.h>

#include <cstring>

/**
 * @class tram::AnimationComponent components/animation.h <components/animation.h>
 * 
 * Plays back Animations.
 * @see https://racenis.github.io/tram-sdk/documentation/components/animation.html
 */

namespace tram {
    
template <> Pool<AnimationComponent> PoolProxy<AnimationComponent>::pool("AnimationComponent pool", COMPONENT_LIMIT_ANIMATION, false);
template <> void Component<AnimationComponent>::init() { ptr = PoolProxy<AnimationComponent>::New(); }
template <> void Component<AnimationComponent>::yeet() { PoolProxy<AnimationComponent>::Delete(ptr); }

void AnimationComponent::Init() {
    assert(!is_ready);
    
    // I feel like this code should be in in the constructor instead
    
    is_init = true;
    pose = PoolProxy<Render::Pose>::New();

    // initialize animation matrices to identity matrix
    for (size_t i = 0; i < Render::BONE_COUNT; i++) {
        pose->pose[i] = mat4(1.0f);
    }

    if (resources_waiting == 0) Start();
}

AnimationComponent::~AnimationComponent() {
    assert(is_ready);
    assert(pose);
    
    // if delete this component before the model resource is lodaed, we will
    // fail on these asserts
    // TODO: fix
    
    PoolProxy<Render::Pose>::Delete(pose);
    
    pose = nullptr;
    is_ready = false;
}

void AnimationComponent::Start() {
    assert(!is_ready);
    
    // it's probably not necessary to cache this, but whatever
    armature_bone_count = model->GetArmature().size();
    armature_bones = &model->GetArmature()[0];
    
    // making a copy of bone parents is needed for some fun effects
    for (size_t i = 0; i < armature_bone_count; i++) {
        armature_bone_parents[i] = armature_bones[i].parent;
    }
    
    for (size_t i = 0; i < ANIM_COUNT; i++) {
        if (anim_playing[i]) FindKeyframePointers(i);
    }

    is_ready = true;
}

/// Sets procedural animation keyframe.
void AnimationComponent::SetKeyframe(name_t bone_name, const Render::Keyframe& keyframe) {
    for (size_t i = 0; i < armature_bone_count; i++) {
        if (armature_bones[i].name == bone_name) {
            base_pose[i] = keyframe;
            return;
        }
    }
}

/// Plays an animation.
/// Animation must be already loaded before playing.
/// @note  Will not play an animation if there are no empty animation slots.
/// @param animation_name   Name of the animation to be played.
/// @param repeats          How many times the animation will be looped before stopping.
/// @param weight           Weight of the animation. Set to 1.0 for normal weight. Should
///                         be in range from 0.0 to 1.0, if set outside of range, then the
///                         animation might look really weird.
/// @param speed            Speed of the animation. Value of 1.0 will play the animation
///                         at regular speed, 0.5 will play it half speed, 2.0 double speed.
/// @param interpolate      If set to true, then animation will be interpolated, if set to
///                         false, then it will use only the latest keyframe.
/// @param pause_on_last    If set to true, then the animation will pause on the last keyframe.
void AnimationComponent::Play(name_t animation_name, uint32_t repeats, float weight, float speed, bool interpolate, bool pause_on_last_frame) {
    if (!animation_name) return;
    
    // TODO: guard for negative weight? speed?
    
    // find an empty slot for the animation
    size_t slot;
    for (slot = 0; slot < ANIM_COUNT; slot++) {
        // maybe reset the animation if its already playing, instead of just returning?
        if (anim_playing[slot] == animation_name) return;
        if (anim_playing[slot] == UID()) break;
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
    anim_info[slot].fade_ammount = 1.0f;
    anim_info[slot].pause = false; 
    anim_info[slot].fade_in = false; 
    anim_info[slot].fade_out = false; 
    anim_info[slot].pause_on_last_frame = pause_on_last_frame;
    anim_info[slot].animation_header = nullptr;
    
    for (size_t i = 0; i < Render::BONE_COUNT; i++) anim_info[slot].keyframe_headers[i] = nullptr;
    
    anim_info[slot].animation_header = Render::Animation::Find(animation_name)->GetPointer();

    if(anim_info[slot].animation_header == nullptr){
        std::cout << "Animation " << animation_name << " not loaded!" << std::endl;
        anim_playing[slot] = 0;
        return;
    }
    
    if (is_ready) {
        FindKeyframePointers(slot);
    }
}

/// Extracts pointers to keyframes from animation data.
void AnimationComponent::FindKeyframePointers(size_t animation_index) {
    const auto& slot = animation_index;
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

/// Stops an animation if its playing.
/// Will do nothing if the animation is not playing.
void AnimationComponent::Stop(name_t animation_name) {
    if (!animation_name) return;
    
    for (size_t i = 0; i < ANIM_COUNT; i++) {
        if(anim_playing[i] == animation_name) {
            anim_playing[i] = UID();

            // reset headers
            for (size_t j = 0; j < Render::BONE_COUNT; j++) {
                anim_info[i].keyframe_headers[j] = nullptr;
            }
            
            if (anim_finish_callback) anim_finish_callback(this, animation_name);
            return;
        }
    }
}

/// Sets animation's pause state.
/// This will do nothing, if the animation is not playing. Pausing an already paused
/// or continuing an already playing animation will do nothing.
/// @param animation_name   Name of the animation to pause.
/// @param pause            Set to true to pause the animation, set to false to continue.
void AnimationComponent::SetPause(name_t animation_name, bool pause) {
    if (!animation_name) return;
    
    for (size_t i = 0; i < ANIM_COUNT; i++) {
        if (anim_playing[i] == animation_name) {
            anim_info[i].pause = pause;
            return;
        }
    }
}

/// Pauses an animation.
/// If the animation is being played, it will be paused. Otherwise this method
/// will do nothing. Use Continue() method to continue playing the animation.
void AnimationComponent::Pause(name_t animation_name) {
    SetPause(animation_name, true);
}

/// Continues an animation.
/// If an animation is paused, it will continue playback. Otherwise this method
/// will do nothing.
void AnimationComponent::Continue(name_t animation_name) {
    SetPause(animation_name, false);
}

/// Fades in an animation.
/// Before fading in an animation, it must be started with Play().
void AnimationComponent::FadeIn(name_t animation_name, float speed) {
    SetFade(animation_name, true, speed);
}

/// Fades out an animation.
void AnimationComponent::FadeOut(name_t animation_name, float speed) {
    SetFade(animation_name, false, speed);
}

/// Sets the weight of an animation.
void AnimationComponent::SetWeight(name_t animation_name, float weight) {
    for (size_t i = 0; i < ANIM_COUNT; i++) {
        if (anim_playing[i] == animation_name) {
            anim_info[i].weight = weight;
            return;
        }
    }
}

/// Sets the speed of an animation.
void AnimationComponent::SetSpeed(name_t animation_name, float speed) {
    for (size_t i = 0; i < ANIM_COUNT; i++) {
        if (anim_playing[i] == animation_name) {
            anim_info[i].speed = speed;
            return;
        }
    }
}

/// Sets the repeats of an animation.
void AnimationComponent::SetRepeats(name_t animation_name, uint32_t repeats) {
    for (size_t i = 0; i < ANIM_COUNT; i++) {
        if (anim_playing[i] == animation_name) {
            anim_info[i].repeats = repeats;
            return;
        }
    }
}

/// Returns true if an animation is playing.
bool AnimationComponent::IsPlaying(name_t animation_name) {
    for (size_t i = 0; i < ANIM_COUNT; i++) {
        if (anim_playing[i] == animation_name) {
            return true;
        }
    }
    return false;
}

/// Fades in or fades out an animation.
/// This will gradually increase or decrease the weight of an animation until it is fully
/// faded in or faded out.
/// The animation must already be started with Play() for it to
/// be faded, otherwise this method will do nothing.
/// If an animation is fully faded out, it will automatically be stopped.
/// @param animation_name Name of the animation to be faded.
/// @param fade_in      Set to true, if animation is to be faded in, false if faded out.
/// @param fade_length  How long, in seconds, will it take for the animation to finish
///                     fading in/out.
void AnimationComponent::SetFade(name_t animation_name, bool fade_in, float fade_length) {
    for (size_t i = 0; i < ANIM_COUNT; i++) {
        if (anim_playing[i] == animation_name) {
            anim_info[i].fade_in = fade_in;
            anim_info[i].fade_out = !fade_in;
            anim_info[i].fade_speed = 1.0f / fade_length;
            anim_info[i].fade_ammount = fade_in ? 0.0f : 1.0f;
            return;
        }
    }
}

/// Reparents a bone.
/// Make sure to not create cycles in the bone hierarchy.
/// @param bone_name    Name of the bone for which a new parent will be set.
/// @param new_parent   Name of the bone which will be the new parent.
///                     Can be set to "none" to clear the bone's parent.
void AnimationComponent::Reparent(name_t bone_name, name_t new_parent) {
    int32_t bone_index = -1;
    int32_t parent_index = -1;
    
    for (size_t i = 0; i < armature_bone_count; i++) {
        if (armature_bones[i].name == bone_name) {
            bone_index = i;
        } else if (armature_bones[i].name == new_parent) {
            parent_index = i;
        }
    }
    
    if (bone_index == -1) return;
    
    armature_bone_parents[bone_index] = parent_index;
}

/// Updates all of the armatures.
/// This static function calls the Refresh() method on all of the ArmatureComponents.
void AnimationComponent::Update() {
    for (auto& comp : PoolProxy<AnimationComponent>::GetPool()) comp.Refresh();
}

/// Sets an animation to a specific frame.
/// If the animation is not already started with Play(), then this method will
/// do nothing.
void AnimationComponent::SetFrame(name_t animation_name, float frame) {
    for (size_t i = 0; i < ANIM_COUNT; i++){
        if(anim_playing[i] == animation_name){
            anim_info[i].frame = frame;
            return;
        }
    }
}

/// Updates an armature.
/// Pushes animations forward and regenerates matrices.
void AnimationComponent::Refresh() {
    
    // it might be useful in the future to split this method into multiple methods
    
    if (!is_ready) return;
    Render::Keyframe anim_mixed[Render::BONE_COUNT];
    for (uint64_t i = 0; i < armature_bone_count; i++) anim_mixed[i] = base_pose[i];
    
    // increment animations' frames and check if they have stopped/repeated
    for (size_t i = 0; i < ANIM_COUNT; i++) {
        if(anim_playing[i] == UID()) continue;
        
        // increase the frames of the animation
        auto& anim = anim_info[i];
        float frames_since_update = GetDeltaTime();
        if (!anim.pause) anim.frame += frames_since_update * anim_info[i].speed;
        
        // do fade-ins/fade-outs
        if (anim.fade_in) {
            anim.fade_ammount += frames_since_update * anim.fade_speed;
            if (anim.fade_ammount > 1.0f) anim.fade_in = false;
        } else if (anim.fade_out) {
            anim.fade_ammount -= frames_since_update * anim.fade_speed;
            if (anim.fade_ammount < 0.0f) {
                Stop(anim_playing[i]);
            }
        }
        
        // find the first keyframe header
        for (size_t k = 0; k < armature_bone_count; k++){
            Render::NameCount* keyframe_header = anim_info[i].keyframe_headers[k];
            if (keyframe_header == nullptr) continue;
            
            Render::Keyframe* keyframes = (Render::Keyframe*)(keyframe_header + 1);
            size_t keyframe_count = keyframe_header->second;
            
            // if playback's current frame is after the last frame of the animation
            if (anim.frame > keyframes[keyframe_count-1].frame) {
                if (anim.pause_on_last_frame && anim.repeats == 1) {
                    anim.frame = keyframes[keyframe_count-1].frame - 0.1f;
                    anim.pause = true;
                    
                    // maybe instead of calling finish callback? call pause callback?
                    if (anim_finish_callback) anim_finish_callback(this, anim_playing[i]);
                } else {
                    anim.repeats--;
                    anim.frame = 0.0f;
                    
                    if (anim.repeats == 0) {
                        //Stop(anim.animation_header->first);
                        Stop(anim_playing[i]);
                        continue;
                    }
                }
            }
        }
    }
    
    
    // mix together keyframes
    for (size_t i = 0; i < ANIM_COUNT; i++) {
        if (!anim_playing[i]) continue;
                
        for (size_t k = 0; k < armature_bone_count; k++) {
            Render::NameCount* keyframe_header = anim_info[i].keyframe_headers[k];
            if (keyframe_header != nullptr) {
                Render::Keyframe* keyframes = (Render::Keyframe*)(keyframe_header + 1);
                size_t keyframe_count = keyframe_header->second;
                const auto& anim = anim_info[i];
                
                // find the first keyframe that happens after the animation's current frame
                size_t second_keyframe = -1llu;
                for (size_t f = 0; f < keyframe_count; f++) {
                    if (keyframes[f].frame > anim.frame) {
                        second_keyframe = f;
                        break;
                    }
                }
                
                // if not found, that means that the current frame is past the end of the animation
                // so let's set the animation to the last frame
                if (second_keyframe == -1llu) {
                    second_keyframe = keyframe_count - 1;
                }
                
                size_t first_keyframe = second_keyframe - 1;
                
                // interpolation ratio between keyframes
                float mix_w = anim.interpolate ? (anim.frame - keyframes[second_keyframe].frame) / (keyframes[first_keyframe].frame - keyframes[second_keyframe].frame) : 0.0f;
                
                // add up animation's weight and fade
                float total_mix_weight = anim.weight * anim.fade_ammount;
                
                // mix together will all other animations
                anim_mixed[k].location += glm::mix(keyframes[second_keyframe].location, keyframes[first_keyframe].location, mix_w) * total_mix_weight;
                anim_mixed[k].rotation *= glm::mix(quat(vec3(0.0f)), glm::mix(keyframes[second_keyframe].rotation, keyframes[first_keyframe].rotation, mix_w), total_mix_weight);
                anim_mixed[k].scale = glm::mix(anim_mixed[k].scale, anim_mixed[k].scale * glm::mix(keyframes[second_keyframe].scale, keyframes[first_keyframe].scale, mix_w), total_mix_weight); // * total_mix_weight;                    
            }
        }
    }

    // convert mixed keyframes to pose matrices
    for(uint64_t i = 0; i < armature_bone_count; i++){

        pose->pose[i] = mat4(1.0f);

        mat4 modelToBone = glm::translate(mat4(1.0f), -armature_bones[i].head);
        
        const auto& head = armature_bones[i].head;
        const auto& tail = armature_bones[i].tail;
        const auto& roll = armature_bones[i].roll;
        
        vec3 tail_dir = glm::normalize(tail - head);
        
        auto rot = glm::rotation(tail_dir, vec3(0.0f, 0.0f, -1.0f));
        modelToBone = glm::toMat4(rot) * modelToBone;
        auto rolltransf = glm::rotate(mat4(1.0f), -roll, vec3(0.0f, 0.0f, -1.0f));
        modelToBone = rolltransf * modelToBone;


        mat4 boneAnim = mat4(1.0f);
        boneAnim = glm::toMat4(anim_mixed[i].rotation) * boneAnim;
        boneAnim = glm::translate(mat4(1.0f), anim_mixed[i].location) * boneAnim;
        boneAnim = glm::scale(mat4(1.0f), anim_mixed[i].scale) * boneAnim;

        mat4 boneToModel = glm::inverse(modelToBone);

        if (armature_bone_parents[i] == -1) {
            pose->pose[i] = boneToModel * boneAnim * modelToBone;
        } else {
            pose->pose[i] = pose->pose[armature_bone_parents[i]] * boneToModel * boneAnim * modelToBone;
        }
        
        /* idk i don't remember what this debugging code is for, but might be useful
        vec3 o(0.0f);
        vec3 x(1.0f, 0.0f, 0.0f);
        vec3 y(0.0f, 0.0f, -1.0f);
        vec3 z(0.0f, 1.0f, 0.0f);
        o = poz * vec4(o, 1.0f);
        Render::AddLine(o, poz * vec4(x, 1.0f), Render::COLOR_RED);
        Render::AddLine(o, poz * vec4(y, 1.0f), Render::COLOR_GREEN);
        Render::AddLine(o, poz * vec4(z, 1.0f), Render::COLOR_BLUE);
        */
    }
}

static EventListener frame_event;
static bool draw_info = false;

/// Checks whether the debug text is drawn.
/// Check SetDebugInfoDraw() for more info.
bool AnimationComponent::IsDebugInfoDraw() {
    return draw_info;
}

/// Sets the drawing of debug info.
/// If set to true, each frame some debug text will be drawn for each animation
/// component. This is useful for debugging.
void AnimationComponent::SetDebugInfoDraw(bool draw) {
    if (draw_info == draw) return;
    draw_info = draw;
    
    if (draw) {
        frame_event.make(Event::FRAME, [](Event&) {
            for (auto& comp : PoolProxy<AnimationComponent>::GetPool()) {
                if (!comp.parent) continue;
                char str[200] = "";
                char buf[100];
                for (size_t i = 0; i < ANIM_COUNT; i++) {
                    if (!comp.anim_playing[i]) continue;
                    sprintf(buf, "[%i] %.2f | %i | %s\n",
                        (int)i,
                        comp.anim_info[i].weight,
                        comp.anim_info[i].repeats,
                        (const char*)comp.anim_playing[i]);
                    strcat(str, buf);
                }
                
                Render::AddText(comp.GetParent()->GetLocation(), str);
            }
        });
    } else {
        frame_event.clear();
    } 
}

}