// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#ifndef TRAM_SDK_EXTENSIONS_MENU_MENU_H
#define TRAM_SDK_EXTENSIONS_MENU_MENU_H

#include <framework/gui.h>

namespace tram::Ext::Menu {

void Init();
void Update();

extern GUI::font_t FONT_WIDGETS;
extern GUI::font_t FONT_TEXT;
extern GUI::font_t FONT_TEXT_BOLD;
extern GUI::font_t FONT_HEADER;
extern GUI::font_t FONT_PIXELART;

class Menu {
public:
    virtual void Display() = 0;
    virtual uint32_t Layer() = 0;
    virtual ~Menu() = default;

    static void Push(Menu*);
    static bool Pop();
    static bool Empty();
    static void Clear();
    
    static void Add(Menu*);
    static void Remove(Menu*);
};

}

#endif