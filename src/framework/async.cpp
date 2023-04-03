// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <templates/queue.h>

#include <framework/core.h>
#include <framework/async.h>
#include <framework/system.h>
#include <framework/resource.h>
#include <framework/entitycomponent.h>

#include <thread>
#include <vector>

namespace tram::Async {

static std::vector<std::thread> resource_loaders;
static bool loaders_should_stop = false;

struct ResourceRequest {
    EntityComponent* requester;
    uint64_t requester_id;
    Resource* requested_res;
};

static Queue<ResourceRequest> disk_loader_queue ("resource request queue", 500);
static Queue<ResourceRequest> memory_loader_queue ("resource request queue 2nd stage", 500);
static Queue<ResourceRequest> finished_queue ("finished resource request queue", 500);

/// Adds a resource to the loading queue.
/// @param requester EntityComponent that will be notified when the resource is loaded.
/// Can be set to nullptr, in which case nothing will be notified.
/// @param requested_resource The resource that will be loaded.
void RequestResource (EntityComponent* requester, Resource* requested_resource) {
    disk_loader_queue.push(ResourceRequest {
        .requester = requester,
        .requester_id = requester ? requester->GetID() : 0,
        .requested_res = requested_resource
    });
}

/// Resource loading function.
/// Should only be used by through the Async::Init() function.
void ResourceLoader() {
    while (!loaders_should_stop) {
        ResourceLoader1stStage();
        
        disk_loader_queue.lock();
        auto len = disk_loader_queue.size();
        disk_loader_queue.unlock();
        
        if (len > 0) {
            continue;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

/// Porcesses the first resource queue.
/// If you started the Async system with at least one thread, you don't
/// need to call this function.
void ResourceLoader1stStage() {        
    if (ResourceRequest req; disk_loader_queue.try_pop(req)) {
        if (req.requested_res->GetStatus() == Resource::UNLOADED) {
            req.requested_res->LoadFromDisk();
        }

        if (req.requested_res->GetStatus() == Resource::READY) {
            finished_queue.push(req);
        } else {
            memory_loader_queue.push(req);
        }
    }
}

/// Processes the second resource queue.
/// @warning This function should only be called from the rendering thread.
void ResourceLoader2ndStage() {
    ResourceRequest req;

    while (memory_loader_queue.try_pop(req)) {
        if (req.requested_res->GetStatus() == Resource::LOADED) {
            req.requested_res->LoadFromMemory();
        }

        finished_queue.push(req);
    }

}

/// Loads a resource from disk, skipping the queue.
/// Shouldn't be used outside of resource LoadFromDisk() methods. 
void ForceLoadResource (Resource* res) {
    if (res->GetStatus() == Resource::UNLOADED) {
        res->LoadFromDisk();
    }

    ResourceRequest req {
        .requester = nullptr,
        .requester_id = 0,
        .requested_res = res
    };

    if (res->GetStatus() == Resource::READY) {
        finished_queue.push(req);
    } else {
        memory_loader_queue.push(req);
    }
}

/// Notifies EntityComponents about finished resources.
void FinishResource () {
    ResourceRequest req;

    while (finished_queue.try_pop(req)) {
        // TODO: check if requester pointer is valid
        // TODO: check if requester ids match up
        if (req.requester) {
            req.requester->ResourceReady();
        }
    }
}

/// Starts the async resource loader thread.
/// @param Number of threads for async loading.
void Init (size_t threads) {
    assert(System::IsInitialized(System::SYSTEM_CORE));
    
    if (threads) {
        loaders_should_stop = false;
        
        for (size_t i; i < threads; i++) {
            resource_loaders.push_back (std::thread(ResourceLoader));
        }
    }
    
    System::SetInitialized(System::SYSTEM_ASYNC, true);
}

/// Stops the async resource loader thread.
void Yeet () {
    assert(System::IsInitialized(System::SYSTEM_ASYNC));
    
    loaders_should_stop = true;
    
    for (auto& loader : resource_loaders) {
        loader.join();
    }
    
    System::SetInitialized(System::SYSTEM_ASYNC, false);
}

/// Returns number of resources in queues.
size_t GetWaitingResources () {
    size_t sum = 0;
    disk_loader_queue.lock();
    memory_loader_queue.lock();
    finished_queue.lock();
    sum += disk_loader_queue.size();
    sum += memory_loader_queue.size();
    sum += finished_queue.size();
    disk_loader_queue.unlock();
    memory_loader_queue.unlock();
    finished_queue.unlock();
    return sum;
}

}
