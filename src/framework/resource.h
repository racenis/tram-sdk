// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_RESOURCE_H
#define TRAM_SDK_FRAMEWORK_RESOURCE_H

#include <framework/uid.h>
#include <framework/entitycomponent.h>

namespace tram {

class Resource  {
public:
    enum Status {
        UNLOADED,   //< Resource not in memory. Can't be used.
        LOADED,     //< Resource loaded in memory, but still needs some processing (like uploading to video memory).
        READY,      //< Resource is ready for sus.
    };

    Resource (name_t name) : name(name) {}
    
    inline Resource::Status GetStatus() const { return status; }
    inline name_t GetName() const { return name; }
    inline size_t GetReferences() const { return references; }
    inline bool IsLoadFailed() const { return load_fail; }
    
    inline void AddReference() { references++; }
    inline void RemoveReference() { references--; }
    
    virtual void LoadFromDisk() = 0;
    virtual void LoadFromMemory() = 0;
    
    void Load() { LoadFromDisk(); LoadFromMemory(); }
    
    virtual void Unload() = 0;
protected:
    Status status = UNLOADED;
    name_t name;
    bool load_fail = false;
    size_t references = 0;
};


namespace Async { void RequestResource (EntityComponent* requester, Resource* requested_resource); }

template <typename T>
class ResourceProxy {
public:
    ResourceProxy(EntityComponent* parent) : parent(parent) {}
    ~ResourceProxy() { set(nullptr); }
    void set(Resource* new_res){
        if (resource) {
            resource->RemoveReference();
            Async::CancelRequest(parent, resource);
        } 

        resource = new_res;
        
        if (resource) {
            resource->AddReference();
            
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

#endif // TRAM_SDK_FRAMEWORK_RESOURCE_H