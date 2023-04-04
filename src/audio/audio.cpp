// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <audio/audio.h>
#include <audio/sound.h>

#include <framework/system.h>
#include <framework/logging.h>

#include <components/audiocomponent.h>
#include <unordered_map>

#include <audio/openal/openal.h>


namespace tram::Audio {

/// Starts the Audio system.
void Init() {
    assert(System::IsInitialized(System::SYSTEM_CORE));
    assert(System::IsInitialized(System::SYSTEM_UI));
    
    OpenAL::Init();
    
    System::SetInitialized(System::SYSTEM_AUDIO, true);
}

/// Updates the Audio system.
void Update() {        
    OpenAL::Update();
}

// Stops the Audio system.
void Uninit() {
    for (auto& it : PoolProxy<AudioComponent>::GetPool()) it.~AudioComponent();
    for (auto& it : PoolProxy<Sound>::GetPool()) it.Unload();
    
    OpenAL::Uninit();
}

}

