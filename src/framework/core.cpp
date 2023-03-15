// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <framework/event.h>
#include <framework/system.h>
#include <framework/uid.h>

#ifndef ENGINE_EDITOR_MODE
#include <framework/ui.h>
#endif

#include <iostream>
#include <cassert>

namespace tram {
    /// Engine version string.
    /// If you fork the engine, you should probably change this.
    char const* ENGINE_VERSION = "Tramway SDK v0.0.4-alpha";
    
    /// If set to true, then the main loop should stop and animation should exit.
    bool EXIT = false;
    
    float TIME = 0.0f;
    
    uint32_t TICK = 0;

    /// Generates a unique ID number.
    uint64_t GenerateID () {
        static uint64_t num = 0;
        num++;
        return num;
    }

    /// Initializes the core of the framework.
    void Init () {
        // set the 0th string to 'none'
        UID none("none");
        
        
        // these are all of the default systems, as of now.
        // feel free to extend the list, but do note that this list
        // is in the same order as the System::System enumeration.
        System::Register("Core", "CORE");
        System::Register("User Interface", "UI");
        System::Register("Graphical User Interface", "GUI");
        System::Register("Async", "ASYNC");
        System::Register("Rendering", "RENDER");
        System::Register("Physics", "PHYSICS");
        System::Register("Audio", "AUDIO");
        System::Register("Misc", "MISC");
        
        // this will register all of the default events
        for (event_t i = 0; i < Event::LAST_EVENT; i++) {
            Event::Register();
        }
        
        assert(!System::IsInitialized(System::SYSTEM_CORE));
        
        System::SetInitialized(System::SYSTEM_CORE, true);
        
        std::cout << ENGINE_VERSION << std::endl;
    }
    
    /// Updates the core system.
    /// @note This should be called only once per update cycle.
    void Update () {
        #ifndef ENGINE_EDITOR_MODE
        TIME = UI::GetTime();
        #endif
        
        TICK++;
        
        for (System::system_t i = 0; i < System::GetSystemCount(); i++) {
            System::SetUpdated(i, false);
        }
    }
    
    /// Returns the current tick.
    /// This value gets incremented once per update cycle.
    /// @return Count of update cycles since the application was started.
    uint32_t GetTick() {
        return TICK;
    }
    
    /// Returns the time at the start of the current tick.
    /// This value changes only once per update cycle.
    /// @return Time, in seconds, since the application was started.
    float GetTickTime() {
        return TIME;
    }
}


