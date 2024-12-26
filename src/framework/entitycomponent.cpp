// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/entitycomponent.h>

/**
 * @class tram::EntityComponent framework/entitycomponent.h
 * 
 * Component base class.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/entitycomponent.html
 */
 
 /* TODO:
  * I don't really like how the Component<> template works. for example, what
  * happens when a component needs to be allocated with some mechanism other
  * than a Pool<>?
  * 
  * we could try the following:
  * - remove make() and clear() definitions from the template
  * - implement the make() and clear() in the implementation files for every
  *   component type
  * - maybe make a macro to do the implementation automatically
  */

namespace tram {

EntityComponent::EntityComponent() {

}

EntityComponent::~EntityComponent() {

}

}