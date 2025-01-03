// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_EXTENSIONS_MENU_SYSTEM_H
#define TRAM_SDK_EXTENSIONS_MENU_SYSTEM_H

#include <extensions/menu/menu.h>

#include <functional>

namespace tram {
    class WorldCell;
}

namespace tram::Ext::Menu {

//void InitCallbacks();
//void UpdateCallbacks();

class SystemMenu : public Menu {
public:
    void Display();
    uint32_t Layer() { return 0; }
    ~SystemMenu() = default;
protected:

};

class SettingsMenu : public Menu {
public:
    void Display();
    uint32_t Layer() { return 0; }
    ~SettingsMenu() = default;
protected:
    uint32_t tab = 0;
    uint32_t binding_action = 0;
    bool binding_state_ready = false;
};

}

#endif