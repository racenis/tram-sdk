// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#include <extensions/kitchensink/design.h>

#include <framework/system.h>

#include <assert.h>

namespace tram::Ext::Design {

uint32_t KITCHENSINK_SYSTEM = -1u;

void Init() {
    assert(System::IsInitialized(System::SYSTEM_RENDER) && "Render system needs to be initialized first!");
    assert(System::IsInitialized(System::SYSTEM_AUDIO) && "Audio system needs to be initialized first!");
    
    // TODO: add physics check and other checks
    assert(KITCHENSINK_SYSTEM == -1u && "Camera system is already initialized!");
    
    // here we register all the entities and stuff
    
    KITCHENSINK_SYSTEM = System::Register("Random mechanics and stuff", "DESIGN");
    System::SetInitialized(KITCHENSINK_SYSTEM, true);
}

void Update() {
    assert(System::IsInitialized(KITCHENSINK_SYSTEM) && "Design system needs to be initialized first!");
}

}
