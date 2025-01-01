// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/animation.h>
#include <components/animation.h>
#include <framework/logging.h>
#include <framework/file.h>

#include <templates/hashmap.h>
#include <templates/stackpool.h>

#include <cstring>

/**
 * @class tram::Render::Animation render/animation.h <render/animation.h>
 * 
 * Skeletal animation for a 3D model.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/render/animation.html
 */
 
 /**
 * @struct tram::Render::Keyframe render/animation.h <render/animation.h>
 * 
 * Keyframe for a skeletal animation.
 * 
 * All of the translation, rotation and scaling operations will be performed
 * in a given bone's local space.
 * 
 * @property tram::Render::Keyframe::frame
 * Used only when keyframes are used in the context of an animation.
 * 
 * @see tram::Render::Animation
 * @see https://racenis.github.io/tram-sdk/documentation/render/animation.html
 */

 /**
 * @struct tram::Render::Bone render/animation.h <render/animation.h>
 * 
 * Bone for a 3D models skeleton.
 * 
 * The name property identifies a bone and is referenced in animations. The
 * parent property is used to construct the skeletal hierarchy. The head, tail
 * and roll properties are used to construct the bone's local space within the
 * local space of the 3D model.
 * 
 * @property tram::Render::Bone::name
 * Unique identifier for the bone.
 * Must be unique in the context of a skeleton. This name is how Animation
 * keyframes will reference the bone.
 * 
 * @property tram::Render::Bone::parent
 * Parent index of the bone.
 * This index is in the context of a skeleton. If set to an index of some other
 * bone, this bone will move together with the parent bone.
 * Can be set to `-1` if you don't want the bone to move with any other bone,
 * and instead want it to move with the models origin or for bones with no
 * parents, i.e. root bones.
 * 
 * @property tram::Render::Bone::head
 * Origin of the bone.
 * Position in model space. The bone will rotate and scale around this point.
 * 
 * @property tram::Render::Bone::tail
 * Direction of the bone.
 * Position in model space. The vector going from the head of the bone to the
 * tail of the bone will be used as the `z` axis of the bone.
 * 
 * @property tram::Render::Bone::roll
 * Rotation of the bone.
 * Determines the rotation of the bone around its `z` axis. This is used to
 * determine in which direction the `x` and `y` axis of the bone local space
 * will be pointing in.
 * 
 * @see tram::Render::Animation
 * @see https://racenis.github.io/tram-sdk/documentation/render/animation.html
 */

using namespace tram;

template <> Pool<Render::Animation> PoolProxy<Render::Animation>::pool ("animation pool", 50, false);
template <> Pool<Render::Pose> PoolProxy<Render::Pose>::pool ("pose pool", 100, false);

namespace tram::Render {
    
Hashmap<Animation*> animation_list ("model name list", 500);

// this is dumb, we should not do this!!!!
// TODO: fix
StackPool<uint8_t> animation_pool ("animation keyframe pool", 1024 * 1024);

void Animation::LoadFromDisk() {
    char filename [100] = "data/animations/";
    strcat (filename, name);
    strcat (filename, ".anim");

    File file (filename, File::READ);

    if (!file.is_open()) {
        Log(Severity::WARNING, System::RENDER, "Can't open {}!", filename);
        return;
    }

    Log(Severity::WARNING, System::RENDER, "Loading animation: {}", filename);

    name_t header = file.read_name();
    
    if (header != "ANIMv1") {
        Log("Unrecognized header '{}' in animation {}", header, filename);
        return;
    }
    
    // in the ANIMv1 format, there are 24 time units in a single second
    // in the later formats, a time unit is a single second
    const float time_scale = header == "ANIMv1" ? 1.0f / 24.0f : 1.0f;
    
    // the older format also has a superfluous 
    if (header == "ANIMv1") file.read_name();
    
    header_count = file.read_uint32();
    
    const size_t header_size = header_count * sizeof(KeyframeHeader);
    headers = (KeyframeHeader*)malloc(header_size);
    
    uint32_t keyframe_count = 0;
    uint32_t keyframe_offset = 0;
    for (uint32_t j = 0; j < header_count; j++){
        
        keyframe_offset = keyframe_count;
        
        headers[j].bone = file.read_name();
        headers[j].keyframe_count = file.read_uint32();
        headers[j].keyframe_offset = keyframe_offset;
        
        keyframe_count += headers[j].keyframe_count;
        
        keyframes = (Keyframe*)realloc(keyframes, keyframe_count * sizeof(Keyframe));
        
        for (uint64_t k = 0; k < headers[j].keyframe_count; k++){
            Keyframe* kframe = &keyframes[k + keyframe_offset];
            
            kframe->frame = file.read_float32() * time_scale;
            kframe->location = {file.read_float32(), file.read_float32(), file.read_float32()};
            // glm stores quaternions in wxyz, but the animation format stores
            // them in xyzw like blender, so we can't use the initializer list
            kframe->rotation.x = file.read_float32();
            kframe->rotation.y = file.read_float32();
            kframe->rotation.z = file.read_float32();
            kframe->rotation.w = file.read_float32();
            kframe->scale = {file.read_float32(), file.read_float32(), file.read_float32()};
        }
    }
    
    status = Resource::READY;
}

void Animation::Unload() {

}

Keyframe* Animation::GetKeyframes(name_t bone) {
    for (uint32_t i = 0; i < header_count; i++) {
        if (headers[i].bone != bone) continue;
        
        return &keyframes[headers[i].keyframe_offset];
    }
    
    return nullptr;
}

uint32_t Animation::GetKeyframeCount(name_t bone) {
    for (uint32_t i = 0; i < header_count; i++) {
        if (headers[i].bone != bone) continue;
        
        return headers[i].keyframe_count;
    }
    
    return 0;
}

Animation* Animation::Find (name_t name) {
    Animation* animation = animation_list.Find(name);
    
    if (!animation) {
        animation = PoolProxy<Animation>::New(name);
        animation_list.Insert(UID(name), animation);
    }
    
    return animation;
}
    
}

