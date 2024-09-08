// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_DESIGN_INVENTORY_H
#define TRAM_SDK_EXTENSIONS_DESIGN_INVENTORY_H

#include <extensions/kitchensink/attributes.h>

#include <framework/gui.h>

namespace tram::Ext::Kitchensink {

typedef void (*item_action_func)();

struct ItemClass {
    name_t name;
    
    name_t viewmodel;
    
    item_action_func primary_action = nullptr;
    item_action_func secondary_action = nullptr;
    item_action_func idle_action = nullptr;
    
    bool draw_hands = false;
    
    GUI::font_t sprite_font = 0;
    GUI::font_t icon_font = 0;
    GUI::glyph_t sprite_glyph = 0;
    GUI::glyph_t icon_glyph = 0;
    
    int width = 1, height = 1;
    int stack = 1;
    
    std::vector<Attribute> attributes;
    
    static ItemClass* Find(name_t item_class);
};

struct ItemInstance {
    name_t item_class;
    int count = 1;
    int x = 0, y = 0;
};

struct Inventory {
    std::vector<ItemInstance> items;
    
    int holding = -1;
    int width = 1, height = 1;
    
    bool allow_overlap = true;
    
    int AddItem(const ItemInstance& item);
    int AddItem(name_t item_class, int count);
    int RemoveItem(name_t item_class, int count);
    int GetItemCount(name_t item_class);
};




}

#endif // TRAM_SDK_EXTENSIONS_DESIGN_INVENTORY_H