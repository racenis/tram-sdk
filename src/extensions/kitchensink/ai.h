// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_KITCHENSINK_AI_H
#define TRAM_SDK_EXTENSIONS_KITCHENSINK_AI_H

#include <vector>
#include <framework/uid.h>
#include <framework/math.h>

namespace tram::Ext::Kitchensink {


class AIPackage {
public:
    char pudding[100];

    static AIPackage* Find(name_t name);
private:
    
};

}

#endif // TRAM_SDK_EXTENSIONS_KITCHENSINK_AI_H