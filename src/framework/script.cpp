// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/script.h>

#include <cstring>

#include <iostream>

#include <framework/core.h>
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
        std::cout << "hello form c++!" << std::endl;
        std::cout << "size : " << array.size() << std::endl;
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
    
    SetFunction("__tram_impl_event_add_listener", {}, [](valuearray_t) -> value_t {
        return GetDeltaTime();
    });
    
    SetFunction("__tram_impl_event_remove_listener", {}, [](valuearray_t) -> value_t {
        return GetDeltaTime();
    });
    

    LoadScript("api");
}

}
