// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_SCRIPT_H
#define TRAM_SDK_FRAMEWORK_SCRIPT_H

#include <framework/value.h>
#include <vector>

namespace tram::Script {

void SetGlobal(name_t name, value_t value);
value_t GetGlobal(name_t name);
void SetFunction(name_t name, std::vector<Type> parameters, value_t (*function)(valuearray_t));
value_t CallFunction(name_t name, std::vector<Value> parameters);
value_t Evaluate(const char* code);

class Interface {
public:
    virtual name_t GetName() = 0;
    virtual value_t GetGlobal(name_t name) = 0;
    virtual void SetGlobal(name_t name, value_t value) = 0;
    virtual void SetFunction(name_t name, std::vector<Type> parameters, value_t (*function)(valuearray_t)) = 0;
    virtual value_t CallFunction(name_t name, std::vector<Value> parameters) = 0;
    
    virtual void LoadScript(const char* path) = 0;
    virtual value_t Evaluate(const char* code) = 0;
};

void SetInterface(Interface* language);
void LoadScript(const char* path);

void Init();

}

#endif // TRAM_SDK_FRAMEWORK_SCRIPT_H