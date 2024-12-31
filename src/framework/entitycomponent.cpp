// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/entitycomponent.h>

#include <framework/logging.h>

/**
 * @class tram::EntityComponent framework/entitycomponent.h <framework/entitycomponent.h>
 * 
 * Component base class.
 * 
 * Usually the way that components work is that a user creates a component, i.e.
 * constructs an instance of a component, then they call setter methods on it to
 * feed it parameters and finally they call the Init() method in the component.
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
 
namespace tram {

/// Initializes an entity component.
/// If an entity component has all of its resources already loaded, this will
/// immediately Start() the component.
void EntityComponent::Init() {
    if (is_init) {
        Log(Severity::WARNING, System::CORE, "Called Init() multiple times on a {}", typeid(*this).name());
        return;
    }
    
    is_init = true;
    
    if (!resources_waiting) {
         Start();
    }
}

/// Notifies the component of a streamed-in resource.
void EntityComponent::ResourceReady() {
    if (!resources_waiting) {
        Log(Severity::WARNING, System::CORE, "ResourceReady() called more than there are resources_waiting. This shouldn't happen.");
        return;
    }
    
    resources_waiting--;
    
    if (!resources_waiting && is_init) {
        Start();
    }
}

}