// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// MENU.CPP -- implementations of some default menus

#include <gui.h>

namespace Core::GUI {
    void DebugMenu() {
        static bool debugdraw_lines = false;
        
        if (debugdraw_lines) for (size_t i = 0; i < PoolProxy<WorldCell>::GetPool().GetSize(); i++) if (*((uint64_t*)(PoolProxy<WorldCell>::GetPool().begin() + i))!=0) if( (PoolProxy<WorldCell>::GetPool().begin() + i)->IsDrawn())  (PoolProxy<WorldCell>::GetPool().begin() + i)->Draw();
            
        
        
        
        if (!UI::debug_menu_open) return;
        
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
            Text("Physics:", 1); FrameBreakLine();
            Text("Transitions:", 1); FrameBreakLine();
            EndFrame();
            
            Frame(FRAME_RIGHT, 320-100);
            CheckBox(Render::DRAW_PHYSICS_DEBUG); FrameBreakLine();
            CheckBox(debugdraw_lines); FrameBreakLine();
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
            static std::vector<char*> worldcell_names;
            static uint32_t worldcell_selected = 0;
            worldcell_names.clear();
            auto& worldcell_pool = PoolProxy<WorldCell>::GetPool();
            for (size_t i = 0; i < worldcell_pool.GetSize(); i++) if (*((uint64_t*)(worldcell_pool.begin() + i))!=0) worldcell_names.push_back((char*)ReverseUID((worldcell_pool.begin() + i)->GetName())); else worldcell_names.push_back((char*)"[NONE]");
            DropdownBox((const char**)worldcell_names.data(), worldcell_names.size(), worldcell_selected);
            FrameBreakLine();
            
            auto worldcell = worldcell_pool.begin() + worldcell_selected;
            char numbuffer[20];
            std::sprintf(numbuffer, "%lld", worldcell->EntityCount());
            bool is_draw = worldcell->IsDrawn();
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
            Text(ReverseUID(worldcell->GetName()), 1); FrameBreakLine();
            Text(numbuffer, 1); FrameBreakLine();
            CheckBox(is_load); FrameBreakLine();
            CheckBox(is_int); FrameBreakLine();
            CheckBox(is_intl); FrameBreakLine();
            CheckBox(is_draw); FrameBreakLine();
            EndFrame();
            EndFrame();
            
            if(is_draw != is_draw_bef) worldcell->SetDrawn(is_draw);
            
            EndFrame();
            EndFrame();
        }
    }
    
    void EscapeMenu() {
        if (!UI::escape_menu_open) return;
        
        Frame(FRAME_TOP, 20);
        
        Text("Escape Menu", 2);
        
        EndFrame();
    }
}