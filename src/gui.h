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
    
    void Begin();
    void End();
    
    void Frame(orientation frame_orient, float offset);
    void Frame(orientation frame_orient, float width, float height);
    void EndFrame();  // pops a frame
    void ScrollBar(); // scrollbar for the frame
    void Glyph(const float& x, const float& y, const float& w, const float& h, const float& tex_x, const float& tex_y, const float& tex_w, const float& tex_h, const glm::vec3& color, const uint32_t& tex);     // new glyph using the top of the stack
    void FrameBorder();
    
    void Text(char* text, font_t font, float scale);
    
}

#endif // GUI_H