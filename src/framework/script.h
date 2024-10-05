// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_SCRIPT_H
#define TRAM_SDK_FRAMEWORK_SCRIPT_H

#include <framework/value.h>
#include <vector>

namespace tram::Script {

void SetGlobal(name_t name, value_t value);
value_t GetGlobal(name_t name);
void SetFunction(name_t name, std::vector<Type> parameters, value_t (*function)(valuearray_t));
value_t CallFunction(name_t name, std::vector<Value> parameters);


struct Language {
    name_t name;
    
    value_t (*get_global)(name_t name);
    void (*set_global)(name_t name, value_t value);
    void (*set_function)(name_t name, std::vector<Type> parameters, value_t (*function)(valuearray_t));
    value_t (*call_function)(name_t name, std::vector<Value> parameters);
    
    void (*load_script)(const char* path);
};

void SetLanguage(Language language);
void LoadScript(const char* path);

void Init();

}

#endif // TRAM_SDK_FRAMEWORK_SCRIPT_H