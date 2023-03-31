// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_LANGUAGE_H
#define TRAM_SDK_FRAMEWORK_LANGUAGE_H

#include <framework/uid.h>

namespace tram::Language {
    const char* Get (name_t name);
    void Load (const char* filename);
}

#endif // TRAM_SDK_FRAMEWORK_LANGUAGE_H