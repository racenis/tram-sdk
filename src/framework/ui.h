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
    enum InputState {
        STATE_DEFAULT,
        STATE_STARTUP,
        STATE_NO_INPUT,
        STATE_FLYING,
        STATE_MENU_OPEN
    };

    // TODO: rename this to 'keyboard events' ?? maybe
    // or like inputaction or something
    enum KeyboardAction : uint16_t {
        KEY_NONE =      0,
        KEY_FORWARD =   1,
        KEY_BACKWARD =  2,
        KEY_LEFT =      4,
        KEY_RIGHT =     8,
        KEY_JUMP =      16,
        KEY_CROUCH =    32,
        KEY_ACTIVATE =  64
    };

    // TODO: rename this to just keyboard ??? wtf
    struct KeyAction {
        enum Type: uint8_t {
            UNBOUND,
            KEYBOARD_KEY,
            SPECIAL_OPTION
        };

        KeyAction::Type type = UNBOUND;

        union {
            KeyboardAction action = KEY_NONE;
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

    extern std::unordered_map<int, KeyAction> KeyActionBindings;

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

    // TODO: yeet these
    void LoadFontInfo(const char* filename, size_t fontIndex);
    void LoadGlyphInfo(const char* filename, size_t fontIndex);
    void SetText(const char* text, uint32_t x, uint32_t y, float scale, float width, bool justify, bool stretch, uint32_t font, const vec3& color);
    void SetDebugText(const char* text, const vec3& location, const vec3& color);
}




#endif // UI_H
