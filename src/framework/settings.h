// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_SETTINGS_H
#define TRAM_SDK_FRAMEWORK_SETTINGS_H

#include <framework/value.h>

#include <initializer_list>
#include <vector>

namespace tram::Settings {

enum : uint32_t {
    USER = 1,         //< Save the setting in user data
    APPLICATION = 2,  //< Save the setting in application state data
    MENU = 4,         //< Show the setting in the setting menu
    
    // maybe we could add some read-only or write-only flags?
    
    NONE = 0
};

void Register(bool& value, const char* name, uint32_t flags);
void Register(float& value, const char* name, uint32_t flags);
void Register(int32_t& value, const char* name, uint32_t flags);
void Register(uint32_t& value, const char* name, uint32_t flags);

Value Get(name_t name);
void Set(name_t name, Value value);

void Parse(const char** argv, int argc);

void Save(const char* file);
void Load(const char* file);

uint32_t Flags(name_t name);

std::vector<name_t> GetSettings(uint32_t filter);

template<typename T>
struct Property {
    Property(std::initializer_list<value_t> values) {
        value = values.begin()[0];
        
        const char* name = values.begin()[1];
        uint32_t flags = values.begin()[2];

        Register(value, name, flags);
    }
    
    Property(const bool& value) {
        this->value = value;
    }
    
    /*T& operator=(const Setting<T>& other) {
        this->value = other;
        return *this;
    }*/
    
    operator T() {
        return value;
    }
    
    T value;
};

}

#endif // TRAM_SDK_FRAMEWORK_SETTINGS_H
