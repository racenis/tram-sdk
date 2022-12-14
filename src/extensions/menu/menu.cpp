// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/gui.h>
#include <framework/worldcell.h>

#include <physics/physics.h>
#include <render/render.h>


using namespace Core::GUI;

namespace Core::Ext::Menu {
    void DebugMenu() {
        static bool debugdraw_trans = false;
        static bool debugdraw_paths = false;
        static bool debugdraw_navmeshes = false;
        static bool debugdraw_stats = false;
        
        
        if (debugdraw_trans) for (auto& cell : PoolProxy<WorldCell>::GetPool()) if (cell.IsDrawn()) cell.DrawTransitions();
        if (debugdraw_paths) for (auto& cell : PoolProxy<WorldCell>::GetPool()) if (cell.IsDrawn()) cell.DrawPaths();
        if (debugdraw_navmeshes) for (auto& cell : PoolProxy<WorldCell>::GetPool()) if (cell.IsDrawn()) cell.DrawNavmeshes();
        
        if (debugdraw_stats) {
            char meminfobuffer[100]; char timeinfobuffer[100]; char fpsinfobuffer[100];
            //sprintf(meminfobuffer, "VRAM approximate usage: %lldKB", (RESOURCE_VRAM_USAGE / 1024));
            //sprintf(timeinfobuffer, "Frame time avg: %.2fms", Stats::final_time_avg[Stats::FRAME]);
            //sprintf(fpsinfobuffer, "FPS last second: %.2ffps", (1000.0f / Stats::final_time_avg[Stats::FRAME]));
            GUI::Text(meminfobuffer, 2); GUI::FrameBreakLine();
            GUI::Text(timeinfobuffer, 2); GUI::FrameBreakLine();
            GUI::Text(fpsinfobuffer, 2); GUI::FrameBreakLine();
        }
        
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
            Text(worldcell->GetName(), 1); FrameBreakLine();
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