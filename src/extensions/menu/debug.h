// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_MENU_DEBUG_H
#define TRAM_SDK_EXTENSIONS_MENU_DEBUG_H

#include <extensions/menu/menu.h>

#include <functional>

namespace tram::Ext::Menu {

class DebugMenu : public Menu {
public:
    void Display();
    ~DebugMenu() = default;
};

class EntityProperties : public Menu {
public:
    void SetPicked(id_t entity);
    void Display();
    ~EntityProperties() = default;
protected:
    id_t entity = 0;
};

class EntityPicker : public Menu {
public:
    EntityPicker(std::function<void(id_t)> callback);
    void Display();
    ~EntityPicker() = default;
protected:
    std::function<void(id_t)> callback;
};

}

#endif