// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// CORE.CPP -- Core functions and globals.

#include <framework/core.h>
#include <framework/event.h>
#include <framework/system.h>

#include <render/render.h>
#include <render/armature.h>

#include <framework/ui.h>

#include <cstring>
#include <fstream>

namespace Core {
    char const* ENGINE_VERSION = "Tramway SDK v0.0.4-alpha";
    
    bool EXIT = false;
    float TIME = 0.0f;

    // TODO: these should be moved into their own file
    

    void Init(){
        // set the 0th string to 'none'
        UID none("none");
        
        
        // these are all of the default systems, as of now.
        // feel free to extend the list, but do note that this list
        // is in the same order as the System::System enumeration.
        System::Register("Core", "CORE");
        System::Register("User Interface", "UI");
        System::Register("Async", "ASYNC");
        System::Register("Rendering", "RENDER");
        System::Register("Physics", "PHYSICS");
        System::Register("Audio", "AUDIO");
        System::Register("Misc", "MISC");
        
        // this will register all of the default events
        for (event_t i = 0; i < Event::LAST_EVENT; i++) {
            Event::Register();
        }
    }




}


