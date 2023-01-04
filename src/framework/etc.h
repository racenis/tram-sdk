// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef ETC_H
#define ETC_H

// TODO: yeet this file

#include <framework/uid.h>

namespace Core {
    typedef uint64_t id_t;
    
    id_t GenerateID();  // this 100% should be in core.h
    
    name_t FindLangStr(name_t name);        // these should be in their own file
    void LoadText(const char* filename);
}

#endif // ETC_H