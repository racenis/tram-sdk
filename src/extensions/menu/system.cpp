#include <extensions/menu/system.h>

namespace tram::Ext::Menu {
    
void SystemMenu::Display() {
    uint32_t menu_height = 130;
    uint32_t menu_width = 100;
    uint32_t menu_offset = (UI::GetScreenHeight() - menu_height) / 2;
    uint32_t menu_offset2 = (UI::GetScreenWidth() - menu_width) / 2;
    
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, menu_offset);
    GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, menu_offset2);
    GUI::PushFrameRelative(GUI::FRAME_TOP, menu_height);
    GUI::PushFrameRelative(GUI::FRAME_LEFT, menu_width);
    GUI::FillFrame(0, GUI::WIDGET_WINDOW);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        GUI::Button("New", false, 90);
        GUI::NewLine();
        GUI::Button("Load", false, 90);
        GUI::NewLine();
        GUI::Button("Save", false, 90);
        GUI::NewLine();
        if (GUI::Button("Settings", true, 90)) {
            Menu::Push(new SettingsMenu);
        }
        GUI::NewLine();
        if (GUI::Button("Exit", true, 90)) {
            
        }
    
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    
    
    
    
    GUI::PushFrameRelative(GUI::FRAME_BOTTOM, 24);
    GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 5);
    GUI::Text(1, GetVersion());
    GUI::Text(1, " open-source graphics package. To get help, visit http://github.com/racenis/tram-sdk/");
    
    GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 1);
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 1);
    GUI::SetColor(Render::COLOR_WHITE);
    
    GUI::Text(1, GetVersion());
    GUI::Text(1, " open-source graphics package. To get help, visit http://github.com/racenis/tram-sdk/");
    
    GUI::SetColor(Render::COLOR_BLACK);
    GUI::PopFrame();
    GUI::PopFrame();
    
    GUI::PopFrame();
    GUI::PopFrame();
    
    
    
    
    
}

void SettingsMenu::Display() {
    uint32_t menu_height = 240;
    uint32_t menu_width = 320;
    uint32_t menu_offset = (UI::GetScreenHeight() - menu_height) / 2;
    uint32_t menu_offset2 = (UI::GetScreenWidth() - menu_width) / 2;
    
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, menu_offset);
    GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, menu_offset2);
    GUI::PushFrameRelative(GUI::FRAME_TOP, menu_height);
    GUI::PushFrameRelative(GUI::FRAME_LEFT, menu_width);
    GUI::FillFrame(0, GUI::WIDGET_WINDOW);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        GUI::RadioButton(0, tab, "Engine ");
        GUI::RadioButton(1, tab, "Input ");
        GUI::RadioButton(2, tab, "Video ");
        GUI::RadioButton(3, tab, "Audio ");
    
        GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 22);
        GUI::FillFrame(0, GUI::WIDGET_REVERSE_WINDOW);
        GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        switch (tab) {
            default: {
                bool enable_debug = true;
                GUI::CheckBox(enable_debug, "Enable debug mode", false);
            } break;
            case 1: {
                bool inverse_x = false;
                bool inverse_y = false;
                GUI::Text(1, "Mouse sensitivity (TODO: implement)"); GUI::NewLine(GUI::LINE_NORMAL);
                GUI::CheckBox(inverse_x, "Invert mouse X axis"); GUI::NewLine(GUI::LINE_LOW);
                GUI::CheckBox(inverse_y, "Invert mouse Y axis"); GUI::NewLine(GUI::LINE_LOW);
                GUI::Text(1, "Key bindings"); GUI::NewLine(GUI::LINE_LOW);
                
                GUI::PushFrameRelative(GUI::FRAME_BOTTOM, 140);
                GUI::FillFrame(0, GUI::WIDGET_REVERSE_WINDOW);
                GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
                    
                    uint32_t pages = 5;
                    static uint32_t selected_page = 0;
                    
                    GUI::PushFrameRelative(GUI::FRAME_RIGHT, 25);
                    for (uint32_t i = 0; i < pages; i++) {
                        char str[2] = "1";
                        *str += i;
                        GUI::RadioButton(i, selected_page, str);
                        GUI::NewLine(GUI::LINE_LOW);
                    }
                    GUI::PopFrame();
                    
                GUI::PopFrame();
                GUI::PopFrame();
            } break;
            case 2: {
                GUI::Text(1, "Field of view (TODO: implement)"); GUI::NewLine();
                GUI::Text(1, "Render distance (TODO: implement)"); GUI::NewLine();
            } break;
            case 3: {
                GUI::Text(1, "Audio volume (TODO: implement)"); GUI::NewLine();
            } break;
            }
        GUI::PopFrame();
        GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}

}