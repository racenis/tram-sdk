#include <extensions/menu/system.h>

#include <framework/settings.h>
#include <audio/audio.h>
#include <platform/api.h>

namespace tram::Ext::Menu {

std::string string_float(float value) {
    int value_num = floorf(value);
    int value_div = value * 10 - value_num * 10;
    return std::to_string(value_num) + "." + std::to_string(value_div);
}
    
void SystemMenu::Display() {
    uint32_t menu_height = 130;
    uint32_t menu_width = 100;
    uint32_t menu_offset = (UI::GetScreenHeight() / GUI::GetScaling() - menu_height) / 2;
    uint32_t menu_offset2 = (UI::GetScreenWidth() / GUI::GetScaling() - menu_width) / 2;
    
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
    uint32_t menu_offset = (UI::GetScreenHeight() / GUI::GetScaling() - menu_height) / 2;
    uint32_t menu_offset2 = (UI::GetScreenWidth() / GUI::GetScaling() - menu_width) / 2;
    
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
                GUI::NewLine();
                
                GUI::Text(1, "GUI Scale ");
                uint32_t gui_scale = GUI::GetScaling();
                if (GUI::RadioButton(1, gui_scale, "1") ||
                    GUI::RadioButton(2, gui_scale, "2") ||
                    GUI::RadioButton(3, gui_scale, "3")
                ) {
                    GUI::SetScaling(gui_scale);
                }
                GUI::NewLine();
                
                Value camera_shake = Settings::Get("camerashake");
                if (camera_shake.GetType() == TYPE_FLOAT32) {
                    float shake = camera_shake.GetFloat();
                    GUI::Text(1, "Camera shake ");
                    if (GUI::Slider(shake)) {
                        Settings::Set("camerashake", shake);
                    }
                    GUI::NewLine();
                }
                
            } break;
            case 1: {
                static bool inverse_x = false;
                static bool inverse_y = false;
                float sensitivity = fabsf(UI::GetAxisSensitivity(UI::KEY_MOUSE_X));
                GUI::Text(1, "Mouse sensitivity ");
                sensitivity /= 5.0f;
                bool changed_sensitivity = GUI::Slider(sensitivity, true, 150);
                sensitivity *= 5.0f;
                GUI::TextBox(string_float(sensitivity).c_str(), 50, 0);
                GUI::NewLine(GUI::LINE_NORMAL);
                changed_sensitivity |= GUI::CheckBox(inverse_x, "Invert mouse X axis"); GUI::NewLine(GUI::LINE_LOW);
                changed_sensitivity |= GUI::CheckBox(inverse_y, "Invert mouse Y axis"); GUI::NewLine(GUI::LINE_NORMAL);
                if (changed_sensitivity) {
                    UI::SetAxisSensitivity(UI::KEY_MOUSE_X, sensitivity * (inverse_x ? -1.0f : 1.0f));
                    UI::SetAxisSensitivity(UI::KEY_MOUSE_Y, sensitivity * (inverse_y ? -1.0f : 1.0f));
                }
                GUI::Text(1, binding_action ? "Key bindings ... press any key to bind ..." : "Key bindings"); GUI::NewLine(GUI::LINE_LOW);
                
                GUI::PushFrameRelative(GUI::FRAME_BOTTOM, 116);
                GUI::FillFrame(0, GUI::WIDGET_REVERSE_WINDOW);
                GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
                    
                    auto bindings = UI::GetAllKeyboardKeyBindings();

                    const uint32_t keys_per_page = 4;
                    uint32_t pages = (bindings.size() + keys_per_page - 1) / keys_per_page;
                    static uint32_t selected_page = 0;
                    
                    std::vector<std::pair<UI::keyboardaction_t, std::vector<UI::KeyboardKey>>> bindings_in_page;
                    for (uint32_t i = keys_per_page * selected_page; i < bindings.size() && bindings_in_page.size() < keys_per_page; i++) {
                        bindings_in_page.push_back(bindings[i]);
                    }
                    
                    for (auto& binding : bindings_in_page) {
                        GUI::Text(1, UI::GetKeyboardActionName(binding.first));
                        GUI::NewLine();
                    }
                    
                    GUI::PushFrameRelative(GUI::FRAME_RIGHT, 200);
                    for (auto& binding : bindings_in_page) {
                        std::string button_text = binding.second.size() ? "" : "none";
                        for (auto key : binding.second) {
                            if (button_text.size()) button_text += " / ";
                            button_text += UI::GetKeyboardKeyName(key);
                        }
                        if (GUI::Button(button_text.c_str())) {
                            binding_action = binding.first;
                            binding_state_ready = false;
                        };
                        GUI::NewLine();
                    }
                    GUI::PopFrame();
                    
                    if (binding_action) {
                        if (!binding_state_ready) {
                            bool keys_released = true;
                            for (uint16_t i = 0; i < UI::KeyboardKey::KEY_LASTKEY; i++) {
                                if (i == UI::KEY_ESCAPE || i == UI::KEY_GRAVE_ACCENT) continue;
                                if (UI::PollKeyboardKey((UI::KeyboardKey)i)) {
                                    keys_released = false;
                                }
                            }
                            if (keys_released) {
                                binding_state_ready = true;
                            }
                        } else {
                            for (uint16_t i = 0; i < UI::KeyboardKey::KEY_LASTKEY; i++) {
                                if (UI::PollKeyboardKey((UI::KeyboardKey)i)) {
                                    UI::BindKeyboardKey((UI::KeyboardKey)i, binding_action);
                                    binding_state_ready = false;
                                    binding_action = 0;
                                }
                            }
                        }
                    }
                    
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
                float fov = Render::GetViewFov() / 180.0f;
                float clp = Render::GetViewDistance() / 500.0f;
                
                GUI::Text(1, "Field of view"); GUI::NewLine();
                GUI::Text(1, "Render distance"); GUI::NewLine();
                bool vsync = Platform::Window::IsVsync();
                bool fullscreen = Platform::Window::IsFullscreen();
                if (GUI::CheckBox(vsync, "VSync")) Platform::Window::SetVsync(vsync);
                if (GUI::CheckBox(fullscreen, "Fullscreen")) Platform::Window::SetFullscreen(fullscreen);
                
                GUI::NewLine();
                
                uint32_t monitor = Platform::Window::GetCurrentMonitor();
                uint32_t monitor_count = Platform::Window::GetMonitorCount();
                
                GUI::Text(1, "Monitor ");
                if (GUI::RadioButton(0, monitor, "1", monitor_count > 0) ||
                    GUI::RadioButton(1, monitor, "2", monitor_count > 1) ||
                    GUI::RadioButton(2, monitor, "3", monitor_count > 2) ||
                    GUI::RadioButton(3, monitor, "4", monitor_count > 3)
                ) {
                    Platform::Window::SetMonitor(monitor);
                }
                
                GUI::NewLine();
                
                uint32_t resolution = 0;
                if (UI::GetScreenWidth() == 640 && UI::GetScreenHeight() == 480) {
                    resolution = 0;
                } else if (UI::GetScreenWidth() == 800 && UI::GetScreenHeight() == 600) {
                    resolution = 1;
                } else if (UI::GetScreenWidth() == 1024 && UI::GetScreenHeight() == 768) {
                    resolution = 2;
                } else {
                    resolution = 3;
                }
                
                if (GUI::RadioButton(0, resolution, "640x480")) UI::SetWindowSize(640, 480);
                if (GUI::RadioButton(1, resolution, "800x600")) UI::SetWindowSize(800, 600);
                if (GUI::RadioButton(2, resolution, "1024x768")) UI::SetWindowSize(1024, 768);
                GUI::RadioButton(3, resolution, "Other", false);
                
                GUI::PushFrameRelative(GUI::FRAME_RIGHT, 200);
                    bool fovch = GUI::Slider(fov, true, 150);
                    fov *= 180.0f;
                    if (fovch) Render::SetViewFov(fov);
                    GUI::TextBox((string_float(fov) + "\xb0").c_str(), 50);
                    
                    GUI::NewLine();
                    bool clpch = GUI::Slider(clp, true, 150);
                    clp *= 500.0f;
                    if (clpch) Render::SetViewDistance(clp);
                    GUI::TextBox((string_float(clp) + "m").c_str(), 50);
                GUI::PopFrame();
            } break;
            case 3: {
                GUI::Text(1, "Audio volume"); GUI::NewLine();
                
                float vol = Audio::GetVolume();
                
                GUI::PushFrameRelative(GUI::FRAME_RIGHT, 200);
                    vol /= 2.0f;
                    bool changed = GUI::Slider(vol, true, 150);
                    vol *= 2.0f;
                    GUI::TextBox(string_float(vol).c_str(), 50);
                    
                    if (changed) Audio::SetVolume(vol);
                GUI::PopFrame();
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