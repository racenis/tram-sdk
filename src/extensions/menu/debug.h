// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_MENU_DEBUG_H
#define TRAM_SDK_EXTENSIONS_MENU_DEBUG_H

#include <extensions/menu/menu.h>

#include <functional>

namespace tram {
    class WorldCell;
}

namespace tram::Ext::Menu {

void InitCallbacks();
void UpdateCallbacks();

class InterceptMenu;
class StatisticsMenu;

class DebugMenu : public Menu {
public:
    void Display();
    ~DebugMenu() = default;
protected:
    static bool intercept_enabled;
    static bool statistics_enabled;
    static InterceptMenu* intercept_menu;
    static StatisticsMenu* statistics_menu;
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

class ListSelection : public Menu {
public:
    ListSelection(std::function<void(uint32_t)> callback, std::vector<std::string> list);
    void Display();
    ~ListSelection() = default;
protected:
    std::function<void(uint32_t)> callback;
    std::vector<std::string> list;
};

class MessageSend : public Menu {
public:
    void SetEntity(id_t entity);
    void SetMessageType(uint32_t type);
    void Display();
    ~MessageSend() = default;
protected:
    uint32_t parameter_type = 0;
    id_t entity_id = 0;
    uint32_t message_type = 0;
};

class InterceptMenu : public Menu {
public:
    void Display();
    inline void SetOffset(uint32_t offset) { this->offset = offset; } 
    ~InterceptMenu() = default;
protected:
    uint32_t offset = 0;
};

class WorldCellProperties : public Menu {
public:
    void SetPicked(WorldCell* cell);
    void Display();
    ~WorldCellProperties() = default;
protected:
    WorldCell* cell = nullptr;
};

class StatisticsMenu : public Menu {
public:
    void Display();
    ~StatisticsMenu() = default;
};

}

#endif