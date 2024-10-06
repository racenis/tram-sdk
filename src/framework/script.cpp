// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/script.h>

#include <cstring>

#include <iostream>

#include <framework/core.h>
#include <framework/entity.h>
#include <framework/event.h>

namespace tram::Script {

static Language language = {
    name_t(),
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr
};
    
void SetGlobal(name_t name, value_t value) {
    language.set_global(name, value);
}

value_t GetGlobal(name_t name) {
    return language.get_global(name);
}

void SetFunction(name_t name, std::vector<Type> parameters, value_t (*function)(valuearray_t)) {
    language.set_function(name, parameters, function);
}

value_t CallFunction(name_t name, std::vector<Value> parameters) {
    return language.call_function(name, parameters);
}

void SetLanguage(Language lang) {
    language = lang;
}

void LoadScript(const char* path) {
    language.load_script(path);
}

void Init() {
    if (!language.name) {
        std::cout << "Failed script initialization! Language not set!" << std::endl;
        abort();
    }
    
    // FRAMEWORK/CORE.H
    
    SetFunction("__tram_impl_core_get_tick", {}, [](valuearray_t) -> value_t {
        return GetTick();
    });
    
    SetFunction("__tram_impl_core_get_tick_time", {}, [](valuearray_t) -> value_t {
        return GetTickTime();
    });
    
    SetFunction("__tram_impl_core_get_delta_time", {}, [](valuearray_t) -> value_t {
        return GetDeltaTime();
    });
    
    
    // FRAMEWORK/EVENT.H
    
    SetFunction("__tram_impl_event_register", {TYPE_STRING}, [](valuearray_t array) -> value_t {
        assert(array.size());
        const char* name = array[0];
        assert(name);
        char* copy = (char*)malloc(strlen(name) + 1);
        strcpy(copy, name);
        
        return Event::Register(copy);
    });
    
    SetFunction("__tram_impl_event_get_type", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        return Event::GetType(array[0]);
    });
    
    SetFunction("__tram_impl_event_get_name", {TYPE_UINT16}, [](valuearray_t array) -> value_t {
        return Event::GetName(array[0]);
    });
    
    SetFunction("__tram_impl_event_get_last", {}, [](valuearray_t) -> value_t {
        return Event::GetLast();
    });
    
    SetFunction("__tram_impl_event_post", {TYPE_UINT16, TYPE_UINT16, TYPE_UINT32, TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Event event;
        
        event.type = array[0];
        event.subtype = array[1];
        event.poster_id = array[2];
        event.data_int = (uint32_t)array[3];
    
        Event::Post(event);
        
        return name_t();
    });
    
    SetFunction("__tram_impl_event_add_listener", {TYPE_UINT16, TYPE_UINT32}, [](valuearray_t array) -> value_t {
        size_t data_int = (uint32_t)array[1];
        void* data_ptr = (void*)data_int;
        
        return Event::AddListener(array[0], data_ptr, [](Event& event, void* data) {
            size_t data_ptr = (size_t)data;
            uint32_t data_int = (uint32_t)data_ptr;
            
            CallFunction("__tram_impl_event_event_callback", {event.type,
                                                              event.subtype,
                                                              event.poster_id,
                                                              (uint32_t)event.data_int,
                                                              data_int});
        });
    });
    
    SetFunction("__tram_impl_event_remove_listener", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Event::RemoveListener((listener_t)array[0]);
        return name_t();
    });
    
    SetFunction("__tram_impl_entity_find_by_name", {TYPE_NAME}, [](valuearray_t array) -> value_t {
        //std::cout << "findning !! " << (name_t)array[0] << std::endl;
        Entity* entity = Entity::Find((name_t)array[0]);
        //std::cout << "found: " << entity << " | " << entity->GetID() << std::endl;
        return entity ? entity->GetID() : 0;
        //return entity->GetID();
    });
    
    SetFunction("__tram_impl_entity_find_by_id", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        return entity ? entity->GetID() : 0;
    });
    
    SetFunction("__tram_impl_entity_get_name", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        return entity ? entity->GetName() : name_t();
    });
    
    SetFunction("__tram_impl_entity_get_location", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        return entity ? entity->GetLocation() : vec3();
    });
    
    SetFunction("__tram_impl_entity_set_location", {TYPE_UINT32, TYPE_VEC3}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        if (entity) entity->SetLocation(array[1]);
        return name_t();
    });
    
    SetFunction("__tram_impl_entity_get_rotation", {TYPE_UINT32}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        return entity ? entity->GetRotation() : quat(1.0f, 0.0f, 0.0f, 0.0f);
    });
    
    SetFunction("__tram_impl_entity_set_rotation", {TYPE_UINT32, TYPE_QUAT}, [](valuearray_t array) -> value_t {
        Entity* entity = Entity::Find((uint32_t)array[0]);
        if (entity) entity->SetRotation(array[1]);
        return name_t();
    });
    
    
    

    LoadScript("api");
}

}
