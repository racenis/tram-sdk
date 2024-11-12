// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_KITCHENSINK_MENU_H
#define TRAM_SDK_EXTENSIONS_KITCHENSINK_MENU_H

#include <extensions/menu/menu.h>

namespace tram::Ext::Kitchensink {
    
class ListInventory : public Ext::Menu::Menu {
    void Display();
    uint32_t Layer() { return 0; }
};

class ListTrade : public Ext::Menu::Menu {
    void Display();
    uint32_t Layer() { return 0; }
};

class ListDialog : public Ext::Menu::Menu {
    void Display();
    uint32_t Layer() { return 0; }
};

class QuestList : public Ext::Menu::Menu {
    void Display();
    uint32_t Layer() { return 0; }
};

class KitchensinkMenuList : public Ext::Menu::Menu {
    void Display();
    uint32_t Layer() { return 0; }
};

    
}

#endif // TRAM_SDK_EXTENSIONS_KITCHENSINK_MENU_H