// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <templates/queue.h>
#include <templates/pool.h>

#include <framework/core.h>
#include <framework/async.h>
#include <framework/system.h>
#include <framework/resource.h>
#include <framework/entitycomponent.h>
#include <framework/logging.h>

#include <platform/api.h>

#include <config.h>

#include <thread>
#include <vector>

/*
 * currently with how everything is implemented, only EntityComponents can
 * request resources.
 * 
 * it would be better if we instead would accept a function pointer and a
 * void data pointer, so that more kinds of things could request resources
 * 
 * ideas:
 * typedef an Async::task_t = uint32_t
 * create a task_t EnqueueTask(function ptr, data ptr)
 */

/**
 * @file framework/async.cpp
 * 
 * Implementation of the Async system.
 */


/**
 * @namespace tram::Async
 * 
 * Multi-threading and Resource streaming.
 * 
 * Currently Async only does Resource streaming, but we could do other kinds of
 * Async processing in the future too.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/async.html
 */

namespace tram::Async {

static std::vector<std::thread> resource_loaders;
static bool loaders_should_stop = true;

enum RequestNotification {
    NONE,
    COMPONENT,
    CALLBACK
};

struct ResourceRequest {
    RequestNotification notification_type;
    Resource* resource = nullptr;
    
    union {
        struct {
            void (*callback)(void*);
            void* callback_data;
        };
        
        EntityComponent* requester;
    };
    
