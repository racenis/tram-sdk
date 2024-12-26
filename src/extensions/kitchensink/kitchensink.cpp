// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/kitchensink/kitchensink.h>

#include <framework/system.h>

#include <assert.h>

/**
 * @namespace tram::Ext::Kitchensink
 * 
 * Extension containing random stuff that doesn't fit in anywhere else.
 * 
 * Featuring a collection (mostly) unfinished re-usable game mechanics for RPG
 * games, as well as a couple of useful data tools and entities.
 * 
 * This is where you can find out func_door analog too.
 */

namespace tram::Ext::Kitchensink {

uint32_t KITCHENSINK_SYSTEM = -1u;

void Init() {
    assert(System::IsInitialized(System::SYSTEM_RENDER) && "Render system needs to be initialized first!");
    assert(System::IsInitialized(System::SYSTEM_AUDIO) && "Audio system needs to be initialized first!");
    
    // TODO: add physics check and other checks
    assert(KITCHENSINK_SYSTEM == -1u && "Camera system is already initialized!");
    
    // here we register all the entities and stuff
    
    KITCHENSINK_SYSTEM = System::Register("Random mechanics and stuff", "KITCHN");
    System::SetInitialized(KITCHENSINK_SYSTEM, true);
}

void Update() {
    assert(System::IsInitialized(KITCHENSINK_SYSTEM) && "Kitchensink system needs to be initialized first!");
}

}
