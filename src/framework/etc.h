// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef ETC_H
#define ETC_H

//#include <cstdint>
#include <framework/uid.h>

namespace Core {
    typedef uint64_t id_t;
    
    id_t GenerateID();
    
    name_t FindLangStr(name_t name);
    void LoadText(const char* filename);
}

#endif // ETC_H