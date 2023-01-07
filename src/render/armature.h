// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// ARMATURE.H -- Animation stuff
// Animation bone, keyframe, animation resource object definitions.

#ifndef ARMATURE_H
#define ARMATURE_H

#include <unordered_map>

#include <templates/pool.h>
#include <templates/stackpool.h>

#include <framework/core.h>
#include <framework/uid.h>
#include <framework/resource.h>

#include <framework/math.h>

namespace Core::Render {
    typedef std::pair<name_t, uint64_t> NameCount; // animation header
    const size_t BONE_COUNT = 30;

    struct Bone {
        name_t name = 0;
        uint32_t parentIndex = -1;  // why is this name using pascalCase
        vec3 head = vec3(1.0f);
        vec3 tail = vec3(1.0f);
        float roll = 0.0f;
    };

    struct Keyframe {
        float frame;
        vec3 location = vec3(0.0f);
        quat rotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
        vec3 scale = vec3(1.0f);
    };
    
    struct Pose {
        mat4 pose[BONE_COUNT];
    };

    extern Pool<Pose> poseList;

    class Animation : public Resource {
    public:
        Animation(name_t name) : Resource(name) {}

        static NameCount* Find(name_t anim_name);

        void LoadFromDisk();
        void LoadFromMemory(){}
        static void LoadAll();
    };

    void UpdateArmatures();

}

#endif // ARMATURE_H
