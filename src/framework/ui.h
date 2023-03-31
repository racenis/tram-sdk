// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_UI_H
#define TRAM_SDK_FRAMEWORK_UI_H

#include <render/render.h>

class GLFWwindow;
class GLFWcursor;

namespace tram::UI {
    // TODO: make this list a bit less unreadable
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
        KEY_MENU,
        KEY_RIGHTMOUSE,
        KEY_LEFTMOUSE,
        KEY_MIDDLEMOUSE,
        KEY_LASTKEY
    };
    
    enum KeyboardAxis : uint16_t {
        KEY_MOUSE_X,
        KEY_MOUSE_Y,
        KEY_MOUSE_SCROLL,
        KEY_LASTAXIS
    };
    
    enum InputState {
        STATE_DEFAULT,
        STATE_STARTUP,
        STATE_NO_INPUT,
        STATE_FLYING,
        STATE_MENU_OPEN
    };

    enum KeyboardAction : uint16_t {
        KEY_ACTION_NONE,
        KEY_ACTION_FORWARD,
        KEY_ACTION_BACKWARD,
        KEY_ACTION_LEFT,
        KEY_ACTION_RIGHT,
        KEY_ACTION_JUMP,
        KEY_ACTION_CROUCH,
        KEY_ACTION_ACTIVATE
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
    
    extern InputState INPUT_STATE;

    extern GLFWwindow* WINDOW;
    extern name_t WINDOW_TITLE;
    
    extern float CAMERA_PITCH;
    extern float CAMERA_YAW;
    
    const float CAMERA_SENSITIVITY = 0.1f;
    const float CAMERA_SPEED = 0.1f;

    void Init();
    void Uninit();
    void Update();
    void EndFrame();
    
    float GetScreenWidth();
    float GetScreenHeight();
    
    void SetCursor(CursorType cursor);
    void SetTextInput(char* text, uint32_t len);
    
    void SetWebMainLoop(void(*loop_function)(void));
    
    void BindKeyboardKey (KeyboardKey key, KeyBinding binding);

    bool PollKeyboardKey (KeyboardKey key);
    float PollKeyboardAxis (KeyboardAxis key);
}




#endif // TRAM_SDK_FRAMEWORK_UI_H
