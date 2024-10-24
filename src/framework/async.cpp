// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <templates/queue.h>
#include <templates/pool.h>

#include <framework/core.h>
#include <framework/async.h>
#include <framework/system.h>
#include <framework/resource.h>
#include <framework/entitycomponent.h>

#include <thread>
#include <vector>

namespace tram::Async {

static std::vector<std::thread>resource_loaders;
static bool loaders_should_stop = false;

struct ResourceRequest {
    EntityComponent* requester;
    Resource* resource;
#ifndef __x86_64__
    void* padding[2];
#endif
};

static Queue<ResourceRequest*> disk_loader_queue("Async::LoadResourcesFromDisk() queue", 500);
static Queue<ResourceRequest*> memory_loader_queue("Async::LoadResourcesFromMemory() queue", 500);
static Queue<ResourceRequest*> finished_queue("Async::FinishResources() queue", 500);

static Pool<ResourceRequest> request_pool("Async::ResourceRequest pool", 750);

/// Adds a resource to the loading queue.
/// @param requester EntityComponent that will be notified when the resource is loaded.
/// Can be set to nullptr, in which case nothing will be notified.
/// @param requested_resource The resource that will be loaded.
void RequestResource(EntityComponent* requester, Resource* resource) {
    assert(System::IsInitialized(System::SYSTEM_ASYNC));
    
    disk_loader_queue.push(request_pool.AddNew(ResourceRequest {
        .requester = requester,
        .resource = resource
    }));
}

/// Cancels a resource load request.
/// Should be called if the EntityComponent that requested a Resource to be
/// loaded will be destructed before the Resource has finished loading.
/// @note The resource will be fully loaded anyway, but the requester will not
///       be notified.
void CancelRequest(EntityComponent* requester, Resource* resource) {
    for (auto& req : request_pool) {
        if (req.requester == requester && req.resource == resource) {
            req.requester = nullptr;
        }
    }
}

/// Loads a resource from disk, skipping the queue.
/// Shouldn't be used outside of resource LoadFromDisk() methods. 
void LoadDependency(Resource* res) {
    if (res->GetStatus() == Resource::UNLOADED) {
        res->LoadFromDisk();
    }

    ResourceRequest* req = request_pool.AddNew(ResourceRequest {
        .requester = nullptr,
        .resource = res
    });

    if (res->GetStatus() == Resource::READY) {
        finished_queue.push(req);
    } else {
        memory_loader_queue.push(req);
    }
}

/// Resource loading function.
/// Should only be used by through the Async::Init() function.
static void ResourceLoader() {
    while (!loaders_should_stop) {
        LoadResourcesFromDisk();
        
        disk_loader_queue.lock();
        auto len = disk_loader_queue.size();
        disk_loader_queue.unlock();
        
        if (len > 0) {
            continue;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

/// Processes the first resource queue.
/// If you started the Async system with at least one thread, you don't
/// need to call this function.
void LoadResourcesFromDisk() {        
    if (ResourceRequest* req; disk_loader_queue.try_pop(req)) {
        if (req->resource->GetStatus() == Resource::UNLOADED) {
            req->resource->LoadFromDisk();
        }

        if (req->resource->GetStatus() == Resource::READY) {
            finished_queue.push(req);
        } else {
            memory_loader_queue.push(req);
        }
    }
}

/// Processes the second resource queue.
/// @warning This function should only be called from the rendering thread.
void LoadResourcesFromMemory() {
    ResourceRequest* req;

    while (memory_loader_queue.try_pop(req)) {
        if (req->resource->GetStatus() == Resource::LOADED) {
            req->resource->LoadFromMemory();
        }

        finished_queue.push(req);
    }

}

/// Notifies EntityComponents about finished resources.
void FinishResources() {
    ResourceRequest* req;

    while (finished_queue.try_pop(req)) {
        if (req->requester) {
            req->requester->ResourceReady();
        }
    }
}

/// Starts the async resource loader thread.
/// @param Number of threads for async loading.
void Init(size_t threads) {
    assert(System::IsInitialized(System::SYSTEM_CORE));
    
#ifdef __EMSCRIPTEN__
        threads = 0;
#endif
    
    if (threads) {
        loaders_should_stop = false;
        
        for (size_t i = 0; i < threads; i++) {
            resource_loaders.push_back(std::thread(ResourceLoader));
        }
    }
    
    System::SetInitialized(System::SYSTEM_ASYNC, true);
}

/// Stops the async resource loader thread.
void Yeet() {
    assert(System::IsInitialized(System::SYSTEM_ASYNC));
    
    loaders_should_stop = true;
    
    for (auto& loader : resource_loaders) {
        loader.join();
    }
    
    System::SetInitialized(System::SYSTEM_ASYNC, false);
}

/// Returns number of resources in queues.
size_t GetWaitingResources() {
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
