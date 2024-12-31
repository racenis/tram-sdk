// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_ARMATURE_H
#define TRAM_SDK_ARMATURE_H

#include <unordered_map>

#include <framework/core.h>
#include <framework/uid.h>
#include <framework/resource.h>

#include <framework/math.h>

namespace tram::Render {
    
typedef std::pair<name_t, uint64_t> NameCount; // animation header
const size_t BONE_COUNT = 30; // bone count in a pose

struct Bone {
    name_t name;
    int32_t parent = -1;
    vec3 head = {0.0f, 0.0f, 0.0f};
    vec3 tail = {1.0f, 1.0f, 1.0f};
    float roll = 0.0f;
};

struct Keyframe {
    float frame = 0.0f;
    vec3 location = {0.0f, 0.0f, 0.0f};
    quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    vec3 scale = {1.0f, 1.0f, 1.0f};
    char padding[4]; // for emscripten ??
};

struct Pose {
    mat4 pose[BONE_COUNT];
};

struct KeyframeHeader {
    name_t bone;
    uint32_t keyframe_offset;
    uint32_t keyframe_count;
};

class Animation : public Resource {
public:
    Animation(name_t name) : Resource(name) {}

    Keyframe* GetKeyframes(name_t bone);
    uint32_t GetKeyframeCount(name_t bone);
    
    inline KeyframeHeader* GetHeader(uint32_t index) { return &headers[index]; }
    inline uint32_t GetHeaderCount() { return header_count; }
    
    void LoadFromDisk();
    void LoadFromMemory() {}
    
    void Unload();
    
    static Animation* Find(name_t name);
    static void LoadAll();
private:
    Keyframe* keyframes = nullptr;
    
    KeyframeHeader* headers = nullptr;
    uint32_t header_count = 0;
};

}

#endif // TRAM_SDK_ARMATURE_H
