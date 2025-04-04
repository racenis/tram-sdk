// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/camera/firstperson.h>

#include <framework/ui.h>
#include <framework/entity.h>
#include <audio/audio.h>

/**
 * @class tram::Ext::Camera::Camera extensions/camera/camera.h <extensions/camera/camera.h>
 * 
 * First person camera.
 * 
 * This camera will look in the same direction that an entity is looking at,
 * while also following it at a set distance.
 * 
 * Supports view bobbing
 */

namespace tram::Ext::Camera {
    
const float TILT_SPEED = 0.01f;
//const float BOB_SPEED = 0.2f;
const float BOB_CHANGE_SPEED = 0.05f;

extern float shake_multiplier;
    
void FirstPersonCamera::SetTilt(float tilt) {
    this->tilt_goal = tilt;
}

void FirstPersonCamera::SetMouselook(bool mouselook) {
    this->mouselook = mouselook;
}

void FirstPersonCamera::SetRotateFollowing(bool rotate_following) {
    this->rotate_following = rotate_following;
}

/// Sets the weight of the bobbing.
/// The larger the weight, the more exaggerated the bobbing will be. This
/// parameter can be used to change the ammount of bobbing for different
/// activities, e.g. 1.0f for walking, 2.0f for running and 0.0f for standing.
void FirstPersonCamera::SetBobbing(float bobbing_weight) {
    this->bobbing_weight_goal = bobbing_weight;
}

/// Sets the distance bobbed during bobbing.
/// @param bobbing_distance  Best if set to between 0.0f and 0.5f.
void FirstPersonCamera::SetBobbingDistance(float bobbing_distance) {
    this->bobbing_distance = bobbing_distance;
}

/// Sets the tilt bobbed during bobbing.
/// The camera will be tilted back and forth along the Z, i.e. forward axis.
/// @note Try not to throw up if you set this value too high.
/// @param bobbing_tilt Tilt, in radians.
void FirstPersonCamera::SetBobbingTilt(float bobbing_tilt) {
    this->bobbing_tilt_goal = bobbing_tilt;
}

/// Sets the speed of the bobs during bobbing.
void FirstPersonCamera::SetBobSpeed(float bob_speed) {
    this->bob_speed = bob_speed;
}

/// Sets the bobbing callback.
/// The bobbing callback is called every time a full view bob is performed. This
/// callback is very effective when used to trigger footstep sound effects.
void FirstPersonCamera::SetBobbingCallback(void (*bob_callback)(FirstPersonCamera*)) {
    this->bob_callback = bob_callback;
}

float FirstPersonCamera::GetBob() {
    return this->bob;
}

float FirstPersonCamera::GetWeight() {
    return this->bobbing_weight;
}

void FirstPersonCamera::Update () {
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
        //location = following->GetLocation();
        position = Entity::Find(following)->GetLocation();
        //rotation = following->Query(QUERY_LOOK_DIRECTION);
        //rotation = look_at_direction;
        rotation = this->following_lookat;
        //look_at_entity = following->GetID();
    }
    
    // putting everything together
    vec3 term_loc = position;
    quat term_rot = rotation;
    
    term_rot *= quat(vec3(0.0f, 0.0f, tilt + (sinf(bob) * bobbing_tilt * bobbing_weight * shake_multiplier)));
    
    term_loc += following_offset;
    term_loc += vec3(0.0f, 1.0f, 0.0f) * fabsf(sinf(bob)) * bobbing_distance * bobbing_weight * shake_multiplier;
    
    if (following_interpolation != 1.0f) {
        term_loc = glm::mix(Render::GetViewPosition(), term_loc, following_interpolation);
    }
    
    position = term_loc;
    rotation = term_rot;
    
    SetViewTransform();
    SetListenerTransform();
    
    //Render::SetViewPosition(term_loc, 0);
    //Render::SetViewRotation(term_rot, 0);
    
    //Audio::SetListenerPosition(term_loc);
    //Audio::SetListenerOrientation(term_rot);
}
    
}