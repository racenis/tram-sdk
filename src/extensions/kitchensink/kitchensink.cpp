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
    KITCHENSINK_SYSTEM = System::Register("Random mechanics and stuff", "KITCHN");
    
    System::SetState(KITCHENSINK_SYSTEM, System::INIT);
    
    System::AssertDependency(System::AUDIO);
    System::AssertDependency(System::RENDER);
    System::AssertDependency(System::PHYSICS);
    
    System::SetState(KITCHENSINK_SYSTEM, System::READY);
}

void Update() {
    
}

}
