// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_SETTINGS_H
#define TRAM_SDK_FRAMEWORK_SETTINGS_H

#include <framework/value.h>

namespace tram::Settings {

void Register(ValuePtr ptr, name_t name);
void Register(Value value, name_t name);

Value Get(name_t name);
void Set(name_t name, Value value);

void Parse(const char** argv, int argc);

void Save(const char* file);
void Load(const char* file);

}

#endif // TRAM_SDK_FRAMEWORK_SETTINGS_H
