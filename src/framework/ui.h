// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_UI_H
#define TRAM_SDK_FRAMEWORK_UI_H

#include <render/render.h>

namespace tram::UI {

enum KeyboardKey : uint16_t {
    // stutter keys
    KEY_SPACE, KEY_APOSTROPHE, KEY_COMMA, KEY_MINUS, KEY_PERIOD, KEY_SLASH,
    
    // number keys
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    
    // programming keys
    KEY_SEMICOLON, KEY_EQUAL,
    
    // letter keys
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M,
    KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
    
    // keys that you press by accident
    KEY_LEFT_BRACKET, KEY_BACKSLASH, KEY_RIGHT_BRACKET, KEY_GRAVE_ACCENT,
    
    // i have no idea what these are
    KEY_WORLD_1, KEY_WORLD_2,
    
    // stutter keys
    KEY_ESCAPE, KEY_ENTER, KEY_TAB, KEY_BACKSPACE, KEY_INSERT, KEY_DELETE,
    
    // arrow keys
    KEY_RIGHT, KEY_LEFT, KEY_DOWN, KEY_UP,
    
    // keys right above arrow keys
    KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_HOME, KEY_END,
    KEY_CAPS_LOCK, KEY_SCROLL_LOCK, KEY_NUM_LOCK, KEY_PRINT_SCREEN, KEY_PAUSE,
    
    // function keys
    KEY_F1,     KEY_F2,     KEY_F3,     KEY_F4,     KEY_F5,     KEY_F6,
    KEY_F7,     KEY_F8,     KEY_F9,     KEY_F10,    KEY_F11,    KEY_F12, KEY_F13,
    KEY_F14,    KEY_F15,    KEY_F16,    KEY_F17,    KEY_F18,    KEY_F19,
    KEY_F20,    KEY_F21,    KEY_F22,    KEY_F23,    KEY_F24,    KEY_F25,
    KEY_KP_0,   KEY_KP_1,   KEY_KP_2,   KEY_KP_3,   KEY_KP_4,   KEY_KP_5,
    KEY_KP_6,   KEY_KP_7,   KEY_KP_8,   KEY_KP_9,
    
    // keyoad keys
    KEY_KP_DECIMAL, KEY_KP_DIVIDE, KEY_KP_MULTIPLY, KEY_KP_SUBTRACT, KEY_KP_ADD,
    KEY_KP_ENTER,  KEY_KP_EQUAL,
    
    // modfier keys
    KEY_LEFT_SHIFT, KEY_LEFT_CONTROL, KEY_LEFT_ALT, KEY_LEFT_SUPER,
    KEY_RIGHT_SHIFT, KEY_RIGHT_CONTROL, KEY_RIGHT_ALT, KEY_RIGHT_SUPER,
    
    // i have no idea what this key is
    KEY_MENU,
    
    // mouse keys
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
    STATE_DEFAULT,      //< Normal input, events get generated, cursor disabled.
    STATE_NO_INPUT,     //< Inputs get ignored, no cursor.
    STATE_FLYING,       //< Events are skipped, inputs directly move view.
    STATE_MENU_OPEN,    //< Input gets registered, but no events are generated, cursor enabled.
    STATE_CURSOR        //< Normal input, events get generated, but cursor enabled.
};

typedef uint16_t keyboardaction_t;

enum KeyboardAction : keyboardaction_t {
    KEY_ACTION_NONE,
    KEY_ACTION_FORWARD,
    KEY_ACTION_BACKWARD,
    KEY_ACTION_STRAFE_LEFT,
    KEY_ACTION_STRAFE_RIGHT,
    KEY_ACTION_JUMP,
    KEY_ACTION_CROUCH,
    KEY_ACTION_SPRINT,
    KEY_ACTION_ACTIVATE,
    KEY_ACTION_UP,
    KEY_ACTION_DOWN,
    KEY_ACTION_LEFT,
    KEY_ACTION_RIGHT,
    KEY_ACTION_LAST
};

enum CursorType {
    CURSOR_DEFAULT,
    CURSOR_TEXT,
    CURSOR_CLICK
};

const float CAMERA_SENSITIVITY = 6.0f;
const float CAMERA_SPEED = 6.0f;

void Init();
void Uninit();
void Update();
void EndFrame();

float GetScreenWidth();
float GetScreenHeight();

void SetWindowTitle(const char* title);
void SetWindowSize(int w, int h);

void SetCursor(CursorType cursor);
void SetTextInput(char* text, uint32_t len);

void SetWebMainLoop(void(*loop_function)(void));

void BindKeyboardKey(KeyboardKey key, keyboardaction_t action);
void BindKeyboardKey(KeyboardKey key, void (*action)());

bool PollKeyboardKey(KeyboardKey key);
float PollKeyboardAxis(KeyboardAxis key);
float PollKeyboardAxisDelta(KeyboardAxis key);

void SetInputState(InputState state);
InputState GetInputState();

// maybe instead of each platform calling this? these could be fed as pointers
// into the platform?
// TODO: fix
void KeyPress(KeyboardKey key);
void KeyRelease(KeyboardKey key);
void KeyCode(uint16_t code);
void KeyMouse(float x_value, float y_value);
void KeyScroll(float value);
void ScreenResize(int w, int h);
void ScreenClose();

keyboardaction_t RegisterKeyboardAction(const char* name);
keyboardaction_t GetKeyboardAction(name_t name);
name_t GetKeyboardActionName(keyboardaction_t type);

std::vector<std::pair<keyboardaction_t, std::vector<KeyboardKey>>> GetAllKeyboardKeyBindings();
const char* GetKeyboardKeyName(KeyboardKey key);

bool ShouldExit();

}

#endif // TRAM_SDK_FRAMEWORK_UI_H
