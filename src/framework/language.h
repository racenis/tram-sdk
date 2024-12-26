// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_LANGUAGE_H
#define TRAM_SDK_FRAMEWORK_LANGUAGE_H

#include <framework/uid.h>

namespace tram::Language {
    const char* Get(name_t name);
    void Load(const char* filename);
    
    char UTF16ToLatin7(uint16_t);
}

#endif // TRAM_SDK_FRAMEWORK_LANGUAGE_H