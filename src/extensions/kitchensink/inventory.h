// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_DESIGN_INVENTORY_H
#define TRAM_SDK_EXTENSIONS_DESIGN_INVENTORY_H

#include <extensions/kitchensink/attributes.h>

#include <framework/gui.h>

namespace tram {
    class Entity;
}

namespace tram::Ext::Kitchensink {

struct Inventory;
struct ItemInstance;
typedef void (*item_action_func)(Inventory*, ItemInstance*);


// TODO: ItemClass should not be struct, but rather a class
// - it should also have virtual methods instead of action names!!
// - there should also be a .. make? or something. register?

struct ItemClass {
    name_t name;
    name_t base_class;
    
    name_t viewmodel;
    name_t worldmodel;
    
    name_t primary_action;
    name_t secondary_action;
    name_t idle_action;
    
    name_t equipped_slot;
    
    GUI::font_t sprite_font = 0;
    GUI::font_t icon_font = 0;
    GUI::glyph_t sprite_glyph = 0;
    GUI::glyph_t icon_glyph = 0;
    
    int width = 1, height = 1;
    int stack = 1;
    
    float weight = 0.0f;
    
    name_t default_compartment;
    
    // TODO: also add effects
    std::vector<Attribute> attributes;
    
    static ItemClass* Find(name_t item_class);
};

struct ItemInstance {
    name_t item_class;
    name_t compartment;
    int count = 1;
    int x = 0, y = 0;
    bool equipped = false;
};

// TODO:
// instead of holding itemInstances directly, inventories should hold compartments
// then each compartment would have a type/manager class that is an instance of
struct Inventory {
    std::vector<ItemInstance> items;
    
    int width = 1, height = 1;
    
    bool allow_overlap = true;
    
    int AddItem(const ItemInstance& item);
    int AddItem(name_t item_class, int count);
    int RemoveItem(name_t item_class, int count);
    int GetItemCount(name_t item_class);
    static Inventory* Find(Entity* entity);
};




}

#endif // TRAM_SDK_EXTENSIONS_DESIGN_INVENTORY_H