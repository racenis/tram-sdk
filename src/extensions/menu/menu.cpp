// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/gui.h>
#include <framework/stats.h>
#include <framework/worldcell.h>
#include <framework/system.h>

#include <physics/physics.h>
#include <render/render.h>
#include <render/sprite.h>
#include <render/material.h>

#include <templates/stack.h>

#include <extensions/menu/menu.h>
#include <extensions/menu/debug.h>
#include <extensions/menu/system.h>

#include <algorithm>

/**
 * @namespace tram::Ext::Menu
 * 
 * Menu framework.
 * 
 * Built on top of the GUI system, this menu framework allows the creation of
 * menus. 
 * It also has some pre-built menus, like the debug menu and the system menu.
 */

/**
 * @namespace tram::Ext::Menu::Menu
 * 
 * Menu base interface class.
 * 
 * To implement this interface, implement the Display() method. This method will
 * be called once per frame and needs to draw the menu.
 * Then implement the Layer() method. This method needs to return the layer
 * index of the menu. 
 * 
 * The menu framework will draw only the topmost menu in the menu stack with any
 * given index.
 * 
 * When you are ready to deploy a menu, create it using `new` and Push() it on
 * the stack. After that, you can forget it. When it gets popped from the stack,
 * it will be `delete`d for you.
 */

using namespace tram::GUI;
using namespace tram::Render;

