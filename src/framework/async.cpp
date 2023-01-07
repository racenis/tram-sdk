// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <templates/queue.h>

#include <framework/core.h>
#include <framework/async.h>
#include <framework/system.h>
#include <framework/resource.h>
#include <framework/entitycomponent.h>

#include <thread>

namespace Core::Async {
    std::thread resource_loader_thread;
    bool loaders_should_stop = false;

    struct ResourceRequest {
        EntityComponent* requester;
        uint64_t requester_id;
        Resource* requested_res;
    };

    Queue<ResourceRequest> resourceRequestQueue("resource request queue", 500);
    Queue<ResourceRequest> resourceRequestQueue2ndStage("resource request queue 2nd stage", 500);
    Queue<ResourceRequest> finishedResourceRequestQueue("finished resource request queue", 500);

    /// Adds a resource to the loading queue.
    /// @param requester EntityComponent that will be notified when the resource is loaded.
    /// Can be set to nullptr, in which case nothing will be notified.
    /// @param requested_resource The resource that will be loaded.
    void RequestResource(EntityComponent* requester, Resource* requested_resource) {
        resourceRequestQueue.Lock();

        ResourceRequest* req = resourceRequestQueue.AddNew();

        req->requester = requester;
        req->requester_id = requester ? requester->GetID() : 0;
        req->requested_res = requested_resource;

        resourceRequestQueue.Unlock();
    }

    /// Resource loading function.
    /// Should only be used by through the Async::Init() function.
    void ResourceLoader() {
        while (!loaders_should_stop){

            resourceRequestQueue.Lock();
            ResourceRequest* req = resourceRequestQueue.GetFirstPtr();
            resourceRequestQueue.Unlock();

            while (req){
                if(req->requested_res->GetStatus() == Resource::UNLOADED){
                    req->requested_res->LoadFromDisk();
                }

                if(req->requested_res->GetStatus() == Resource::READY){
                    finishedResourceRequestQueue.Lock();
                    ResourceRequest* f_req = finishedResourceRequestQueue.AddNew();
                    f_req->requester = req->requester;          // TODO: change this to memcpy()
                    f_req->requester_id = req->requester_id;
                    f_req->requested_res = req->requested_res;
                    finishedResourceRequestQueue.Unlock();
                } else {
                    resourceRequestQueue2ndStage.Lock();
                    ResourceRequest* n_req = resourceRequestQueue2ndStage.AddNew();
                    n_req->requester = req->requester;
                    n_req->requester_id = req->requester_id;
                    n_req->requested_res = req->requested_res;
                    resourceRequestQueue2ndStage.Unlock();
                }

                resourceRequestQueue.Lock();
                resourceRequestQueue.Remove();
                req = resourceRequestQueue.GetFirstPtr();
                resourceRequestQueue.Unlock();
            }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        }
    }

    /// Processes the second resource queue.
    /// @warning This function should only be called from the rendering thread.
    void ResourceLoader2ndStage(){
        resourceRequestQueue2ndStage.Lock();
        ResourceRequest* req = resourceRequestQueue2ndStage.GetFirstPtr();
        resourceRequestQueue2ndStage.Unlock();

        while (req){
            if(req->requested_res->GetStatus() == Resource::LOADED || req->requested_res->GetStatus() == Resource::LOAD_FAIL){
                req->requested_res->LoadFromMemory();
            }

            finishedResourceRequestQueue.Lock();
            ResourceRequest* f_req = finishedResourceRequestQueue.AddNew();
            f_req->requester = req->requester;
            f_req->requester_id = req->requester_id;
            f_req->requested_res = req->requested_res;
            finishedResourceRequestQueue.Unlock();

            resourceRequestQueue2ndStage.Lock();
            resourceRequestQueue2ndStage.Remove();
            req = resourceRequestQueue2ndStage.GetFirstPtr();
            resourceRequestQueue2ndStage.Unlock();
        }

    }

    /// Loads a resource from disk, skipping the queue.
    /// Shouldn't be used outside of resource LoadFromDisk() methods. 
    void ForceLoadResource(Resource* res){
        if(res->GetStatus() == Resource::UNLOADED){
            res->LoadFromDisk();
        }

        if(res->GetStatus() == Resource::READY || res->GetStatus() == Resource::LOAD_FAIL){
            finishedResourceRequestQueue.Lock();
            ResourceRequest* f_req = finishedResourceRequestQueue.AddNew();
            f_req->requester = nullptr;
            f_req->requester_id = 0;
            f_req->requested_res = res;
            finishedResourceRequestQueue.Unlock();
        } else {
            resourceRequestQueue2ndStage.Lock();
            ResourceRequest* n_req = resourceRequestQueue2ndStage.AddNew();
            n_req->requester = nullptr;
            n_req->requester_id = 0;
            n_req->requested_res = res;
            resourceRequestQueue2ndStage.Unlock();
        }
    }

    /// Notifies EntityComponents about finished resources.
    void FinishResource(){
        finishedResourceRequestQueue.Lock();
        ResourceRequest* req = finishedResourceRequestQueue.GetFirstPtr();
        finishedResourceRequestQueue.Unlock();

        while (req){
            // TODO: check if requester pointer is valid
            // TODO: check if requester ids match up
            if (req->requester){
                req->requester->ResourceReady();
            }

            finishedResourceRequestQueue.Lock();
            finishedResourceRequestQueue.Remove();
            req = finishedResourceRequestQueue.GetFirstPtr();
            finishedResourceRequestQueue.Unlock();
        }

    }

    /// Starts the async resource loader thread.
    void Init() {
        assert(System::IsInitialized(System::SYSTEM_CORE));
        
        loaders_should_stop = false;
        resource_loader_thread = std::thread(ResourceLoader);
        
        System::SetInitialized(System::SYSTEM_ASYNC, true);
    }

    /// Stops the async resource loader thread.
    void Yeet() {
        assert(System::IsInitialized(System::SYSTEM_ASYNC));
        
        loaders_should_stop = true;
        resource_loader_thread.join();
        
        System::SetInitialized(System::SYSTEM_ASYNC, false);
    }
}
