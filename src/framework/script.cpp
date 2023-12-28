// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/script.h>

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
    
}
