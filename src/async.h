// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// ASYNC.H -- All of the multithreaded code
// Asynchronous resource loading

#include <core.h>

namespace Core::Async {

    /// Loads a resource and then notifies the EntityComponent.
    void RequestResource (EntityComponent* requester, Resource* requested_resource);

    /// Bypasses the first resource loading queue and loads the resource immediately.
    void ForceLoadResource(Resource* res);

    /// Starts up the async threads.
    void Init();

    /// Stops the async threads.
    void Yeet();

    /// Processes the second resource queue.
    /// @note This should only be called from the rendering thread.
    void ResourceLoader2ndStage();
    
    /// Notifies the EntityComponents that requested the resources.
    void FinishResource();

}
