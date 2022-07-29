// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// UI.H -- User interface
// Keyboard/mouse inputs, windowing

#ifndef UI_H
#define UI_H

#include <render.h>

class GLFWwindow;
class GLFWcursor;

namespace Core::UI {
    class GUIScreen;
    class GUIElement;


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

    extern InputState INPUT_STATE;

    extern GLFWwindow* WINDOW;
    extern name_t WINDOW_TITLE;

    extern std::unordered_map<int, KeyAction> KeyActionBindings;

    extern FontCharInfo fontinfo[2][256];

    extern bool ismouse_left;

    extern float cur_x, cur_y;

    void Init();
    void Uninit();
    void Update();
    void EndFrame();
    void SetCursor(CursorType cursor);
    void SetGUIScreen(GUIScreen* screen);
    void SetHUDMenu(GUIScreen* screen);
    void SetMainMenu(GUIScreen* screen);

    void LoadFontInfo(const char* filename, size_t fontIndex);



    // TODO: factor the rest of this out to gui.h
    class GUIScreen;

    class GUIElement {
    public:
        GUIElement(float pos_x, float pos_y, float pos_w, float pos_h) : pos_x(pos_x), pos_y(pos_y), pos_w(pos_w), pos_h(pos_h) {}
        GUIScreen* parent = nullptr;
        virtual void Draw() = 0;
    protected:
        float pos_x, pos_y, pos_w, pos_h;
    };

    class GUIScreen {
    public:
        GUIScreen () = default;

        virtual void Update () {
            SetCursor(CURSOR_DEFAULT);

            for (auto e: elements) {
                e->Draw();
            }
        }

        bool is_menu = true;

        void AddElement (GUIElement* element) {
            element->parent = this;
            elements.push_back(element);
        }

    protected:
        std::vector<GUIElement*> elements;
    };

    class GUIText : public GUIElement {
    public:
        GUIText(float pos_x, float pos_y, float pos_w, float pos_h, const char* const text) : GUIElement(pos_x, pos_y, pos_w, pos_h), text(text) {}
        void Draw(){
            SetText(text, pos_x, pos_y, 1.0f, pos_w, false, false, 0, Render::COLOR_WHITE);
        }
    protected:
        const char* const text;
    };

    class GUIButton : public GUIElement {
    public:
        GUIButton(float pos_x, float pos_y, float pos_w, float pos_h, const char* const text) : GUIElement(pos_x, pos_y, pos_w, pos_h), text(text) {}
        void Draw(){
            const glm::vec3& scolor = (cur_x > pos_x && cur_x < pos_x + pos_w && cur_y > pos_y && cur_y < pos_y + pos_h) ? Render::COLOR_PINK : Render::COLOR_WHITE;

            SetText(text, pos_x+16.0f, pos_y+7.0f, 1.0f, pos_w-32.0f, true, true, 0, scolor);

            const float thickness = 1.0f;
            Render::SetGlyph(pos_x-4,  pos_y-4,     0,  0, 20, 20, 1, 1, thickness, 4, scolor);    // top-left corner
            Render::SetGlyph(pos_x-4,  pos_y + pos_h - 16,  0, 28, 20, 20, 1, 1, thickness, 4, scolor);    // bottom-left corner
            Render::SetGlyph(pos_x + pos_w-16, pos_y-4,    28,  0, 20, 20, 1, 1, thickness, 4, scolor);    // top-right corner
            Render::SetGlyph(pos_x + pos_w-16, pos_y + pos_h-16,   28, 28, 20, 20, 1, 1, thickness, 4, scolor);    // bottom-right corner

            Render::SetGlyph(pos_x+16, pos_y-4, 20, 0, 8, 10, (pos_w-(16*2))/8, 1, thickness, 4, scolor);      // top
            Render::SetGlyph(pos_x+16, pos_y + pos_h-6, 20, 0, 8, 10, (pos_w-(16*2))/8, 1, thickness, 4, scolor);

            Render::SetGlyph(pos_x-4, pos_y+16, 0, 20, 10, 8, 1, (pos_h-(16*2))/8, thickness, 4, scolor);     // left
            Render::SetGlyph(pos_x + pos_w-6, pos_y+16, 0, 20, 10, 8, 1, (pos_h-(16*2))/8, thickness, 4, scolor);
        }
        bool IsClicked(){
            if (cur_x > pos_x && cur_x < pos_x + pos_w && cur_y > pos_y && cur_y < pos_y + pos_h){
                SetCursor(CURSOR_CLICK);
                return ismouse_left;
            } else return false;
        }
    protected:
        const char* const text;
    };


    class GUIBox : public GUIElement {
    public:
        GUIBox (float pos_x, float pos_y, float pos_w, float pos_h, float line_thickness, bool rounded_corners) : 
            GUIElement (pos_x, pos_y, pos_w, pos_h),
            thickness (line_thickness),
            corners (rounded_corners) {}

        void Draw() {
            Render::SetGlyph(pos_x-4,  pos_y-4,     0,  0, 20, 20, 1, 1, thickness, 4, Render::COLOR_WHITE);    // top-left corner
            Render::SetGlyph(pos_x-4,  pos_y + pos_h - 16,  0, 28, 20, 20, 1, 1, thickness, 4, Render::COLOR_WHITE);    // bottom-left corner
            Render::SetGlyph(pos_x + pos_w-16, pos_y-4,    28,  0, 20, 20, 1, 1, thickness, 4, Render::COLOR_WHITE);    // top-right corner
            Render::SetGlyph(pos_x + pos_w-16, pos_y + pos_h-16,   28, 28, 20, 20, 1, 1, thickness, 4, Render::COLOR_WHITE);    // bottom-right corner

            Render::SetGlyph(pos_x+16, pos_y-4, 20, 0, 8, 10, (pos_w-(16*2))/8, 1, thickness, 4, Render::COLOR_WHITE);      // top
            Render::SetGlyph(pos_x+16, pos_y + pos_h-6, 20, 0, 8, 10, (pos_w-(16*2))/8, 1, thickness, 4, Render::COLOR_WHITE);

            Render::SetGlyph(pos_x-4, pos_y+16, 0, 20, 10, 8, 1, (pos_h-(16*2))/8, thickness, 4, Render::COLOR_WHITE);     // left
            Render::SetGlyph(pos_x + pos_w-6, pos_y+16, 0, 20, 10, 8, 1, (pos_h-(16*2))/8, thickness, 4, Render::COLOR_WHITE);
        }
    protected:
        float thickness;
        bool corners;
    };


    class GUIMarker : public GUIElement {
    public:
        GUIMarker(const char* const text, const glm::vec3& position) : 
            GUIElement (0.0f, 0.0f, 0.0f, 0.0f),
            text (text),
            pos (position) {}
        void Draw() {
            screen_pos = glm::project(pos, Render::matrices.view, Render::matrices.projection, glm::vec4 (0.0f, 0.0f, 640.0f, 480.0f));
            Render::SetGlyph(screen_pos.x, 480.0f - screen_pos.y, 134, 0, 24, 24, 1, 1, 1, 4, Render::COLOR_WHITE);
            SetText(text, screen_pos.x-100.0f, 480.0f + 25.0f - screen_pos.y, 1.0f, 200.0f, 0, 1, 1, Render::COLOR_WHITE);
        }
    protected:
        const char* const text;
        glm::vec3 pos;
        glm::vec3 screen_pos;
    };

}




#endif // UI_H
