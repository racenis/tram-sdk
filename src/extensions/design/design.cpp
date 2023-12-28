// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <extensions/design/design.h>

#include <framework/system.h>

#include <assert.h>

namespace tram::Ext::Design {

uint32_t DESIGN_SYSTEM = -1u;

void Init() {
    assert(System::IsInitialized(System::SYSTEM_RENDER) && "Render system needs to be initialized first!");
    assert(System::IsInitialized(System::SYSTEM_AUDIO) && "Audio system needs to be initialized first!");
    
    // TODO: add physics check and other checks
    assert(DESIGN_SYSTEM == -1u && "Camera system is already initialized!");
    
    // here we register all the entities and stuff
    
    DESIGN_SYSTEM = System::Register("Random mechanics and stuff", "DESIGN");
    System::SetInitialized(DESIGN_SYSTEM, true);
}

void Update() {
    assert(System::IsInitialized(DESIGN_SYSTEM) && "Design system needs to be initialized first!");
}

}
