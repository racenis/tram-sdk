// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <extensions/camera/camera.h>
#include <framework/system.h>
#include <framework/entity.h>
#include <framework/ui.h>
#include <render/render.h>
#include <audio/audio.h>
#include <components/controller.h>

namespace tram::Ext::Camera {

uint32_t CAMERA_SYSTEM = -1u;
Camera* selected_camera = nullptr;

const float TILT_SPEED = 0.01f;
//const float BOB_SPEED = 0.2f;
const float BOB_CHANGE_SPEED = 0.05f;

id_t look_at_entity = 0;
quat look_at_direction = {1.0f, 0.0f, 0.0f, 0.0f};

void Init() {
    assert(System::IsInitialized(System::SYSTEM_RENDER) && "Render system needs to be initialized first!");
    assert(System::IsInitialized(System::SYSTEM_AUDIO) && "Audio system needs to be initialized first!");
    assert(CAMERA_SYSTEM == -1u && "Camera system is already initialized!");
    
    CAMERA_SYSTEM = System::Register("Camera control system", "CAMERA");
    System::SetInitialized(CAMERA_SYSTEM, true);
    
    Event::AddListener(Event::LOOK_AT, [](Event& event) {
        if (event.poster_id != look_at_entity) return;
        look_at_direction = (quat)*(Value*)event.data;
    });
}

void Update() {
    assert(System::IsInitialized(CAMERA_SYSTEM) && "Camera system needs to be initialized first!");
    if (selected_camera) selected_camera->Update();
}

void SetCamera (Camera* camera) {
    selected_camera = camera;
}

void Camera::SetTilt (float tilt) {
    this->tilt_goal = tilt;
}

void Camera::SetMouselook (bool mouselook) {
    this->mouselook = mouselook;
}

void Camera::SetRotateFollowing (bool rotate_following) {
    this->rotate_following = rotate_following;
}

void Camera::SetBobbing (float bobbing_weight) {
    this->bobbing_weight_goal = bobbing_weight;
}

void Camera::SetBobbingDistance (float bobbing_distance) {
    this->bobbing_distance = bobbing_distance;
}

void Camera::SetBobbingTilt(float bobbing_tilt) {
    this->bobbing_tilt_goal = bobbing_tilt;
}

void Camera::SetBobSpeed(float bob_speed) {
    this->bob_speed = bob_speed;
}

void Camera::SetBobbingCallback(void (*bob_callback)(Camera*)) {
    this->bob_callback = bob_callback;
}

float Camera::GetBob() {
    return this->bob;
}

float Camera::GetWeight() {
    return this->bobbing_weight;
}

void Camera::Update () {
    if (UI::GetInputState() == UI::STATE_FLYING) return;
    
    // move tilt towards tilt goal
    if (abs(tilt_goal - tilt) <= TILT_SPEED * GetDeltaTime() * 60.0f && abs(tilt_goal) <= TILT_SPEED * GetDeltaTime() * 60.0f) {
        tilt = 0.0f;
    } else if (tilt_goal >= 0.0f && tilt < tilt_goal) {
        tilt += TILT_SPEED * GetDeltaTime() * 60.0f;
    } else if (tilt_goal <= 0.0f && tilt > tilt_goal) {
        tilt -= TILT_SPEED * GetDeltaTime() * 60.0f;
    }
    
    // do some bobbing
    if (fabsf(bobbing_weight_goal - bobbing_weight) <= BOB_CHANGE_SPEED * GetDeltaTime() * 60.0f) {
        bobbing_weight = bobbing_weight_goal;
    } else if (bobbing_weight > bobbing_weight_goal) {
        bobbing_weight -= BOB_CHANGE_SPEED * GetDeltaTime() * 60.0f;
    } else if (bobbing_weight < bobbing_weight_goal) {
        bobbing_weight += BOB_CHANGE_SPEED * GetDeltaTime() * 60.0f;
    }
    
    if (fabsf(bobbing_tilt - bobbing_tilt_goal) <= TILT_SPEED * GetDeltaTime() * 60.0f) {
        bobbing_tilt = bobbing_tilt_goal;
    } else if (bobbing_tilt > bobbing_tilt_goal) {
        bobbing_tilt -= TILT_SPEED * GetDeltaTime() * 60.0f;
    } else if (bobbing_tilt < bobbing_tilt_goal) {
        bobbing_tilt += TILT_SPEED * GetDeltaTime() * 60.0f;
    }
    
    // process the bobbing
    if (bobbing_weight > 0.0f) {
        bob += bob_speed * GetDeltaTime() * 60.0f;
        
        // first callback
        if (bob > glm::pi<float>() && callback_count % 2 == 0) {
            if (bob_callback) bob_callback(this);
            callback_count++;
        }
        
        // second callback
        if (bob >= 2.0f*glm::pi<float>()) {
            bob = fmodf(bob, 2.0f*glm::pi<float>());
            if (bob_callback) bob_callback(this);
            callback_count++;
        }
    }
    
    // update position to entity that is being followed
    if (following) {
        location = following->GetLocation();
        //rotation = following->Query(QUERY_LOOK_DIRECTION);
        rotation = look_at_direction;
        look_at_entity = following->GetID();
    }
    
    // putting everything together
    vec3 term_loc = location;
    quat term_rot = rotation;
    
    term_rot *= quat(vec3(0.0f, 0.0f, tilt + (sinf(bob) * bobbing_tilt * bobbing_weight)));
    
    term_loc += following_offset;
    term_loc += vec3(0.0f, 1.0f, 0.0f) * fabsf(sinf(bob)) * bobbing_distance * bobbing_weight;
    
    if (following_interpolation != 1.0f) {
        term_loc = glm::mix(Render::GetViewPosition(), term_loc, following_interpolation);
    }
    
    Render::SetViewPosition(term_loc, 0);
    Render::SetViewRotation(term_rot, 0);
    
    Audio::SetListenerPosition(term_loc);
    Audio::SetListenerOrientation(term_rot);
}

}
