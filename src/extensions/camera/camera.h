// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_CAMERA_CAMERA_H
#define TRAM_SDK_EXTENSIONS_CAMERA_CAMERA_H

#include <framework/math.h>

namespace tram {
    class Entity;
}

namespace tram::Ext::Camera {

class Camera {
public:
    void Update ();
    
    void SetTilt(float tilt);
    void SetMouselook(bool mouselook);
    void SetRotateFollowing(bool rotate_following);
    void SetBobbing(float bobbing_weight);
    void SetBobbingDistance(float bobbing_distance);
    void SetBobbingCallback(void (*bob_callback)(Camera*));
    
    inline void SetLocation(vec3 location) { this->location = location; }
    inline void SetRotation(quat rotation) { this->rotation = rotation; }
    
    inline vec3 GetLocation() { return location; }
    inline quat GetRotation() { return rotation; }
    
    inline void SetFollowing(Entity* following) { this->following = following; }
    inline void SetFollowingOffset(vec3 offset) { this->following_offset = offset; }
    
protected:
    float tilt = 0.0f;
    float tilt_goal = 0.0f;
    float tilt_last = 0.0f;
    float bob = 0.0f;
    float bobbing_weight_goal = 0.0f;
    float bobbing_weight = 0.0f;
    float bobbing_distance = 0.2f;
    void (*bob_callback)(Camera*) = nullptr;
    
    bool mouselook = false;
    bool rotate_following = false;
    float yaw = 0.0f;
    float pitch = 0.0f;
    
    vec3 location = vec3 (0.0f, 0.0f, 0.0f);
    quat rotation = quat (vec3 (0.0f, 0.0f, 0.0f));
    
    vec3 following_offset = vec3 (0.0f, 0.0f, 0.0f);
    Entity* following = nullptr;
};

void SetCamera (Camera* camera);
void Init();
void Update();

}

#endif // TRAM_SDK_EXTENSIONS_CAMERA_CAMERA_H