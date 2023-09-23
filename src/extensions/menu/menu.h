// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_MENU_MENU_H
#define TRAM_SDK_EXTENSIONS_MENU_MENU_H

#include <framework/gui.h>

namespace tram::Ext::Menu {
    void Init();
    void DebugMenu();
    void EscapeMenu();
    
    extern GUI::font_t FONT_SYMBOL;
    extern GUI::font_t FONT_TEXT;
    extern GUI::font_t FONT_TEXT_BOLD;
    extern GUI::font_t FONT_HEADER;
}

#endif