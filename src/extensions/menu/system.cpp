// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/menu/system.h>

#include <framework/settings.h>
#include <framework/serialization.h>
#include <audio/audio.h>
#include <platform/api.h>
#include <platform/file.h>

#include <filesystem>

#include <config.h>

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
    GUI::FillFrame(GUI::WIDGET_WINDOW);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        if (GUI::Button("New", true, 90)) {
            Menu::Push(new SaveMenu(false, false, false));
        }
        GUI::NewLine();
        if (GUI::Button("Save", true, 90)) {
            Menu::Push(new SaveMenu(true, false, false));
        }
        GUI::NewLine();
        if (GUI::Button("Load", true, 90)) {
            Menu::Push(new SaveMenu(false, true, false));
        }
        GUI::NewLine();
        if (GUI::Button("Settings", true, 90)) {
            Menu::Push(new SettingsMenu);
        }
        GUI::NewLine();
        if (GUI::Button("Exit", true, 90)) {
            Menu::Push(new SaveMenu(false, false, true));
        }
    
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    
    
    
    
    GUI::PushFrameRelative(GUI::FRAME_BOTTOM, 24);
    GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 5);
    GUI::Text(GetVersion());
    GUI::Text(" open-source graphics package. To get help, visit http://github.com/racenis/tram-sdk/");
    
    GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 1);
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 1);
    GUI::SetColor(Render::COLOR_WHITE);
    
    GUI::Text(GetVersion());
    GUI::Text(" open-source graphics package. To get help, visit http://github.com/racenis/tram-sdk/");
    
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
    GUI::FillFrame(GUI::WIDGET_WINDOW);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        GUI::RadioButton(0, tab, "Engine ");
        GUI::RadioButton(1, tab, "Input ");
        GUI::RadioButton(2, tab, "Video ");
        GUI::RadioButton(3, tab, "Audio ");
    
        GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 22);
        GUI::FillFrame(GUI::WIDGET_REVERSE_WINDOW);
        GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        switch (tab) {
            default: {
                bool enable_debug = Settings::Get("enable-debug");
                if (GUI::CheckBox(enable_debug, "Enable debug mode")) {
                    Settings::Set("enable-debug", enable_debug);
                    Settings::SetFlag("enable-debug", Settings::MODIFIED, true);
                }
                GUI::NewLine();
                
                GUI::Text("GUI Scale ");
                uint32_t gui_scale = GUI::GetScaling();
                if (GUI::RadioButton(1, gui_scale, "1") ||
                    GUI::RadioButton(2, gui_scale, "2") ||
                    GUI::RadioButton(3, gui_scale, "3")
                ) {
                    GUI::SetScaling(gui_scale);
                    Settings::SetFlag("gui-scale", Settings::MODIFIED, true);
                }
                GUI::NewLine();
                
                Value camera_shake = Settings::Get("camerashake");
                if (camera_shake.GetType() == TYPE_FLOAT32) {
                    float shake = camera_shake.GetFloat();
                    GUI::Text("Camera shake ");
                    if (GUI::Slider(shake)) {
                        Settings::Set("camerashake", shake);
                        Settings::SetFlag("camerashake", Settings::MODIFIED, true);
                    }
                    GUI::NewLine();
                }
                
            } break;
            case 1: {
                static bool inverse_x = false;
                static bool inverse_y = false;
                float sensitivity = fabsf(UI::GetAxisSensitivity(UI::KEY_MOUSE_X));
                GUI::Text("Mouse sensitivity ");
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
                GUI::Text(binding_action ? "Key bindings ... press any key to bind ..." : "Key bindings"); GUI::NewLine(GUI::LINE_LOW);
                
                GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, 150);
                GUI::NewLine(GUI::LINE_NORMAL);
                bool raw_input = Platform::Window::IsRawInput();
                if (GUI::CheckBox(raw_input, "Raw input")) {
                    Platform::Window::SetRawInput(raw_input);
                }
                GUI::PopFrame();
                
                GUI::PushFrameRelative(GUI::FRAME_BOTTOM, 116);
                GUI::FillFrame(GUI::WIDGET_REVERSE_WINDOW);
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
                        GUI::Text(UI::GetKeyboardActionName(binding.first));
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
                float clp = Render::GetViewDistance() / 2.0f;
                
                GUI::Text("Field of view"); GUI::NewLine();
                GUI::Text("Render distance"); GUI::NewLine();
                bool vsync = Platform::Window::IsVsync();
                bool fullscreen = Platform::Window::IsFullscreen();
                if (GUI::CheckBox(vsync, "VSync")) {
                    Platform::Window::SetVsync(vsync);
                    Settings::SetFlag("window-vsync", Settings::MODIFIED, true);
                }
                if (GUI::CheckBox(fullscreen, "Fullscreen")) {
                    Platform::Window::SetFullscreen(fullscreen);
                    Settings::SetFlag("window-fullscreen", Settings::MODIFIED, true);
                }
                
                GUI::NewLine();
                
                uint32_t monitor = Platform::Window::GetCurrentMonitor();
                uint32_t monitor_count = Platform::Window::GetMonitorCount();
                
                GUI::Text("Monitor ");
                if (GUI::RadioButton(0, monitor, "1", monitor_count > 0) ||
                    GUI::RadioButton(1, monitor, "2", monitor_count > 1) ||
                    GUI::RadioButton(2, monitor, "3", monitor_count > 2) ||
                    GUI::RadioButton(3, monitor, "4", monitor_count > 3)
                ) {
                    Platform::Window::SetMonitor(monitor);
                    Settings::SetFlag("window-monitor", Settings::MODIFIED, true);
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
                
                bool reschange = false;
                if (GUI::RadioButton(0, resolution, "640x480")) UI::SetWindowSize(640, 480), reschange = true;
                if (GUI::RadioButton(1, resolution, "800x600")) UI::SetWindowSize(800, 600), reschange = true;
                if (GUI::RadioButton(2, resolution, "1024x768")) UI::SetWindowSize(1024, 768), reschange = true;
                GUI::RadioButton(3, resolution, "Other", false);
                if (reschange) {
                    Settings::SetFlag("window-width", Settings::MODIFIED, true);
                    Settings::SetFlag("window-height", Settings::MODIFIED, true);
                }
                
                GUI::PushFrameRelative(GUI::FRAME_RIGHT, 200);
                    bool fovch = GUI::Slider(fov, true, 150);
                    fov *= 180.0f;
                    if (fovch) {
                        Render::SetViewFov(fov, 0);
                        Render::SetViewFov(fov, 1);
                        Settings::Set("render-fov", fov);
                        Settings::SetFlag("render-fov", Settings::MODIFIED, true);
                    }
                    GUI::TextBox((string_float(fov) + "\xb0").c_str(), 50);
                    
                    GUI::NewLine();
                    bool clpch = GUI::Slider(clp, true, 150);
                    clp *= 2.0f;
                    if (clpch) {
                        Render::SetViewDistance(clp, 0);
                        Render::SetViewDistance(clp, 1);
                        Settings::Set("render-dist", clp);
                        Settings::SetFlag("render-dist", Settings::MODIFIED, true);
                    }
                    GUI::TextBox((string_float(clp) + "x").c_str(), 50);
                GUI::PopFrame();
            } break;
            case 3: {
                GUI::Text("Audio volume"); GUI::NewLine();
                
                float vol = Audio::GetVolume();
                
                GUI::PushFrameRelative(GUI::FRAME_RIGHT, 200);
                    vol /= 2.0f;
                    bool changed = GUI::Slider(vol, true, 150);
                    vol *= 2.0f;
                    GUI::TextBox(string_float(vol).c_str(), 50);
                    
                    if (changed) {
                        Audio::SetVolume(vol);
                        Settings::SetFlag("audio-volume", Settings::MODIFIED, true);
                    }
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

static bool format_save_dir(char* path) {
#ifdef _WIN32
    char* app_data = getenv("APPDATA");
    const char* default_dir = "TramwaySDKShared";
    
    if (!app_data) {
        Log(Severity::ERROR, System::INVALID, "%APPDATA% env var not set!");
        return false;
    }
#else
    char* app_data = getenv("HOME");
    const char* default_dir = ".TramwaySDKShared";
    
    if (!app_data) {
        Log(Severity::ERROR, System::INVALID, "HOME env var not set!");
        return false;
    }
#endif
    snprintf(path, PATH_LIMIT, "%s/%s/saves", app_data, Core::GetApplicationShortName() ? Core::GetApplicationShortName() : default_dir);
    return true;
}

SaveMenu::SaveMenu(bool saving, bool loading, bool exiting) {
    this->saving = saving;
    this->loading = loading;
    this->exiting = exiting;
    
    if (!saving && !loading) return;
    
    char path[PATH_LIMIT];
    format_save_dir(path);
    
    if (!std::filesystem::exists(path)) {
        Log(Severity::WARNING, System::INVALID, "Save directory not found: {}", path);
        return;
    }

    if (!std::filesystem::is_directory(path)) {
        Log(Severity::WARNING, System::INVALID, "Save directory not directory: {}", path);
        return;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (!entry.is_directory()) {
            Log(Severity::WARNING, System::INVALID, "Stray file in saves: {}", entry.path().c_str());
        }
        saves.push_back(entry.path().filename().string());
    }
}

void SaveMenu::Display() {
    uint32_t menu_height = loading && loading_index == -1 ? 240 : 90;
    uint32_t menu_width = 320;
    uint32_t menu_offset = (UI::GetScreenHeight() / GUI::GetScaling() - menu_height) / 2;
    uint32_t menu_offset2 = (UI::GetScreenWidth() / GUI::GetScaling() - menu_width) / 2;
    
    GUI::PushFrameRelative(GUI::FRAME_TOP_INV, menu_offset);
    GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, menu_offset2);
    GUI::PushFrameRelative(GUI::FRAME_TOP, menu_height);
    GUI::PushFrameRelative(GUI::FRAME_LEFT, menu_width);
    GUI::FillFrame(GUI::WIDGET_WINDOW);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
        if (loading && loading_index == -1) {
            GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 8);
                if (saves.size()) {
                    GUI::Text("Select a save:", GUI::TEXT_CENTER);
                } else {
                    GUI::Text("No saves found.", GUI::TEXT_CENTER);
                }
            GUI::PopFrame();
            
            GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 32);
                int lines_lined = 0;
                for (int i = (int)saves.size() - 1; i >= 0 && i >= (int)saves.size() - 21; i--) {
                    if (GUI::Button(saves[i].c_str(), true, 100)) {
                        loading_index = i;
                    }
                    if (lines_lined++ == 2) lines_lined = 0, GUI::NewLine();
                }
            GUI::PopFrame();
            
            GUI::PushFrameRelative(GUI::FRAME_BOTTOM, 28);
            GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, (menu_width - 70) / 2);
                if (GUI::Button("Cancel", true, 70)) {
                    Menu::Pop();
                }
            GUI::PopFrame();
            GUI::PopFrame();
        } else if (loading) {
            GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 8);
                GUI::Text("All unsaved progress will be lost.", GUI::TEXT_CENTER); GUI::NewLine();
                GUI::Text("Load selected save?", GUI::TEXT_CENTER);
            GUI::PopFrame();
            
            GUI::PushFrameRelative(GUI::FRAME_BOTTOM, 28);
            GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, (menu_width - 152) / 2);
                if (GUI::Button("Yes", true, 70)) {
                    char path[PATH_LIMIT];
                    format_save_dir(path);
                    strncat(path, "/", PATH_LIMIT - strlen(path));
                    strncat(path, saves[loading_index].c_str(), PATH_LIMIT - strlen(path));
                    
                    auto search_list = FileReader::GetSearchList();
                    auto backup = search_list;
                    
                    search_list.push_back({"file", path});
                    
                    Serialization::Reload();
                    FileReader::SetSearchList(search_list);
                    Serialization::LoadFlat();
                    FileReader::SetSearchList(backup);
                    
                    Menu::Clear();
                }
                GUI::Text(" ");
                if (GUI::Button("No", true, 70)) {
                    Menu::Pop();
                }
            GUI::PopFrame();
            GUI::PopFrame();
        } else if (saving) {
            GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 8);
                GUI::Text("Create a new save?", GUI::TEXT_CENTER);
            GUI::PopFrame();

            GUI::PushFrameRelative(GUI::FRAME_BOTTOM, 28);
            GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, (menu_width - 152) / 2);
                if (GUI::Button("Yes", true, 70)) {
                    char path[PATH_LIMIT];
                    format_save_dir(path);
                    char savename[20];
                    snprintf(savename, 20, "/%03llu", saves.size());
                    strncat(path, savename, PATH_LIMIT - strlen(path));
                    
                    FileWriter::SetProtocolAlias("save", "file", path);
                    Serialization::Save();
                    FileWriter::SetProtocolAlias("save", nullptr, nullptr);
                    
                    Menu::Clear();
                }
                GUI::Text(" ");
                if (GUI::Button("No", true, 70)) {
                    Menu::Pop();
                }
            GUI::PopFrame();
            GUI::PopFrame();
        } else if (exiting) {
            GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 8);
                GUI::Text("All unsaved progress will be lost.", GUI::TEXT_CENTER); GUI::NewLine();
                GUI::Text("Save before exiting?", GUI::TEXT_CENTER);
            GUI::PopFrame();
            
            GUI::PushFrameRelative(GUI::FRAME_BOTTOM, 28);
            GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, (menu_width - 230) / 2);
                if (GUI::Button("Yes", true, 70)) {
                    UI::SetShouldExit(true);
                }
                GUI::Text(" ");
                if (GUI::Button("No", true, 70)) {
                    UI::SetShouldExit(true);
                }
                GUI::Text(" ");
                if (GUI::Button("Cancel", true, 70)) {
                    Menu::Pop();
                }
            GUI::PopFrame();
            GUI::PopFrame();
        } else {
            GUI::PushFrameRelative(GUI::FRAME_TOP_INV, 8);
                GUI::Text("All unsaved progress will be lost.", GUI::TEXT_CENTER); GUI::NewLine();
                GUI::Text("Reset everything?", GUI::TEXT_CENTER); GUI::NewLine();
            GUI::PopFrame();
            
            GUI::PushFrameRelative(GUI::FRAME_BOTTOM, 28);
            GUI::PushFrameRelative(GUI::FRAME_LEFT_INV, (menu_width - 152) / 2);
                if (GUI::Button("Yes", true, 70)) {
                    Serialization::Reload();
                    
                    Menu::Clear();
                }
                GUI::Text(" ");
                if (GUI::Button("No", true, 70)) {
                    Menu::Pop();
                }
            GUI::PopFrame();
            GUI::PopFrame();
        }
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}

}