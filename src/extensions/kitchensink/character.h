// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_DESIGN_CHARACTER_H
#define TRAM_SDK_EXTENSIONS_DESIGN_CHARACTER_H

#include <extensions/design/attributes.h>
#include <extensions/design/inventory.h>

namespace tram::Ext::Design {
    struct CharacterClass {
        
        
        void Register(name_t name, const CharacterClass& def);
    };
}

#endif // TRAM_SDK_EXTENSIONS_DESIGN_CHARACTER_H