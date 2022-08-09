// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// GUI.H -- graphical user interface.

#ifndef GUI_H
#define GUI_H

#include <ui.h>

namespace Core::GUI {
    typedef uint32_t font_t;
    
    enum orientation {
        FRAME_CENTER,
        FRAME_TOP,
        FRAME_BOTTOM,
        FRAME_LEFT,
        FRAME_RIGHT,
        FRAME_TOP_RIGHT,
        FRAME_BOTTOM_RIGHT,
        FRAME_TOP_LEFT,
        FRAME_BOTTOM_LEFT,
    };
    
    enum symbol:uint32_t {
        SELECTED = 1,
        PRESSED = 2,
        DISABLED = 3,
        LEFT = 0,
        MIDDLE = 4,
        RIGHT = 8,
        BUTTON_CROSS = 0,
        BUTTON_QUESTION = 4,
        BUTTON_MINIMIZE = 8,
        BUTTON_MAXIMIZE = 12,
        BUTTON_UP = 16,
        BUTTON_DOWN = 20,
        BUTTON_LEFT = 24,
        BUTTON_RIGHT = 28,
        BUTTON_RADIO = 32,
        BUTTON_CHECHBOX = 36,
        BUTTON_TEXT = 40,
        BUTTON_TEXTBOX = 52,
    };
    
    void Begin();
    void End();
    
    void Frame(orientation frame_orient, float offset);
    void Frame(orientation frame_orient, float width, float height);
    void FrameBreakLine();
    void EndFrame();  // pops a frame
    void ScrollBar(); // scrollbar for the frame
    void Glyph(const uint32_t& symbol, const float& x, const float& y);
    void Glyph(const float& x, const float& y, const float& w, const float& h, const float& tex_x, const float& tex_y, const glm::vec3& color, const uint32_t& tex);     // new glyph using the top of the stack
    void FrameBorder();
    bool SmallButton(const symbol& glyph);
    void CheckBox(bool& check);
    void RadioButton(uint32_t& val, const uint32_t& this_val);
    bool Button(char const* text);
    
    void Text(char const* text, font_t font);
    
    void TextBox(char* text, uint32_t max_len);
    void DropdownBox(char** texts, uint32_t len);
    
}

#endif // GUI_H