    // TODO: check if we still need this padding
#ifndef __x86_64__
    void* padding[2];
#endif
};

static Queue<ResourceRequest*> disk_loader_queue("Async::LoadResourcesFromDisk() queue", RESOURCE_LOADER_QUEUE_LIMIT);
static Queue<ResourceRequest*> memory_loader_queue("Async::LoadResourcesFromMemory() queue", RESOURCE_LOADER_QUEUE_LIMIT);
static Queue<ResourceRequest*> finished_queue("Async::FinishResources() queue", RESOURCE_LOADER_QUEUE_LIMIT);

static Pool<ResourceRequest> request_pool("Async::ResourceRequest pool", RESOURCE_LOADER_REQUEST_LIMIT);

/// Adds a resource to the loading queue.
/// @param requester            EntityComponent that will be notified when the 
///                             resource is loaded. Can be set to nullptr, in 
///                             which case nothing will be notified.
/// @param requested_resource   The resource that will be loaded.
void RequestResource(EntityComponent* requester, Resource* resource) {
    if (!System::IsInitialized(System::ASYNC)) {
        Log(Severity::CRITICAL_ERROR, System::ASYNC, "Can not request '{}' load, Async is not initialized", resource->GetName());
    }
    
    disk_loader_queue.push(request_pool.AddNew(ResourceRequest {
        .notification_type = RequestNotification::COMPONENT,
        .resource = resource,
        .requester = requester
    }));
}

/// Adds a resource to the loading queue.
/// @param callback             Callback function which will be called when the
///                             resource has been loaded and is ready for use.
/// @param data                 Data pointer, which will be passed to as a
///                             parameter to the callback function.
/// @param requested_resource   The resource that will be loaded.
void RequestResource(void(*callback)(void* data), void* data, Resource* resource) {
    if (!System::IsInitialized(System::ASYNC)) {
        Log(Severity::CRITICAL_ERROR, System::ASYNC, "Can not request '{}' load, Async is not initialized", resource->GetName());
    }
    
    disk_loader_queue.push(request_pool.AddNew(ResourceRequest {
        .notification_type = RequestNotification::COMPONENT,
        .callback = callback,
        .callback_data = data
    }));
}

/// Cancels a resource load request.
/// Should be called if the EntityComponent that requested a Resource to be
/// loaded will be destructed before the Resource has finished loading.
/// @note The resource will be fully loaded anyway, but the requester will not
///       be notified.
void CancelRequest(EntityComponent* requester, Resource* resource) {
    for (auto& request : request_pool) {
        if (request.requester == requester && request.resource == resource) {
            request.notification_type = RequestNotification::NONE;
            request.requester = nullptr;
        }
    }
}

/// Loads a resource from disk, skipping the queue.
/// Shouldn't be used outside of resource LoadFromDisk() methods. 
void LoadDependency(Resource* resource) {
    if (resource->GetStatus() == Resource::UNLOADED) {
        resource->LoadFromDisk();
    }

    ResourceRequest* request = request_pool.AddNew(ResourceRequest {
        .notification_type = RequestNotification::NONE,
        .resource = resource
    });

    if (resource->GetStatus() == Resource::READY) {
        finished_queue.push(request);
    } else {
        memory_loader_queue.push(request);
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
        
        /* this will make the thread go to sleep when all requested resources
         * have been loaded.
         * 
         * this will prevent the thread from wasting CPU cycles, but it will
         * also make it slow to respond to new requests.
         * 
         * we could use a mutex or something to wake it up, instead of using a
         * timout
         * 
         * TODO: fix
         */
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

/// Processes the first resource queue.
/// If there are any resource loader threads active, this function will not do
/// anything, instead allowing the loader threads to load the resources.
void LoadResourcesFromDisk() {
    if (resource_loaders.size()) {
        // if any resource loader threads are active, we should check if this
        // function is being called from one of them, otherwise we return. this
        // is done in order to not slow down the main thread
        bool called_from_loader = false;
        for (const auto& loader : resource_loaders) {
            if (loader.get_id() == std::this_thread::get_id()) {
                called_from_loader = true;
            }
        }
        
        if (!called_from_loader) return;
    }
    
    if (ResourceRequest* request; disk_loader_queue.try_pop(request)) {
        if (request->resource->GetStatus() == Resource::UNLOADED) {
            request->resource->LoadFromDisk();
        }

        if (request->resource->GetStatus() == Resource::READY) {
            finished_queue.push(request);
        } else {
            memory_loader_queue.push(request);
        }
    }
}

/// Processes the second resource queue.
/// @warning This function should only be called from the rendering thread.
void LoadResourcesFromMemory() {
    if (!Platform::Window::IsRenderContextThread()) {
        Log(Severity::CRITICAL_ERROR, System::ASYNC, "Async::LoadResourcesFromMemory() not called from rendering thread");
    }
    
    for (ResourceRequest* request; memory_loader_queue.try_pop(request);) {
        if (request->resource->GetStatus() == Resource::LOADED) {
            request->resource->LoadFromMemory();
        }

        finished_queue.push(request);
    }
}

/// Notifies EntityComponents about finished resources.
void FinishResources() {
    for (ResourceRequest* request; finished_queue.try_pop(request);) {
        switch (request->notification_type) {
            case RequestNotification::CALLBACK:
                request->callback(request->callback_data);
                break;
            case RequestNotification::COMPONENT:
                request->requester->ResourceReady();
                break;
            case RequestNotification::NONE:
                break;
        }
        
        request_pool.Remove(request);
    }
}

/// Starts the async resource loader thread.
/// @param Number of threads for async loading.
void Init(size_t threads) {
    System::SetState(System::ASYNC, System::INIT);
    System::AssertDependency(System::CORE);
    
    // for some reason using threads makes the program not work in emscripten
    // TODO: investigate
#ifdef __EMSCRIPTEN__
    threads = 0;
#endif
    
    if (threads) {
        loaders_should_stop = false;
        
        for (size_t i = 0; i < threads; i++) {
            resource_loaders.push_back(std::thread(ResourceLoader));
        }
    }
    
    System::SetState(System::ASYNC, System::READY);
}

/// Stops the async resource loader thread.
void Yeet() {
    if (!System::IsInitialized(System::ASYNC)) {
        Log(Severity::ERROR, System::ASYNC, "Async is not initialized, can not yeet it");
    }
    
    loaders_should_stop = true;
    
    for (auto& loader : resource_loaders) {
        loader.join();
    }
    
    System::SetState(System::ASYNC, System::YEET);
}

/// Returns number of resources in queues.
size_t GetWaitingResources() {
    return request_pool.size();
}

}
