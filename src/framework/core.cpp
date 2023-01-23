// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <framework/event.h>
#include <framework/system.h>
#include <framework/uid.h>

#include <iostream>

namespace Core {
    /// Engine version string.
    /// If you fork the engine, you should probably change this.
    char const* ENGINE_VERSION = "Tramway SDK v0.0.4-alpha";
    
    /// If set to true, then the main loop should stop and animation should exit.
    bool EXIT = false;
    
    /// Time since program started, in seconds.
    /// Updated every time the UI::Update() is called. Should be once a frame.
    float TIME = 0.0f;
    
    /// Count of screen updates.
    /// Incremented by one every time the UI::Update() is called. Should be once a frame.
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
        
        System::SetInitialized(System::SYSTEM_CORE, true);
        
        std::cout << ENGINE_VERSION << std::endl;
    }
}


