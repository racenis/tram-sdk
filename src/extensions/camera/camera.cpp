// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/camera/camera.h>
#include <framework/system.h>
#include <framework/entity.h>
#include <framework/ui.h>
#include <framework/settings.h>
#include <render/render.h>
#include <audio/audio.h>
#include <components/controller.h>

/**
 * @namespace tram::Ext::Camera
 * 
 * Several different camera implementations.
 */

/**
 * @class tram::Ext::Camera::Camera extensions/camera/camera.h
 * 
 * Interface API for cameras.
 */

namespace tram::Ext::Camera {

// TODO: switch to system_t??
uint32_t CAMERA_SYSTEM = -1u;

// see? this here is a problem. we have only a single selected camera... but
// cameras can use a limited set of layers.
// which means that we could have a camera for layer 0 and a different active
// camera for layer 1. this current setup does not support it, and frankly, I
// find is completely unacceptable!!
// this must be remedied at the earliest convenience..
Camera* selected_camera = nullptr;

float shake_multiplier = 1.0f;

id_t look_at_entity = 0;
quat look_at_direction = {1.0f, 0.0f, 0.0f, 0.0f};

void Init() {
    assert(System::IsInitialized(System::RENDER) && "Render system needs to be initialized first!");
    assert(System::IsInitialized(System::AUDIO) && "Audio system needs to be initialized first!");
    assert(CAMERA_SYSTEM == -1u && "Camera system is already initialized!");
    
    Settings::Register(shake_multiplier, "camerashake", Settings::SERIALIZE_NONE);
    
    CAMERA_SYSTEM = System::Register("Camera control system", "CAMERA");
    System::SetInitialized(CAMERA_SYSTEM, true);
}

void Update() {
    assert(System::IsInitialized(CAMERA_SYSTEM) && "Camera system needs to be initialized first!");
    if (selected_camera) selected_camera->Update();
}

void Camera::Update() {

}

Camera::~Camera() {
    if (this == selected_camera) selected_camera = nullptr;
}

void Camera::SetViewTransform() {
    for (uint32_t i = 0; i < 8; i++) {
        if (layer_mask & (1 << i)) {
            Render::SetViewPosition(position, i);
            Render::SetViewRotation(rotation, i);
        }
    }
}

void Camera::SetListenerTransform() {
    Audio::SetListenerPosition(position);
    Audio::SetListenerOrientation(rotation);
}

void Camera::SetActive() {
    selected_camera = this;
}

void Camera::SetFollowing(Entity* entity) {
    following = entity->GetID();
    
    following_listener = Event::AddListener(Event::LOOK_AT, this, [](Event& event, void* data) {
        Camera* camera = (Camera*)data;
        
        if (event.poster != camera->GetFollowing()) return;
        
        camera->SetFollowingLookat((quat)*(Value*)event.data);
    });
}

id_t Camera::GetFollowing() {
    return following;
}

void Camera::SetFollowingLookat(quat new_lookat) {
    following_lookat = new_lookat;
}

}
