// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/entitycomponent.h>

/**
 * @class tram::EntityComponent framework/entitycomponent.h
 * 
 * Component base class.
 * 
 * Usually the way that components work is that a user creates a component, i.e.
 * constructs an instance of a component, then they call setter methods on it to
 * feed it parameters and finally they call the Init() method in the comoponent.
 * 
 * In turn, the base component class checks whether all of the component's
 * requested resources have been loaded and then calls the component's Start()
 * method.
 * 
 * @fn tram::EntityComponent::IsReady
 * Returns the component's readiness.
 * The component is considered ready when it's Init() method has been called,
 * all of it's Resource dependencies have been loaded and its Start() method has
 * been called.
 * @return True if the component's Start() method has been called.
 * 
 * @fn tram::EntityComponent::IsInit
 * Returns the component's init status.
 * @return True if the Init() method has been called.
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