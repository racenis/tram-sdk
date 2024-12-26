// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_ENTITYCOMPONENT_H
#define TRAM_SDK_FRAMEWORK_ENTITYCOMPONENT_H

#include <templates/pool.h>

#include <framework/core.h>
#include <framework/async.h>

namespace tram {
    
class Event;
class Entity;

class EntityComponent {
public:
    EntityComponent();
    virtual ~EntityComponent();

    virtual void Init() {
        is_init = true;
        if (resources_waiting == 0) Start();
    }

    virtual void EventHandler(Event &event) = 0;

    inline bool IsReady() { return is_ready; }
    inline bool IsInit() { return is_init; }
    
    inline Entity* GetParent() { return parent; }
    inline void SetParent(Entity* parent) { this->parent = parent; }

protected:
    size_t resources_waiting = 0;
    bool is_ready = false;
    bool is_init = false;
    Entity* parent = nullptr;

    void ResourceReady() { resources_waiting--; if (resources_waiting == 0 && is_init) Start(); }

    virtual void Start() = 0;
    friend void Async::FinishResources();
    template <typename T> friend class ResourceProxy;
};

/// Wrapper for an EntityComponent pointer.
/// Automatically constructs the EntityComponent in the correct pool,
/// also provides protection against accidental memory leaks.
template <typename T>
class Component {
public:
    Component() {}
    ~Component() { if (ptr) PoolProxy<T>::Delete(ptr); }
    void make() { ptr = PoolProxy<T>::New(); }
    void clear() { if (ptr) PoolProxy<T>::Delete(ptr); ptr = nullptr; }
    T* get() { return ptr; }
    T* operator->() { return ptr; }
    T& operator*() { return ptr; }
    operator T*() { return ptr; }
    explicit operator bool() { return ptr != nullptr; }
protected:
    T* ptr = nullptr;
};
    
}

#endif // TRAM_SDK_FRAMEWORK_ENTITYCOMPONENT_H