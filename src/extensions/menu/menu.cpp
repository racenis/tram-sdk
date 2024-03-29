// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/gui.h>
#include <framework/stats.h>
#include <framework/worldcell.h>

#include <physics/physics.h>
#include <render/render.h>
#include <render/sprite.h>
#include <render/material.h>

#include <framework/system.h>

using namespace tram::GUI;
using namespace tram::Render;

namespace tram::Ext::Menu {

font_t FONT_SYMBOL = 0;     //< Font that contains sprites for GUI widgets.
font_t FONT_TEXT = 0;       //< Basic sans-serif font.
font_t FONT_TEXT_BOLD = 0;  //< Basic bold sans-serif font.
font_t FONT_HEADER = 0;     //< Font that contains some image headers.
font_t FONT_PIXELART = 0;   //< Large pixel-art font.

uint32_t MENU_SYSTEM = -1;

bool escape_menu_open = false;
bool debug_menu_open = false;

void ToggleMenuState() { UI::SetInputState((UI::GetInputState() == UI::STATE_DEFAULT) ? UI::STATE_MENU_OPEN : UI::STATE_DEFAULT); }

void EscapeMenuKeyboard () { 
    ToggleMenuState();
    escape_menu_open || debug_menu_open ? escape_menu_open = false, debug_menu_open = false : escape_menu_open = true;
}

void DebugMenuKeyboard () {
    ToggleMenuState();
    escape_menu_open || debug_menu_open ? escape_menu_open = false, debug_menu_open = false : debug_menu_open = true;
}

void Init() {
    assert(System::IsInitialized(System::SYSTEM_GUI) && "GUI system needs to be initialized first!");
    assert(MENU_SYSTEM == -1u && "Menu system is already initialized!");
    
    Material::Make(UID("ui/font_symbols"),     MATERIAL_GLYPH)->Load();
    Material::Make(UID("ui/font_text"),        MATERIAL_GLYPH)->Load();
    Material::Make(UID("ui/font_text_bold"),   MATERIAL_GLYPH)->Load();
    Material::Make(UID("ui/font_headers"),     MATERIAL_GLYPH)->Load();
    Material::Make(UID("ui/font_pixelart"),    MATERIAL_GLYPH)->Load();

    Sprite* font_symbols =      Sprite::Find(UID("font_symbols"));
    Sprite* font_text =         Sprite::Find(UID("font_text"));
    Sprite* font_text_bold =    Sprite::Find(UID("font_text_bold"));
    Sprite* font_headers =      Sprite::Find(UID("font_headers"));
    Sprite* font_pixelart =     Sprite::Find(UID("font_pixelart"));

    font_symbols->Load();
    font_text->Load();
    font_text_bold->Load();
    font_headers->Load();
    font_pixelart->Load();

    FONT_SYMBOL =       GUI::RegisterFont(font_symbols);
    FONT_TEXT =         GUI::RegisterFont(font_text);
    FONT_TEXT_BOLD =    GUI::RegisterFont(font_text_bold);
    FONT_HEADER =       GUI::RegisterFont(font_headers);
    FONT_PIXELART =     GUI::RegisterFont(font_pixelart);
    
    UI::BindKeyboardKey(UI::KEY_ESCAPE, EscapeMenuKeyboard);
    UI::BindKeyboardKey(UI::KEY_GRAVE_ACCENT, DebugMenuKeyboard);
    
    MENU_SYSTEM = System::Register("Default Menus", "MENU");
    System::SetInitialized(MENU_SYSTEM, true);
}

void DebugMenu() {
    static bool debugdraw_trans = false;
    static bool debugdraw_paths = false;
    static bool debugdraw_navmeshes = false;
    static bool debugdraw_stats = false;
    
    
    //if (debugdraw_trans) for (auto& cell : PoolProxy<WorldCell>::GetPool()) if (cell.IsDebugDraw()) cell.DrawTransitions();
    //if (debugdraw_paths) for (auto& cell : PoolProxy<WorldCell>::GetPool()) if (cell.IsDebugDraw()) cell.DrawPaths();
    //if (debugdraw_navmeshes) for (auto& cell : PoolProxy<WorldCell>::GetPool()) if (cell.IsDebugDraw()) cell.DrawNavmeshes();
    
    if (debugdraw_stats) {
        char meminfobuffer[100]; char timeinfobuffer[100]; char fpsinfobuffer[100];
        sprintf(meminfobuffer, "VRAM approximate usage: %lldKB", (Stats::GetStat(Stats::RESOURCE_VRAM) / 1024));
        sprintf(timeinfobuffer, "Frame time avg: %.2fms", Stats::GetStat(System::SYSTEM_RENDER));
        sprintf(fpsinfobuffer, "FPS last second: %.2ffps", (1000.0f / Stats::GetStat(System::SYSTEM_RENDER)));
        GUI::Text(meminfobuffer, 2); GUI::FrameBreakLine();
        GUI::Text(timeinfobuffer, 2); GUI::FrameBreakLine();
        GUI::Text(fpsinfobuffer, 2); GUI::FrameBreakLine();
    }
    
    if (!debug_menu_open) return;
    
    static bool worldcell_menu_open = false;
    static bool debugdraw_menu_open = false;
    
    Frame(FRAME_TOP, 30);
    if (Button("WorldCell")) worldcell_menu_open = true;
    if (Button("Debug Drawing")) debugdraw_menu_open = true;
    EndFrame();
    
    if (debugdraw_menu_open) {
        Frame(FRAME_CENTER, 320, 240);
        GUI::FillFrame(104.0f, 88.0f, 12.0f, 12.0f, Render::COLOR_WHITE, 0);
        Frame(FRAME_RIGHT, 20); if (SmallButton(BUTTON_CROSS)) debugdraw_menu_open = false; EndFrame();
        Frame(FRAME_RIGHT, 320-4);
        Text("Debug Drawing", 2); FrameBreakLine();
        
        Frame(FRAME_BOTTOM, 240-50);
        Frame(FRAME_LEFT, 80);
        Text("Render:", 1); FrameBreakLine();
        Text("Physics:", 1); FrameBreakLine();
        Text("Transitions:", 1); FrameBreakLine();
        Text("Paths:", 1); FrameBreakLine();
        Text("Navmeshes:", 1); FrameBreakLine();
        Text("Statistics:", 1); FrameBreakLine();
        EndFrame();
        
        Frame(FRAME_RIGHT, 320-100);
        CheckBox(Render::DRAW_RENDER_DEBUG); FrameBreakLine();
        CheckBox(Physics::DRAW_PHYSICS_DEBUG); FrameBreakLine();
        CheckBox(debugdraw_trans); FrameBreakLine();
        CheckBox(debugdraw_paths); FrameBreakLine();
        CheckBox(debugdraw_navmeshes); FrameBreakLine();
        CheckBox(debugdraw_stats); FrameBreakLine();
        EndFrame();
        EndFrame();
        
        EndFrame();
        EndFrame();
    }
    
    if (worldcell_menu_open) {
        Frame(FRAME_CENTER, 320, 240);
        GUI::FillFrame(104.0f, 88.0f, 12.0f, 12.0f, Render::COLOR_WHITE, 0);
        Frame(FRAME_RIGHT, 20); if (SmallButton(BUTTON_CROSS)) worldcell_menu_open = false; EndFrame();
        Frame(FRAME_RIGHT, 320-4);
        Text("WorldCell Inspector", 2); FrameBreakLine();
        static std::vector<char const*> worldcell_names;
        static uint32_t worldcell_selected = 0;
        worldcell_names.clear();
        auto& worldcell_pool = PoolProxy<WorldCell>::GetPool();
        for (size_t i = 0; i < worldcell_pool.GetSize(); i++) if (*((uint64_t*)(worldcell_pool.begin().ptr + i))!=0) worldcell_names.push_back((char const*)(worldcell_pool.begin().ptr + i)->GetName()); else worldcell_names.push_back((char*)"[NONE]");
        DropdownBox((const char**)worldcell_names.data(), worldcell_names.size(), worldcell_selected);
        FrameBreakLine();
        
        auto worldcell = worldcell_pool.begin().ptr + worldcell_selected;
        char numbuffer[20];
        std::sprintf(numbuffer, "%lld", worldcell->GetEntityCount());
        bool is_draw = worldcell->IsDebugDraw();
        bool is_load = worldcell->IsLoaded();
        bool is_int = worldcell->IsInterior();
        bool is_intl = worldcell->HasInteriorLighting();
        bool is_draw_bef = is_draw;
        
        Frame(FRAME_BOTTOM, 240-50);
        Frame(FRAME_LEFT, 80);
        Text("Name:", 1); FrameBreakLine();
        Text("Entities:", 1); FrameBreakLine();
        Text("Loaded:", 1); FrameBreakLine();
        Text("Interior:", 1); FrameBreakLine();
        Text("Int. light:", 1); FrameBreakLine();
        Text("Draw:", 1); FrameBreakLine();
        EndFrame();
        
        Frame(FRAME_RIGHT, 320-80);
        Text(worldcell->GetName(), 1); FrameBreakLine();
        Text(numbuffer, 1); FrameBreakLine();
        CheckBox(is_load); FrameBreakLine();
        CheckBox(is_int); FrameBreakLine();
        CheckBox(is_intl); FrameBreakLine();
        CheckBox(is_draw); FrameBreakLine();
        EndFrame();
        EndFrame();
        
        if(is_draw != is_draw_bef) worldcell->SetDebugDraw(is_draw);
        
        EndFrame();
        EndFrame();
    }
}

void EscapeMenu() {
    if (!escape_menu_open) return;
    
    Frame(FRAME_TOP, 20);
    
    Text("Escape Menu", 2);
    
    EndFrame();
}

}
