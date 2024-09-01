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
    uint32_t Layer() { return 0; }
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
    uint32_t Layer() { return 1; }
    void Display();
    ~EntityProperties() = default;
protected:
    id_t entity = 0;
};

class EntityPicker : public Menu {
public:
    EntityPicker(std::function<void(id_t)> callback);
    uint32_t Layer() { return 1; }
    void Display();
    ~EntityPicker() = default;
protected:
    std::function<void(id_t)> callback;
};

class ListSelection : public Menu {
public:
    ListSelection(std::function<void(uint32_t)> callback, std::vector<std::string> list);
    uint32_t Layer() { return 2; }
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
    uint32_t Layer() { return 1; }
    void Display();
    ~MessageSend() = default;
protected:
    uint32_t parameter_type = 0;
    id_t entity_id = 0;
    uint32_t message_type = 0;
    char parameter_string[32] = "";
};

class InterceptMenu : public Menu {
public:
    void Display();
    uint32_t Layer() { return 0; }
    inline void SetOffset(uint32_t offset) { this->offset = offset; } 
    ~InterceptMenu() = default;
protected:
    uint32_t offset = 0;
};

class WorldCellProperties : public Menu {
public:
    void SetPicked(WorldCell* cell);
    uint32_t Layer() { return 1; }
    void Display();
    ~WorldCellProperties() = default;
protected:
    WorldCell* cell = nullptr;
};

class StatisticsMenu : public Menu {
public:
    void Display();
    uint32_t Layer() { return 0; }
    ~StatisticsMenu() = default;
};

class EventEmit : public Menu {
public:
    void SetEventType(uint32_t type) { event_type = type; }
    uint32_t Layer() { return 1; }
    void Display();
    ~EventEmit() = default;
protected:
    uint16_t subtype = 0;
    uint32_t parameter_type = 0;
    uint32_t event_type = 0;
    char subtype_string[8] = "0";
    char parameter_string[32] = "";
};

class SignalMenu : public Menu {
public:
    void SetEntity(id_t entity) { entity_id = entity; }
    void Display();
    uint32_t Layer() { return 2; }
    ~SignalMenu() = default;
protected:
    id_t entity_id = 0;
};

}

#endif