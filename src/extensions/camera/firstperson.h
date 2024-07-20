// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_CAMERA_FIRSTPERSON_H
#define TRAM_SDK_EXTENSIONS_CAMERA_FIRSTPERSON_H

#include <extensions/camera/camera.h>

namespace tram::Ext::Camera {

class FirstPersonCamera : public Camera {
public:
    FirstPersonCamera() {}
    ~FirstPersonCamera() {}
    
    void Update ();
    
    void SetTilt(float tilt);
    void SetMouselook(bool mouselook);
    void SetRotateFollowing(bool rotate_following);
    void SetBobbing(float bobbing_weight);
    void SetBobbingDistance(float bobbing_distance);
    void SetBobbingTilt(float bobbing_tilt);
    void SetBobbingCallback(void (*bob_callback)(FirstPersonCamera*));
    void SetBobSpeed(float speed);
    
    float GetBob();
    float GetWeight();
    
    
    
    //inline void SetFollowing(Entity* following) { this->following = following; }
    inline void SetFollowingInterpolation(float following_interpolation) { this->following_interpolation = following_interpolation; }
    inline void SetFollowingOffset(vec3 offset) { this->following_offset = offset; }
    
protected:
    float tilt = 0.0f;
    float tilt_goal = 0.0f;
    float tilt_last = 0.0f;
    float bob = 0.0f;
    float bobbing_weight_goal = 0.0f;
    float bobbing_weight = 0.0f;
    float bobbing_distance = 0.2f;
    float bobbing_tilt_goal = 0.0f;
    float bobbing_tilt = 0.0f;
    float bob_speed = 0.1f;
    int callback_count = 0;
    void (*bob_callback)(FirstPersonCamera*) = nullptr;
    
    bool mouselook = false;
    bool rotate_following = false;
    float yaw = 0.0f;
    float pitch = 0.0f;
    
    vec3 following_offset = vec3 (0.0f, 0.0f, 0.0f);
    float following_interpolation = 1.0f;
    //Entity* following = nullptr;
};

}

#endif // TRAM_SDK_EXTENSIONS_CAMERA_FIRSTPERSON_H