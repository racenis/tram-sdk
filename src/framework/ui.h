// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// UI.H -- User interface
// Keyboard/mouse inputs, windowing

#ifndef UI_H
#define UI_H

#include <render/render.h>

class GLFWwindow;
class GLFWcursor;

namespace Core::UI {
    enum KeyboardKey : uint16_t {
        KEY_SPACE,
        KEY_APOSTROPHE,
        KEY_COMMA,
        KEY_MINUS,
        KEY_PERIOD,
        KEY_SLASH,
        KEY_0,
        KEY_1,
        KEY_2,
        KEY_3,
        KEY_4,
        KEY_5,
        KEY_6,
        KEY_7,
        KEY_8,
        KEY_9,
        KEY_SEMICOLON,
        KEY_EQUAL,
        KEY_A,
        KEY_B,
        KEY_C,
        KEY_D,
        KEY_E,
        KEY_F,
        KEY_G,
        KEY_H,
        KEY_I,
        KEY_J,
        KEY_K,
        KEY_L,
        KEY_M,
        KEY_N,
        KEY_O,
        KEY_P,
        KEY_Q,
        KEY_R,
        KEY_S,
        KEY_T,
        KEY_U,
        KEY_V,
        KEY_W,
        KEY_X,
        KEY_Y,
        KEY_Z,
        KEY_LEFT_BRACKET,
        KEY_BACKSLASH,
        KEY_RIGHT_BRACKET,
        KEY_GRAVE_ACCENT,
        KEY_WORLD_1,
        KEY_WORLD_2,
        KEY_ESCAPE,
        KEY_ENTER,
        KEY_TAB,
        KEY_BACKSPACE,
        KEY_INSERT,
        KEY_DELETE,
        KEY_RIGHT,
        KEY_LEFT,
        KEY_DOWN,
        KEY_UP,
        KEY_PAGE_UP,
        KEY_PAGE_DOWN,
        KEY_HOME,
        KEY_END,
        KEY_CAPS_LOCK,
        KEY_SCROLL_LOCK,
        KEY_NUM_LOCK,
        KEY_PRINT_SCREEN,
        KEY_PAUSE,
        KEY_F1,
        KEY_F2,
        KEY_F3,
        KEY_F4,
        KEY_F5,
        KEY_F6,
        KEY_F7,
        KEY_F8,
        KEY_F9,
        KEY_F10,
        KEY_F11,
        KEY_F12,
        KEY_F13,
        KEY_F14,
        KEY_F15,
        KEY_F16,
        KEY_F17,
        KEY_F18,
        KEY_F19,
        KEY_F20,
        KEY_F21,
        KEY_F22,
        KEY_F23,
        KEY_F24,
        KEY_F25,
        KEY_KP_0,
        KEY_KP_1,
        KEY_KP_2,
        KEY_KP_3,
        KEY_KP_4,
        KEY_KP_5,
        KEY_KP_6,
        KEY_KP_7,
        KEY_KP_8,
        KEY_KP_9,
        KEY_KP_DECIMAL,
        KEY_KP_DIVIDE,
        KEY_KP_MULTIPLY,
        KEY_KP_SUBTRACT,
        KEY_KP_ADD,
        KEY_KP_ENTER,
        KEY_KP_EQUAL,
        KEY_LEFT_SHIFT,
        KEY_LEFT_CONTROL,
        KEY_LEFT_ALT,
        KEY_LEFT_SUPER,
        KEY_RIGHT_SHIFT,
        KEY_RIGHT_CONTROL,
        KEY_RIGHT_ALT,
        KEY_RIGHT_SUPER,
        KEY_MENU
    };
    
    enum InputState {
        STATE_DEFAULT,
        STATE_STARTUP,
        STATE_NO_INPUT,
        STATE_FLYING,
        STATE_MENU_OPEN
    };

    // TODO: change from bitmask to normal enum
    enum KeyboardAction : uint16_t {
        KEY_ACTION_NONE =      0,
        KEY_ACTION_FORWARD =   1,
        KEY_ACTION_BACKWARD =  2,
        KEY_ACTION_LEFT =      4,
        KEY_ACTION_RIGHT =     8,
        KEY_ACTION_JUMP =      16,
        KEY_ACTION_CROUCH =    32,
        KEY_ACTION_ACTIVATE =  64
    };

    struct KeyBinding {
        enum Type: uint8_t {
            UNBOUND,
            KEYBOARD_ACTION,
            SPECIAL_OPTION
        };

        KeyBinding::Type type = UNBOUND;

        union {
            KeyboardAction action = KEY_ACTION_NONE;
            void (*special_option)(void);
        };
    };

    enum CursorType {
        CURSOR_NONE,
        CURSOR_DEFAULT,
        CURSOR_TEXT,
        CURSOR_CLICK
    };
    
    struct FontCharInfo {
        float left, bottom, right, top, drop;
    };
    
    struct GlyphCharInfo {
        float x, y, w, h, drop;
    };

    extern InputState INPUT_STATE;

    extern GLFWwindow* WINDOW;
    extern name_t WINDOW_TITLE;

    extern FontCharInfo fontinfo[4][256];
    extern GlyphCharInfo glyphinfo[4][256];
    
    extern bool escape_menu_open;
    extern bool debug_menu_open;

    extern bool ismouse_left;
    extern bool isnotmouse_left;
    extern bool wasmouse_left;
    extern float mouse_scroll;

    extern float cur_x, cur_y;
    
    const float CAMERA_SENSITIVITY = 0.1f;
    const float CAMERA_SPEED = 0.1f;

    void Init();
    void Uninit();
    void Update();
    void EndFrame();
    void SetCursor(CursorType cursor);
    void SetTextInput(char* text, uint32_t len);
    double GetTime();
    void BindKeyboardKey (KeyboardKey key, KeyBinding binding);

    // TODO: yeet these
    void LoadFontInfo(const char* filename, size_t fontIndex);
    void LoadGlyphInfo(const char* filename, size_t fontIndex);
    void SetText(const char* text, uint32_t x, uint32_t y, float scale, float width, bool justify, bool stretch, uint32_t font, const vec3& color);
    void SetDebugText(const char* text, const vec3& location, const vec3& color);
}




#endif // UI_H
