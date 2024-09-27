// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_CAMERA_CAMERA_H
#define TRAM_SDK_EXTENSIONS_CAMERA_CAMERA_H

#include <framework/math.h>
#include <framework/core.h>
#include <framework/event.h>

namespace tram::Ext::Camera {

class Camera {
public:
    void SetActive();
    void SetFollowing(Entity*);
    id_t GetFollowing();
    
    inline void SetPosition(vec3 position) { this->position = position; }
    inline void SetRotation(quat rotation) { this->rotation = rotation; }
    
    inline vec3 GetPosition() { return position; }
    inline quat GetRotation() { return rotation; }
    
    void SetViewTransform();
    void SetListenerTransform();
    
    void SetFollowingLookat(quat new_lookat);
    
    void SetLayerMask(uint32_t layer_mask) { this->layer_mask = layer_mask; }
    
    virtual void Update() = 0;
    virtual ~Camera();
protected:
    vec3 position = {0.0f, 0.0f, 0.0f};
    quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};

    listener_t following_listener = 0;
    
    uint32_t layer_mask = 1 + 2;
    
    id_t following = 0;
    quat following_lookat = {1.0f, 0.0f, 0.0f, 0.0f};
};

void Init();
void Update();

}

#endif // TRAM_SDK_EXTENSIONS_CAMERA_CAMERA_H