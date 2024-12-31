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
    EntityComponent() = default;
    virtual ~EntityComponent() = default;

    virtual void Init();

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
    
    // we could make resources_waiting a uint16_t and then we could even turn
    // is_ready and is_init into a single bitmask, saving memory

    void ResourceReady();

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
    ~Component() { clear(); }
    void make() { if (ptr) yeet(); init(); }
    void clear() { if (ptr) yeet(); ptr = nullptr; }
    T* get() { return ptr; }
    T* operator->() { return ptr; }
    T& operator*() { return ptr; }
    operator T*() { return ptr; }
    explicit operator bool() { return ptr != nullptr; }
protected:
    void init();
    void yeet();

    T* ptr = nullptr;
};
    
}

#endif // TRAM_SDK_FRAMEWORK_ENTITYCOMPONENT_H