namespace tram::Ext::Menu {

// TODO: switch to invalid font initialization (zero is valid)
font_t FONT_WIDGETS = 0;    //< Font that contains sprites for GUI widgets.
font_t FONT_TEXT = 0;       //< Basic sans-serif font.
font_t FONT_TEXT_BOLD = 0;  //< Basic bold sans-serif font.
font_t FONT_HEADER = 0;     //< Font that contains some image headers.
font_t FONT_PIXELART = 0;   //< Large pixel-art font.

uint32_t MENU_SYSTEM = -1;

DebugMenu* debug_menu = nullptr;

// replace menu stack with just menu list
// and make menu list act like menu stack
// ALSO make sure that menu list is copied before iterating
// TODO: fix pls
std::vector<Menu*> menu_stack;
std::vector<Menu*> menu_list;

// is this even used??
void ToggleMenuState() { UI::SetInputState((UI::GetInputState() == UI::STATE_DEFAULT) ? UI::STATE_MENU_OPEN : UI::STATE_DEFAULT); }

// is this used either?
// TODO: fix
void CloseAll() {
    if (debug_menu) Menu::Remove(debug_menu);
    
    debug_menu = nullptr;
}

static void EscapeMenuKeyboard () {
    /*if (Menu::Pop()) {
        if (Menu::Empty()) {
            UI::SetInputState(UI::STATE_DEFAULT);
        }
    }*/
    
    if (Menu::Empty()) {
        SystemMenu* system_menu = new SystemMenu();
        Menu::Push(system_menu);
        UI::SetInputState(UI::STATE_MENU_OPEN);
    } else {
        Menu::Pop();
        
        //Menu::Clear();
        if (Menu::Empty()) {
            UI::SetInputState(UI::STATE_DEFAULT);
        }
        
    }
    
    //CloseAll();
    
    
    /*if (debug_menu) {
        CloseAll();
        UI::SetInputState(UI::STATE_DEFAULT);
    } else  {
        CloseAll();
        debug_menu = new DebugMenu();
        Menu::Add(debug_menu);
        UI::SetInputState(UI::STATE_MENU_OPEN);
    }*/
}

static void DebugMenuKeyboard () {
    if (Menu::Empty()) {
        debug_menu = new DebugMenu();
        Menu::Push(debug_menu);
        UI::SetInputState(UI::STATE_MENU_OPEN);
    } else {
        Menu::Clear();
        UI::SetInputState(UI::STATE_DEFAULT);
    }
}

void Init() {
    assert(System::IsInitialized(System::GUI) && "GUI system needs to be initialized first!");
    assert(MENU_SYSTEM == -1u && "Menu system is already initialized!");
    
    Material::Make(UID("ui/font_widgets"),     MATERIAL_GLYPH)->Load();
    Material::Make(UID("ui/font_text"),        MATERIAL_GLYPH)->Load();
    Material::Make(UID("ui/font_text_bold"),   MATERIAL_GLYPH)->Load();
    Material::Make(UID("ui/font_headers"),     MATERIAL_GLYPH)->Load();
    Material::Make(UID("ui/font_pixelart"),    MATERIAL_GLYPH)->Load();

    Sprite* font_widgets =      Sprite::Find(UID("font_widgets"));
    Sprite* font_text =         Sprite::Find(UID("font_text"));
    Sprite* font_text_bold =    Sprite::Find(UID("font_text_bold"));
    Sprite* font_headers =      Sprite::Find(UID("font_headers"));
    Sprite* font_pixelart =     Sprite::Find(UID("font_pixelart"));

    font_widgets->Load();
    font_text->Load();
    font_text_bold->Load();
    font_headers->Load();
    font_pixelart->Load();

    FONT_WIDGETS =      GUI::RegisterFont(font_widgets);
    FONT_TEXT =         GUI::RegisterFont(font_text);
    FONT_TEXT_BOLD =    GUI::RegisterFont(font_text_bold);
    FONT_HEADER =       GUI::RegisterFont(font_headers);
    FONT_PIXELART =     GUI::RegisterFont(font_pixelart);
    
    UI::BindKeyboardKey(UI::KEY_ESCAPE, EscapeMenuKeyboard);
    UI::BindKeyboardKey(UI::KEY_GRAVE_ACCENT, DebugMenuKeyboard);
    
    InitCallbacks();
    
    MENU_SYSTEM = System::Register("Default Menus", "MENU");
    System::SetInitialized(MENU_SYSTEM, true);
}

void Update() {
    GUI::SetColor({0.0f, 0.0f, 0.0f});
    
    auto menu_list_copy = menu_list;
    for (auto menu : menu_list_copy) {
        menu->Display();
    }
    
    // TODO: cop[y menu_stack
    bool displayed[3] = {false, false, false};
    auto menu_stack_copy = menu_stack;
    for (auto menu = menu_stack_copy.rbegin(); menu != menu_stack_copy.rend(); menu++) {

        if (uint32_t layer = (*menu)->Layer(); !displayed[layer]) {
            displayed[layer] = true;
            (*menu)->Display();
        }
        
        
        //menu.
        //*menu->Display();
    }
    /*if (menu_stack.size()) {
        menu_stack.top()->Display();
    }*/
    
    UpdateCallbacks();
}

/// Pushes menu on the menu stack.
void Menu::Push(Menu* menu) {
    //*menu_stack.AddNew() = menu;
    menu_stack.push_back(menu);
}

/// Pops the topmost menu from the stack
/// @note   The menu will be `delete`d after removing it from the stack.
/// @return True if a menu was popped, false if stack was empty.
bool Menu::Pop() {
    if (menu_stack.size()) {
        delete menu_stack.back();
        menu_stack.pop_back();
        return true;
    } else {
        return false;
    }
}

/// Checks whether the menu stack is empty.
/// @return True if it is empty.
bool Menu::Empty() {
    return !menu_stack.size();
}

/// Clears the menu stack.
/// This will behave as if Menu::Pop() was called until the menu stack was
/// empty.
/// @note All of the menus in the stack will be `delete`ed.
void Menu::Clear() {
    while(menu_stack.size()) {
        // maybe instead cal menu::pop??? TODO:fix
        delete menu_stack.back();
        menu_stack.pop_back();
    }
}

// tbh idk what this even does and if it is even used
// TODO: look up what it does and what uses it 
void Menu::Add(Menu* menu) {
    menu_list.push_back(menu);
}

// same as previours
// TODO: fix
void Menu::Remove(Menu* menu) {
    auto thing = std::find(menu_list.begin(), menu_list.end(), menu);
    
    // TODO:  fix memory leak
    //delete *thing;
    menu_list.erase(thing);
}

void DebugMenue() {
    //static bool debugdraw_trans = false;
    //static bool debugdraw_paths = false;
    //static bool debugdraw_navmeshes = false;
    //static bool debugdraw_stats = false;
    
    
    //if (debugdraw_trans) for (auto& cell : PoolProxy<WorldCell>::GetPool()) if (cell.IsDebugDraw()) cell.DrawTransitions();
    //if (debugdraw_paths) for (auto& cell : PoolProxy<WorldCell>::GetPool()) if (cell.IsDebugDraw()) cell.DrawPaths();
    //if (debugdraw_navmeshes) for (auto& cell : PoolProxy<WorldCell>::GetPool()) if (cell.IsDebugDraw()) cell.DrawNavmeshes();
    
    //if (debugdraw_stats) {
        //char meminfobuffer[100]; char timeinfobuffer[100]; char fpsinfobuffer[100];
        //sprintf(meminfobuffer, "VRAM approximate usage: %lldKB", (Stats::GetStat(Stats::RESOURCE_VRAM) / 1024));
        //sprintf(timeinfobuffer, "Frame time avg: %.2fms", Stats::GetStat(System::RENDER));
        //sprintf(fpsinfobuffer, "FPS last second: %.2ffps", (1000.0f / Stats::GetStat(System::RENDER)));
        //GUI::Text(meminfobuffer, 2); GUI::FrameBreakLine();
        //GUI::Text(timeinfobuffer, 2); GUI::FrameBreakLine();
        //GUI::Text(fpsinfobuffer, 2); GUI::FrameBreakLine();
    //}
    
    //if (!debug_menu_open) return;
    
    static bool worldcell_menu_open = false;
    static bool debugdraw_menu_open = false;
    
    //Frame(FRAME_TOP, 30);
    //if (Button("WorldCell")) worldcell_menu_open = true;
    //if (Button("Debug Drawing")) debugdraw_menu_open = true;
    //EndFrame();
    
    if (debugdraw_menu_open) {
        //Frame(FRAME_CENTER, 320, 240);
        //GUI::FillFrame(104.0f, 88.0f, 12.0f, 12.0f, Render::COLOR_WHITE, 0);
        //Frame(FRAME_RIGHT, 20); if (SmallButton(BUTTON_CROSS)) debugdraw_menu_open = false; EndFrame();
        //Frame(FRAME_RIGHT, 320-4);
        //Text("Debug Drawing", 2); FrameBreakLine();
        
        //Frame(FRAME_BOTTOM, 240-50);
        //Frame(FRAME_LEFT, 80);
        //Text("Render:", 1); FrameBreakLine();
        //Text("Physics:", 1); FrameBreakLine();
        //Text("Transitions:", 1); FrameBreakLine();
        //Text("Paths:", 1); FrameBreakLine();
        //Text("Navmeshes:", 1); FrameBreakLine();
        //Text("Statistics:", 1); FrameBreakLine();
        //EndFrame();
        
        //Frame(FRAME_RIGHT, 320-100);
        //CheckBox(Render::DRAW_RENDER_DEBUG); FrameBreakLine();
        //CheckBox(Physics::DRAW_PHYSICS_DEBUG); FrameBreakLine();
        //CheckBox(debugdraw_trans); FrameBreakLine();
        //CheckBox(debugdraw_paths); FrameBreakLine();
        //CheckBox(debugdraw_navmeshes); FrameBreakLine();
        //CheckBox(debugdraw_stats); FrameBreakLine();
        //EndFrame();
        //EndFrame();
        
        //EndFrame();
        //EndFrame();
    }
    
    if (worldcell_menu_open) {
        //Frame(FRAME_CENTER, 320, 240);
        //GUI::FillFrame(104.0f, 88.0f, 12.0f, 12.0f, Render::COLOR_WHITE, 0);
        //Frame(FRAME_RIGHT, 20); if (SmallButton(BUTTON_CROSS)) worldcell_menu_open = false; EndFrame();
        //Frame(FRAME_RIGHT, 320-4);
        //Text("WorldCell Inspector", 2); FrameBreakLine();
        static std::vector<char const*> worldcell_names;
        static uint32_t worldcell_selected = 0;
        worldcell_names.clear();
        auto& worldcell_pool = PoolProxy<WorldCell>::GetPool();
        for (size_t i = 0; i < worldcell_pool.GetSize(); i++) if (*((uint64_t*)(worldcell_pool.begin().ptr + i))!=0) worldcell_names.push_back((char const*)(worldcell_pool.begin().ptr + i)->GetName()); else worldcell_names.push_back((char*)"[NONE]");
        //DropdownBox((const char**)worldcell_names.data(), worldcell_names.size(), worldcell_selected);
        //FrameBreakLine();
        
        auto worldcell = worldcell_pool.begin().ptr + worldcell_selected;
        char numbuffer[20];
        std::sprintf(numbuffer, "%lld", worldcell->GetEntityCount());
        //bool is_draw = worldcell->IsDebugDraw();
        //bool is_load = worldcell->IsLoaded();
        //bool is_int = worldcell->IsInterior();
        //bool is_intl = worldcell->HasInteriorLighting();
        //bool is_draw_bef = is_draw;
        
        //Frame(FRAME_BOTTOM, 240-50);
        //Frame(FRAME_LEFT, 80);
        //Text("Name:", 1); FrameBreakLine();
        //Text("Entities:", 1); FrameBreakLine();
        //Text("Loaded:", 1); FrameBreakLine();
        //Text("Interior:", 1); FrameBreakLine();
        //Text("Int. light:", 1); FrameBreakLine();
        //Text("Draw:", 1); FrameBreakLine();
        //EndFrame();
        
        //Frame(FRAME_RIGHT, 320-80);
        //Text(worldcell->GetName(), 1); FrameBreakLine();
        //Text(numbuffer, 1); FrameBreakLine();
        //CheckBox(is_load); FrameBreakLine();
        //CheckBox(is_int); FrameBreakLine();
        //CheckBox(is_intl); FrameBreakLine();
        //CheckBox(is_draw); FrameBreakLine();
        //EndFrame();
        //EndFrame();
        
        //if(is_draw != is_draw_bef) worldcell->SetDebugDraw(is_draw);
        
        //EndFrame();
        //EndFrame();
    }
}

//void EscapeMenu() {
    //if (!escape_menu_open) return;
    
    //Frame(FRAME_TOP, 20);
    
    //Text("Escape Menu", 2);
    
    //EndFrame();
//}

}
