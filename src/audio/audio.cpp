// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <audio/audio.h>
#include <audio/sound.h>

#include <framework/system.h>
#include <framework/logging.h>

#include <components/audio.h>
#include <unordered_map>

#include <audio/api.h>

/**
 * @file audio/audio.cpp
 * 
 * Main Audio system interface implementation.
 */

namespace tram::Audio {

/// Starts the Audio system.
void Init() {
    assert(System::IsInitialized(System::SYSTEM_CORE));
    assert(System::IsInitialized(System::SYSTEM_UI));
    // TODO: check if we actually need UI for this
    
    API::Init();
    
    System::SetInitialized(System::SYSTEM_AUDIO, true);
}

/// Updates the Audio system.
void Update() {        
    API::Update();
}

// Stops the Audio system.
void Uninit() {
    
    // we need to yeet all of the resources, otherwise OpenAL will complain
    for (auto& it : PoolProxy<AudioComponent>::GetPool()) it.~AudioComponent();
    for (auto& it : PoolProxy<Sound>::GetPool()) it.Unload();
    
    API::Uninit();
}

// TODO: make volume a setting that can be.. well used as a setting
static float volume = 1.0f;
void SetVolume(float value) {
    volume = value;
    API::SetListenerGain(value);
}

float GetVolume() {
    return volume;
}

/// See Audio::API::SetListenerPosition().
void SetListenerPosition(vec3 position) {
    API::SetListenerPosition(position);
}

/// See Audio::API::SetListenerOrientation().
void SetListenerOrientation(quat orientation) {
    API::SetListenerOrientation(orientation);
}

}

