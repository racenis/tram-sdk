// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_GUI_H
#define FRAMEWORK_GUI_H

#include <framework/ui.h>

namespace tram::Render {
    class Sprite;
}

namespace tram::GUI {
    typedef uint32_t font_t;
    
    enum orientation:uint32_t {
        TEXT_LEFT,
        TEXT_CENTER,
        TEXT_RIGHT,
        TEXT_JUSTIFIED,
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
        BUTTON_DROPDOWN = 64,
        CORNER_TOP_LEFT = 0,
        CORNER_TOP_RIGHT = 1,
        CORNER_BOTTOM_LEFT = 2,
        CORNER_BOTTOM_RIGHT = 3,
        CORNER_TOP = 4,
        CORNER_RIGHT = 5,
        CORNER_LEFT = 6,
        CORNER_BOTTOM = 7,
        CORNER_CENTER = 8,
        BOX_PLAIN = 80,
        SCROLL_TRACK = 89,
        SCROLL_BAR = 90,
    };
    
    void Init();
    void Update();
    font_t RegisterFont(Render::Sprite* sprite);
    
    void Begin();
    void End();
    
    void Frame(orientation frame_orient, float offset);
    void Frame(orientation frame_orient, float width, float height);
    void FrameBreakLine();
    void EndFrame();  // pops a frame
    void FillFrame(const float& tex_x, const float& tex_y, const float& tex_w, const float& tex_h, const glm::vec3& color, const uint32_t& tex);
    void ScrollBar(float& scroll, float& height); // scrollbar for the frame
    void Glyph(const uint32_t& symbol, const float& x, const float& y);
    void Glyph(const float& x, const float& y, const float& w, const float& h, const float& tex_x, const float& tex_y, const glm::vec3& color, const uint32_t& tex);     // new glyph using the top of the stack
    void GlyphText(char const* text, font_t font, float x, float y, float space, const glm::vec3& color = Render::COLOR_WHITE);
    void DebugText(char const* text, const glm::vec3& location, const glm::vec3& color = Render::COLOR_WHITE);
    void FrameBorder();
    bool SmallButton(const symbol& glyph);
    void CheckBox(bool& check);
    void RadioButton(uint32_t& val, const uint32_t& this_val);
    bool Button(char const* text);
    
    void Text(char const* text, font_t font, orientation alignment = TEXT_LEFT, const glm::vec3& color = Render::COLOR_WHITE);
    
    void TextBox(char* text, uint32_t max_len);
    void DropdownBox(char const** texts, uint32_t len, uint32_t& selected);
}

#endif // FRAMEWORK_GUI_H