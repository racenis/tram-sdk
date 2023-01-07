// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_RESOURCE_H
#define FRAMEWORK_RESOURCE_H

#include <framework/uid.h>
#include <framework/entitycomponent.h>

namespace Core {
    class Resource  {
    public:
        enum Status {
            UNLOADED,          /// Resource not in memory. Can't be used.
            WAITING_FOR_LOAD,  /// Resource not in memory, but has been placed in the load queue.
            LOAD_FAIL,         // not used??
            LOADED,            // also not used???
            WAITING_FOR_READY, /// Resource loaded in memory, but still needs some processing (like uploading to video memory).
            READY,             /// Resource is ready to be used.
            READY_BUT_FAIL     // don't need this
        };
    protected:
        Status status = UNLOADED;
        name_t name = 0;
        bool load_fail = false;
        size_t references = 0;
    public:
        Resource(){}
        Resource(name_t name) : name(name) {}
        inline Resource::Status GetStatus() const {return status;}
        inline name_t GetName() {return name;};
        inline size_t GetRef() {return references;}
        inline void AddRef(){references++;}
        inline void RemoveRef(){references--;}
        void virtual LoadFromDisk() = 0;
        void virtual LoadFromMemory() = 0;
    };


    namespace Async { void RequestResource (EntityComponent* requester, Resource* requested_resource); }

    template <typename T>
    class ResourceProxy {
    public:
        ResourceProxy(EntityComponent* parent) : parent(parent) {}
        ~ResourceProxy() { set(nullptr); }
        void set(Resource* res){
            if (resource) resource->RemoveRef();

            resource = res;
            
            if (resource) {
                resource->AddRef();
                
                if (resource->GetStatus() != Resource::READY) {
                    parent->resources_waiting++;
                    Async::RequestResource(parent, resource);
                }
            }
        }
        T* get() { return (T*)resource; }
        T* operator= (Resource* res) { set(res); return get(); }
        T* operator->() { return (T*)resource; }
        T& operator*() { return *((T*)resource); }
        explicit operator bool() { return resource != nullptr; }
    protected:
        EntityComponent* parent;
        Resource* resource = nullptr;
    };
}

#endif // FRAMEWORK_RESOURCE_H