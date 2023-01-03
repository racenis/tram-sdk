// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// ARMATURE.H -- Animation stuff
// Animation bone, keyframe, animation resource object definitions.

#ifndef ARMATURE_H
#define ARMATURE_H

#include <framework/core.h>

namespace Core::Render {
    typedef std::pair<name_t, uint64_t> NameCount; // animation header
    const size_t BONE_COUNT = 30;

    struct Bone {
        name_t name = 0;
        uint32_t parentIndex = -1;
        glm::vec3 head = glm::vec3(1.0f);
        glm::vec3 tail = glm::vec3(1.0f);
        float roll = 0.0f;
    };

    struct Keyframe {
        float frame;
        glm::vec3 location = glm::vec3(0.0f);
        glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f);
    };
    
    struct Pose {
        glm::mat4 pose[BONE_COUNT];
    };

    extern Pool<Pose> poseList;

    class Animation : public Resource {
    protected:
        static StackPool<uint8_t> animationpool;

        static std::unordered_map<uint64_t, NameCount*> animationlist;
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
