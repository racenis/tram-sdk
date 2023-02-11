#include <extensions/camera/camera.h>
#include <framework/system.h>
#include <framework/entity.h>
#include <framework/ui.h>
#include <render/render.h>
#include <audio/audio.h>

namespace Core::Ext::Camera {
    uint32_t CAMERA_SYSTEM = -1u;
    Camera* selected_camera = nullptr;
    
    const float TILT_SPEED = 0.01f;
    const float BOB_SPEED = 0.15f;
    const float BOB_CHANGE_SPEED = 0.05f;
    
    float cursorchangex = 0.0f;
    float cursorchangey = 0.0f;
    float cursorx_last = 0.0f;
    float cursory_last = 0.0f;
    
    void Init() {
        assert(System::IsInitialized(System::SYSTEM_RENDER) && "Render system needs to be initialized first!");
        assert(System::IsInitialized(System::SYSTEM_AUDIO) && "Audio system needs to be initialized first!");
        assert(CAMERA_SYSTEM == -1u && "Camera system is already initialized!");
        
        CAMERA_SYSTEM = System::Register("Camera control system", "CAMERA");
        System::SetInitialized(CAMERA_SYSTEM, true);
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

    void Camera::Update () {
        if (UI::INPUT_STATE == UI::STATE_FLYING ||
            UI::INPUT_STATE == UI::STATE_MENU_OPEN
        ) {
            return;
        }
        
        // move tilt towards tilt goal
        if (abs(tilt_goal - tilt) <= TILT_SPEED && abs(tilt_goal) <= TILT_SPEED) {
            tilt = 0.0f;
        } else if (tilt_goal >= 0.0f && tilt < tilt_goal) {
            tilt += TILT_SPEED;
        } else if (tilt_goal <= 0.0f && tilt > tilt_goal) {
            tilt -= TILT_SPEED;
        }
        
        // do some bobbing
        if (abs(bobbing_weight_goal - bobbing_weight) <= BOB_CHANGE_SPEED) {
            bobbing_weight = bobbing_weight_goal;
        } else if (bobbing_weight > bobbing_weight_goal) {
            bobbing_weight -= BOB_CHANGE_SPEED;
        } else if (bobbing_weight < bobbing_weight_goal) {
            bobbing_weight += BOB_CHANGE_SPEED;
        }
        
        if (bobbing_weight > 0.0f) {
            bob += BOB_SPEED;
        }
        
        // retrieve new camera pos
        if (following) {
            location = following->GetLocation();
        }
        
        // retrieve new camera rot
        if (mouselook) {
            cursorchangex = PollKeyboardAxis(UI::KEY_MOUSE_X) - cursorx_last;
            cursorchangey = PollKeyboardAxis(UI::KEY_MOUSE_Y) - cursory_last;
            cursorx_last = PollKeyboardAxis(UI::KEY_MOUSE_X);
            cursory_last = PollKeyboardAxis(UI::KEY_MOUSE_Y);

            yaw += cursorchangex * UI::CAMERA_SENSITIVITY;
            pitch += cursorchangey * UI::CAMERA_SENSITIVITY;
            pitch = pitch > 90.0f ? 90.0f : pitch < -90.0f ? -90.0f : pitch;
            rotation = glm::quat(glm::vec3(-glm::radians(pitch), -glm::radians(yaw), 0.0f));
        }
        
        // putting everything together
        vec3 term_loc = location;
        quat term_rot = rotation;
        
        
        term_rot *= quat(vec3(0.0f, 0.0f, tilt));
        
        term_loc += following_offset;
        term_loc += vec3(0.0f, 1.0f, 0.0f) * (float) sin(bob) * bobbing_distance * bobbing_weight;
        
        Render::CAMERA_POSITION = term_loc;
        Render::CAMERA_ROTATION = term_rot;
        
        Audio::SetListenerPosition(term_loc);
        Audio::SetListenerOrientation(term_rot);
        
        if (rotate_following && following) {
            following->UpdateTransform(following->GetLocation(), quat(vec3(0.0f, -glm::radians(yaw), 0.0f)));
        }
    }
        

    
    
    
    
    
    
    
    
    
    }