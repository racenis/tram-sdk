// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_LANGUAGE_H
#define FRAMEWORK_LANGUAGE_H

#include <framework/uid.h>

namespace Core::Language {
    name_t Get (name_t name);
    void Load (const char* filename);
}

#endif // FRAMEWORK_LANGUAGE_H