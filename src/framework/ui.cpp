// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <platform/platform.h>
#include <platform/image.h>
#include <platform/api.h>

#include <framework/core.h>
#include <framework/event.h>
#include <framework/ui.h>
#include <framework/system.h>

#include <render/render.h>
#include <render/api.h>

#include <fstream>
#include <cstring>
#include <algorithm>
#include <ctime>

#include <templates/hashmap.h>


#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
#endif


using namespace tram::Render;

namespace tram::UI {

static InputState input_state = STATE_DEFAULT;

static float screen_width = 800.0f;
static float screen_height = 600.0f;

static float camera_pitch = 0.0f;
static float camera_yaw = -90.0f;

static bool exit = false;

static bool keyboard_keys_values[KEY_LASTKEY] = {false};
static float keyboard_axis_values[KEY_LASTAXIS] = {0.0f};
static float keyboard_axis_deltas[KEY_LASTAXIS] = {0.0f};
static float keyboard_axis_sensitivity[KEY_LASTAXIS] = {1.0f, 1.0f, 1.0f};

//static double cursorx_last = 0.0f, cursory_last = 0.0f;

// static char* input_text = nullptr;
// static uint32_t input_text_len = 0;

struct KeyBinding {
    keyboardaction_t action = KEY_ACTION_NONE;
    void (*special_option)(void) = nullptr;
    void (*special_option2)(KeyboardKey) = nullptr;
};

static std::unordered_map<KeyboardKey, KeyBinding> key_action_bindings = {
    {KEY_W, KeyBinding {.action = KEY_ACTION_FORWARD}},
    {KEY_S, KeyBinding {.action = KEY_ACTION_BACKWARD}},
    {KEY_A, KeyBinding {.action = KEY_ACTION_STRAFE_LEFT}},
    {KEY_D, KeyBinding {.action = KEY_ACTION_STRAFE_RIGHT}},
    {KEY_SPACE, KeyBinding {.action = KEY_ACTION_JUMP}},
    {KEY_LEFT_CONTROL, KeyBinding {.action = KEY_ACTION_CROUCH}},
    {KEY_LEFT_SHIFT, KeyBinding {.action = KEY_ACTION_SPRINT}},
    {KEY_LEFT_ALT, KeyBinding {.action = KEY_ACTION_FLY}},
    {KEY_E, KeyBinding {.action = KEY_ACTION_ACTIVATE}},
    
    {KEY_UP, KeyBinding {.action = KEY_ACTION_UP}},
    {KEY_DOWN, KeyBinding {.action = KEY_ACTION_DOWN}},
    {KEY_LEFT, KeyBinding {.action = KEY_ACTION_LEFT}},
    {KEY_RIGHT, KeyBinding {.action = KEY_ACTION_RIGHT}},

    {KEY_F1, KeyBinding {.special_option = [](){ exit = true; /*glfwSetWindowShouldClose(WINDOW, EXIT);*/ }}},
    //{KEY_F5, KeyBinding {.special_option = [](){ THIRD_PERSON = !THIRD_PERSON; }}},
    //{KEY_F6, KeyBinding {.special_option = [](){ THIRD_PERSON = !THIRD_PERSON; }}},
    {KEY_F9, KeyBinding {.special_option = [](){ input_state = (input_state == STATE_FLYING) ? STATE_DEFAULT : STATE_FLYING; }}},
    //{KEY_BACKSPACE, KeyBinding {.special_option = [](){ CharacterBackspaceCallback(); }}}
    {KEY_F12, KeyBinding {.special_option = [](){
        char* buffer = (char*)malloc(screen_width * screen_height * 3);

        std::time_t epoch_time;
        std::time(&epoch_time);
        std::tm date_time = *std::localtime(&epoch_time);
        
        char time_str[100];
        std::strftime(time_str, 100, "--%Y-%b-%d--%H-%M-%S", &date_time);
        
        char file_name[200];
        strcpy(file_name, "screenshot");
        strcat(file_name, time_str);
        strcat(file_name, ".png");
        
        Render::API::GetScreen(buffer, screen_width, screen_height);
        Platform::SaveImageToDisk(file_name, screen_width, screen_height, buffer);
        free(buffer);
    }}},
};

void BindKeyboardKey(KeyboardKey key, keyboardaction_t action) {
    // this will check if any other key has already been bound to this action,
    // and will unbind it if it has.
    /*for (auto& binding : key_action_bindings) {
        if (binding.second.action == action) {
            binding.second.action = KEY_ACTION_NONE;
        }
    }*/
    key_action_bindings[key] = {.action = action};
}

void BindKeyboardKey(KeyboardKey key, void (*action)()) {
    key_action_bindings[key] = {.special_option = action};
}

void BindKeyboardKey(KeyboardKey key, void (*action)(KeyboardKey)) {
    key_action_bindings[key] = {.special_option2 = action};
}

void Init() {
    assert(System::IsInitialized(System::SYSTEM_CORE));
    
    Platform::Window::Init();
    
    System::SetInitialized(System::SYSTEM_UI, true);
}

void Uninit() {
    Platform::Window::Uninit();
}

void Update() {
    if (input_state == STATE_FLYING) {
        vec3 camera_position = GetViewPosition();
        quat camera_rotation = GetViewRotation();
        
        if (keyboard_keys_values[KEY_W])
            camera_position += camera_rotation * DIRECTION_FORWARD * CAMERA_SPEED * GetDeltaTime();
        if (keyboard_keys_values[KEY_S])
            camera_position -= camera_rotation * DIRECTION_FORWARD * CAMERA_SPEED * GetDeltaTime();
        if (keyboard_keys_values[KEY_A])
            camera_position -= camera_rotation * DIRECTION_SIDE * CAMERA_SPEED * GetDeltaTime();
        if (keyboard_keys_values[KEY_D])
            camera_position += camera_rotation * DIRECTION_SIDE * CAMERA_SPEED * GetDeltaTime();
            
        camera_yaw += PollKeyboardAxisDelta(KEY_MOUSE_X) * CAMERA_SENSITIVITY * GetDeltaTime();
        camera_pitch += PollKeyboardAxisDelta(KEY_MOUSE_Y) * CAMERA_SENSITIVITY * GetDeltaTime();
        camera_pitch = camera_pitch > 90.0f ? 90.0f : camera_pitch < -90.0f ? -90.0f : camera_pitch;
        
        camera_rotation = quat(vec3(-glm::radians(camera_pitch), -glm::radians(camera_yaw), 0.0f));
        
        SetViewPosition(camera_position, 0);
        SetViewRotation(camera_rotation, 0);
        
        SetViewPosition(camera_position, 1);
        SetViewRotation(camera_rotation, 1);
    }
    
    // generate keypress events. these need to be generated every tick that the
    // keyboard key is being pressed down.
    if (input_state == STATE_DEFAULT) for (auto& b : key_action_bindings) {
        if (PollKeyboardKey(b.first) && b.second.action) {
            Event::Post({Event::KEYPRESS, b.second.action, 0, nullptr});
        }
    }
    
    keyboard_axis_values[KEY_MOUSE_SCROLL] = 0.0f;
    
    // idk why, but these two lines make the mouse break on emscripten
    // TODO: figure out why and fix
#ifndef __EMSCRIPTEN__
    keyboard_axis_deltas[KEY_MOUSE_X] = 0.0f;
    keyboard_axis_deltas[KEY_MOUSE_Y] = 0.0f;
#endif
    
    Platform::Input::Update();
}




void EndFrame() {
    Platform::Window::Update();
}

void SetWebMainLoop(void(*loop_function)(void)) {
#ifdef TRAM_SDK_PLATFORM_WEB
    emscripten_set_main_loop(loop_function, 0 , 0);
#else
    std::cout << "SetWebMainLoop() only works with web platform, aborting..." << std::endl;
    abort();
#endif
}

float GetScreenWidth() {
    return screen_width;
}
    
float GetScreenHeight() {
    return screen_height;
}

void SetWindowTitle(const char* title) {
    Platform::Window::SetTitle(title);
    
}

void SetWindowSize(int w, int h) {
    Platform::Window::SetSize(w, h);
}

void SetCursor(CursorType cursor) {
    switch (cursor) {
        //case CURSOR_NONE:
            //Platform::Window::DisableCursor();
            //break;
        case CURSOR_DEFAULT:
            //Platform::Window::EnableCursor();
            Platform::Window::SetCursor(Platform::Window::CURSOR_DEFAULT);
            break;
        case CURSOR_TEXT:
            //Platform::Window::EnableCursor();
            Platform::Window::SetCursor(Platform::Window::CURSOR_TEXT);
            break;
        case CURSOR_CLICK:
            //Platform::Window::EnableCursor();
            Platform::Window::SetCursor(Platform::Window::CURSOR_CLICK);
            break;
    };
}

void SetInputState (InputState state) {
    input_state = state;
    
    switch (state) {
        case STATE_DEFAULT:
        case STATE_NO_INPUT:
        case STATE_FLYING:
            //keyboard_axis_values[KEY_MOUSE_X] = cursorx_last;
            //keyboard_axis_values[KEY_MOUSE_Y] = cursory_last;
            
            Platform::Window::DisableCursor();
        break;
        case STATE_MENU_OPEN:
        case STATE_CURSOR:
            //cursorx_last = keyboard_axis_values[KEY_MOUSE_X];
            //cursory_last = keyboard_axis_values[KEY_MOUSE_Y];
            
            Platform::Window::EnableCursor();
    }
}


static bool GenerateEvent() {
    switch (input_state) {
        case STATE_DEFAULT:
        case STATE_CURSOR:
            return true;
        case STATE_NO_INPUT:
        case STATE_FLYING:
        case STATE_MENU_OPEN:
            return false;  
        default:
            return false;
    }
}

InputState GetInputState() {
    return input_state;
}

/// Checks the state of a key for the current frame.
/// @return True, if key is pressed, false otherwise.
bool PollKeyboardKey(KeyboardKey key) {
    return keyboard_keys_values[key];
}

/// Checks the state of an axis for the current frame.
/// @return Value of the axis.
float PollKeyboardAxis(KeyboardAxis key) {
    return keyboard_axis_values[key];
}

/// Checks the change of an axis for since the last frame.
/// @return Value of the axis delta.
float PollKeyboardAxisDelta(KeyboardAxis key) {
    return keyboard_axis_sensitivity[key] * keyboard_axis_deltas[key];
}

float GetAxisSensitivity(KeyboardAxis key) {
    return keyboard_axis_sensitivity[key];
}

void SetAxisSensitivity(KeyboardAxis key, float value) {
    keyboard_axis_sensitivity[key] = value;
}

void KeyPress(KeyboardKey key) {
    const auto& binding = key_action_bindings[key];

    if (binding.action && GenerateEvent()) {            
        Event::Post({Event::KEYDOWN, binding.action, 0, nullptr});
    } else if (binding.special_option) {
        binding.special_option();
    } else if (binding.special_option2) {
        binding.special_option2(key);
    }
    
    keyboard_keys_values[key] = true;
}

void KeyRelease(KeyboardKey key) {
    const auto& binding = key_action_bindings[key];
    
    if (binding.action && input_state == STATE_DEFAULT) {            
        Event::Post({Event::KEYUP, binding.action, 0, nullptr});
    }
    
    keyboard_keys_values[key] = false;
}

void KeyCode(uint16_t code) {
    Event::Post({Event::KEYCHAR, code, 0, nullptr});
}

void KeyMouse(float xpos, float ypos) {
    static float last_xpos = xpos;
    static float last_ypos = ypos;
    keyboard_axis_deltas[KEY_MOUSE_X] = xpos - last_xpos;
    keyboard_axis_deltas[KEY_MOUSE_Y] = ypos - last_ypos;
    keyboard_axis_values[KEY_MOUSE_X] = xpos;
    keyboard_axis_values[KEY_MOUSE_Y] = ypos;
    last_xpos = xpos;
    last_ypos = ypos;

    if (input_state == STATE_DEFAULT) {
        Event::Post({Event::CURSORPOS, 0xFFFF, 0, nullptr});
    }
}

void KeyScroll(float value) {
    keyboard_axis_values[KEY_MOUSE_SCROLL] = value;
}

void ScreenResize(int width, int height) {
    screen_width = width;
    screen_height = height;
    Render::SetScreenSize(width, height);
}

void ScreenClose() {
    exit = true;
}

bool ShouldExit() {
    return exit;
}

const size_t MAX_KEYBOARDACTION_TYPES = 100;
static Hashmap<keyboardaction_t> name_t_to_keyboardaction_t("name_t_to_keyboardaction_t", (MAX_KEYBOARDACTION_TYPES*2)+11);
static const char* keyboardaction_names[MAX_KEYBOARDACTION_TYPES] = {
    "none",
    "forward",
    "backward",
    "strafe-left",
    "strafe-right",
    "jump",
    "crouch",
    "sprint",
    "fly",
    "activate",
    "up",
    "down",
    "left",
    "right"
};

static keyboardaction_t last_type = KeyboardAction::KEY_ACTION_LAST;

keyboardaction_t RegisterKeyboardAction(const char* name) {
    keyboardaction_names[last_type] = name;
    return last_type++;
}

keyboardaction_t GetKeyboardAction(name_t name) {
    keyboardaction_t type = name_t_to_keyboardaction_t.Find(name);
    
    if (!type) {
        for (keyboardaction_t i = 0; i < last_type; i++) {
            if (keyboardaction_names[i] == name) {
                name_t_to_keyboardaction_t.Insert(name, i);
                return i;
            }
        }
    }
    
    return type;
}

name_t GetKeyboardActionName(keyboardaction_t type) {
    return keyboardaction_names[type];
}

std::vector<std::pair<keyboardaction_t, std::vector<KeyboardKey>>> GetAllKeyboardKeyBindings() {
    std::vector<std::pair<keyboardaction_t, std::vector<KeyboardKey>>> binding_list;
    for (keyboardaction_t i = 1; i < last_type; i++) {
        binding_list.push_back({i, {}});
    }
    
    /*std::unordered_map<keyboardaction_t, std::vector<KeyboardKey>> binding_map;
    for (auto& binding : key_action_bindings) {
        if (binding.second.action) {
            binding_map[binding.second.action].push_back(binding.first);
        }
    }*/
    
    
    for (auto& binding : key_action_bindings) {
        if (binding.second.action) {
            for (auto& list_binding : binding_list) {
                if (list_binding.first == binding.second.action) {
                    list_binding.second.push_back(binding.first);
                }
            }
        }
        
        //binding_list.push_back({binding.first, binding.second});
        //binding_list.push_back({binding.first, binding.second});
    }

    //std::sort(binding_list.begin(), binding_list.end());
    return binding_list;
}

const char* GetKeyboardKeyName(KeyboardKey key) {
    switch (key) {
    
        case KEY_SPACE:             return "space";
        case KEY_APOSTROPHE:        return "'";
        case KEY_COMMA:             return ",";
        case KEY_MINUS:             return "-";
        case KEY_PERIOD:            return ".";
        case KEY_SLASH:             return "/";
        case KEY_0:                 return "0";
        case KEY_1:                 return "1";
        case KEY_2:                 return "2";
        case KEY_3:                 return "3";
        case KEY_4:                 return "4";
        case KEY_5:                 return "5";
        case KEY_6:                 return "6";
        case KEY_7:                 return "7";
        case KEY_8:                 return "8";
        case KEY_9:                 return "9";
        case KEY_SEMICOLON:         return ";";
        case KEY_EQUAL:             return "=";
        case KEY_A:                 return "a";
        case KEY_B:                 return "b";
        case KEY_C:                 return "c";
        case KEY_D:                 return "d";
        case KEY_E:                 return "e";
        case KEY_F:                 return "f";
        case KEY_G:                 return "g";
        case KEY_H:                 return "h";
        case KEY_I:                 return "i";
        case KEY_J:                 return "j";
        case KEY_K:                 return "k";
        case KEY_L:                 return "l";
        case KEY_M:                 return "m";
        case KEY_N:                 return "n";
        case KEY_O:                 return "o";
        case KEY_P:                 return "p";
        case KEY_Q:                 return "q";
        case KEY_R:                 return "r";
        case KEY_S:                 return "s";
        case KEY_T:                 return "t";
        case KEY_U:                 return "u";
        case KEY_V:                 return "v";
        case KEY_W:                 return "w";
        case KEY_X:                 return "x";
        case KEY_Y:                 return "y";
        case KEY_Z:                 return "z";
        case KEY_LEFT_BRACKET:      return "(";
        case KEY_BACKSLASH:         return "\\";
        case KEY_RIGHT_BRACKET:     return ")";
        case KEY_GRAVE_ACCENT:      return "`";
        case KEY_WORLD_1:           return "world1";
        case KEY_WORLD_2:           return "world2";
        case KEY_ESCAPE:            return "escape";
        case KEY_ENTER:             return "enter";
        case KEY_TAB:               return "tab";
        case KEY_BACKSPACE:         return "backspace";
        case KEY_INSERT:            return "insert";
        case KEY_DELETE:            return "delete";
        case KEY_RIGHT:             return "right";
        case KEY_LEFT:              return "left";
        case KEY_DOWN:              return "down";
        case KEY_UP:                return "up";
        case KEY_PAGE_UP:           return "pageup";
        case KEY_PAGE_DOWN:         return "pagedown";
        case KEY_HOME:              return "home";
        case KEY_END:               return "end";
        case KEY_CAPS_LOCK:         return "capslock";
        case KEY_SCROLL_LOCK:       return "scrolllock";
        case KEY_NUM_LOCK:          return "numlock";
        case KEY_PRINT_SCREEN:      return "printscreen";
        case KEY_PAUSE:             return "pause";
        case KEY_F1:                return "f1";
        case KEY_F2:                return "f2";
        case KEY_F3:                return "f3";
        case KEY_F4:                return "f4";
        case KEY_F5:                return "f5";
        case KEY_F6:                return "f6";
        case KEY_F7:                return "f7";
        case KEY_F8:                return "f8";
        case KEY_F9:                return "f9";
        case KEY_F10:               return "f10";
        case KEY_F11:               return "f11";
        case KEY_F12:               return "f12";
        case KEY_F13:               return "f13";
        case KEY_F14:               return "f14";    
        case KEY_F15:               return "f15";
        case KEY_F16:               return "f16";
        case KEY_F17:               return "f17";
        case KEY_F18:               return "f18";
        case KEY_F19:               return "f19";
        case KEY_F20:               return "f20";
        case KEY_F21:               return "f21";
        case KEY_F22:               return "f22";
        case KEY_F23:               return "f23";
        case KEY_F24:               return "f24";
        case KEY_F25:               return "f25";
        case KEY_KP_0:              return "keypad0";
        case KEY_KP_1:              return "keypad1";
        case KEY_KP_2:              return "keypad2";
        case KEY_KP_3:              return "keypad3";
        case KEY_KP_4:              return "keypad4";
        case KEY_KP_5:              return "keypad5";
        case KEY_KP_6:              return "keypad6";
        case KEY_KP_7:              return "keypad7";
        case KEY_KP_8:              return "keypad8";
        case KEY_KP_9:              return "keypad9";
        case KEY_KP_DECIMAL:        return "keypad.";
        case KEY_KP_DIVIDE:         return "keypad/";
        case KEY_KP_MULTIPLY:       return "keypad*";
        case KEY_KP_SUBTRACT:       return "keypad-";
        case KEY_KP_ADD:            return "keypad+";
        case KEY_KP_ENTER:          return "keypadenter";
        case KEY_KP_EQUAL:          return "keypad=";
        case KEY_LEFT_SHIFT:        return "leftshift";
        case KEY_LEFT_CONTROL:      return "leftcontrol";
        case KEY_LEFT_ALT:          return "leftalt";
        case KEY_LEFT_SUPER:        return "leftsuper";
        case KEY_RIGHT_SHIFT:       return "rightshift";
        case KEY_RIGHT_CONTROL:     return "rightcontrol";
        case KEY_RIGHT_ALT:         return "rightalt";
        case KEY_RIGHT_SUPER:       return "rightsuper";
        case KEY_MENU:              return "menu";
        case KEY_RIGHTMOUSE:        return "rightmouse";
        case KEY_LEFTMOUSE:         return "leftmouse";
        case KEY_MIDDLEMOUSE:       return "middlemouse";
        case KEY_LASTKEY:           return "lastkey";
        default:                    return "anykey";
    }
}

}
