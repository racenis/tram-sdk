// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_ENTITYCOMPONENT_H
#define FRAMEWORK_ENTITYCOMPONENT_H

#include <templates/pool.h>

#include <framework/async.h>
#include <framework/etc.h>

namespace Core {
    class Event;
    class Entity;
    
    class EntityComponent {
    public:
        EntityComponent(){ id = GenerateID(); }
        virtual ~EntityComponent() { std::cout << "EntityComponent base destructor." << std::endl;  }

        virtual void Init() {
            is_init = true;
            if (resources_waiting == 0) Start();
        }

        virtual void EventHandler(Event &event) = 0;

        inline bool IsReady() { return is_ready; }
        inline bool IsInit() { return is_init; }
        
        inline bool GetParent() { return parent; }
        inline void SetParent(Entity* parent) { this->parent = parent; }

        inline uint64_t GetID() { return id; }
    protected:
        uint64_t id;
        size_t resources_waiting = 0;
        bool is_ready = false;
        bool is_init = false;
        Entity* parent = nullptr;

        void ResourceReady() { resources_waiting--; if (resources_waiting == 0 && is_init) Start(); }

        virtual void Start() = 0;
        friend void Async::FinishResource();
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
        explicit operator bool() { return ptr != nullptr; }
    protected:
        T* ptr = nullptr;
    };
}

#endif // FRAMEWORK_ENTITYCOMPONENT_H