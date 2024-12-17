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

class ItemClass {
public:
    name_t name;
    name_t base_class;
    
    name_t viewmodel;
    name_t worldmodel;
    
    name_t equipped_slot;
    
    GUI::font_t sprite_font = 0;
    GUI::font_t icon_font = 0;
    GUI::glyph_t sprite_glyph = 0;
    GUI::glyph_t icon_glyph = 0;
    
    int width = 1, height = 1;
    int stack = 1;
    
    float weight = 0.0f;
    
    name_t default_compartment;
    
    virtual bool OnEquip() { return false; }
    virtual bool OnUnequip() { return false; }
    virtual bool OnAdded() { return false; }
    virtual bool OnRemoved() { return false; }
    virtual bool OnPrimaryAction() { return false; }
    virtual bool OnSecondaryAction() { return false; }
    virtual bool OnIdle() { return false; }
    
    std::vector<Attribute> attributes;
    std::vector<Effect> effects;
    
    static void Register(ItemClass*);
    static ItemClass* Find(name_t item_class);
};

struct ItemInstance {
    name_t item_class;
    name_t compartment;
    int count = 1;
    int x = 0, y = 0;
    bool equipped = false;
};

class InventoryManager {
public:
    virtual int AddItem(name_t item_class, int count) = 0;
    virtual int RemoveItem(name_t item_class, int count) = 0;
    virtual int GetItemCount(name_t item_class) = 0;
    
    virtual ~InventoryManager() = default;
    
    static InventoryManager* New(name_t compartment);
    static void Delete(name_t compartment, InventoryManager*);
    static void Register(name_t compartment, InventoryManager*(*make)(), void (*clear)(InventoryManager*));
};

class ListInventoryManager : public InventoryManager {
public:
    int AddItem(name_t item_class, int count) override;
    int RemoveItem(name_t item_class, int count) override;
    int GetItemCount(name_t item_class) override;
    
    ~ListInventoryManager() override = default;
    
    std::vector<std::pair<name_t, int>> items;
};


// TODO:
// instead of holding itemInstances directly, inventories should hold compartments
// then each compartment would have a type/manager class that is an instance of
class Inventory {
public:
    int AddItem(name_t item_class, int count);
    int RemoveItem(name_t item_class, int count);
    int GetItemCount(name_t item_class);
    
    bool EquipItem(name_t item_class);
    bool UnequipItem(name_t item_class);
    bool UnequipSlot(name_t item_slot);
    
    std::vector<std::pair<name_t, name_t>> equipped;
    std::vector<std::pair<name_t, InventoryManager*>> compartments;
    
private:
    InventoryManager* FindCompartment(name_t compartment);
public:
    
    static void LoadFromDisk(const char* filename);
    static Inventory* Find(Entity* entity);
};




}

#endif // TRAM_SDK_EXTENSIONS_DESIGN_INVENTORY_H