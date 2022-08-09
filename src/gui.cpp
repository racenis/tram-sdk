#include <gui.h>

namespace Core::GUI {
    struct FrameObject {
        float offset_x;
        float offset_y;
        float width;
        float height;
        float cursor_x;
        float cursor_y;
    };
    
    const float LINE_HEIGHT = 24.0f;
    const float SPACE_WIDTH = 2.0f;
    
    UI::CursorType cursor;
    
    Stack<FrameObject> FrameStack ("GUI Frame stack", 100);
    
    char* current_text = nullptr;
    uint32_t current_text_len = 0;
    
    void Glyph(const float& x, const float& y, const float& w, const float& h, const float& tex_x, const float& tex_y, const glm::vec3& color, const uint32_t& tex) {
        auto& t = FrameStack.top();
        if (x < 0.0f || y < 0.0f || x + w > t.width || y + h > t.height) {
            if (x + w < 0.0f || y + h < 0.0f || x > t.width || y > t.height) {
                return;
            }
            float n_x = x, n_y = y, n_w = w, n_h = h, n_tex_x = tex_x, n_tex_y = tex_y;
            if (x < 0.0f) {
                float off = 0.0f - x;
                n_x += off; n_tex_x += off;
                n_w -= off;
            }
            if (y < 0.0f) {
                float off = 0.0f - y;
                n_y += off; n_tex_y += off;
                n_h -= off;
            }
            if (x + w > t.width) {
                float off = (x + w) - t.width;
                n_w -= off;
            }
            if (y + h > t.height) {
                float off = (y + h) - t.height;
                n_h -= off; //n_tex_h -= off;
            }
            Render::SetGlyph(n_x + t.offset_x, n_y + t.offset_y, n_w, n_h, n_tex_x, n_tex_y, n_w, n_h, color, tex);
            return;
        }
        Render::SetGlyph(x + t.offset_x, y + t.offset_y, w, h, tex_x, tex_y, w, h, color, tex);
    }
    
    void Glyph(const uint32_t& symbol, const float& x, const float& y) {
        auto& f = UI::glyphinfo[0][symbol];
        Glyph(x, y, f.w, f.h, f.x, f.y, Render::COLOR_WHITE, 0);
    }
    
    void Begin() {
        // reset the frames
        FrameStack.Reset();
        *FrameStack.AddNew() = FrameObject {0.0f, 0.0f, Render::SCREEN_WIDTH, Render::SCREEN_HEIGHT, 0.0f, 0.0f};
        cursor = UI::CURSOR_DEFAULT;
    }
    
    
    void End() {
        // if cursor changed, tell UI to display new cursor
        static UI::CursorType cursor_last = cursor;
        if (cursor != cursor_last) UI::SetCursor(cursor);
        cursor_last = cursor;
        
        // if input text ptr changed, tell UI
        static char* last_text = current_text;
        if (last_text != current_text) {
            UI::SetTextInput(current_text, current_text_len);
        } else if (current_text && UI::ismouse_left) { 
            current_text = nullptr;
            UI::SetTextInput(current_text, current_text_len);
        } last_text = current_text;
    }
    
    void Frame(orientation frame_orient, float offset) {
        auto& last = FrameStack.top();
        switch (frame_orient) {
            case FRAME_LEFT:
                *FrameStack.AddNew() = FrameObject {last.offset_x, last.offset_y, offset, last.height, 0.0f, 0.0f};
                break;
            case FRAME_RIGHT:
                *FrameStack.AddNew() = FrameObject {last.offset_x + last.width - offset, last.offset_y, offset, last.height, 0.0f, 0.0f};
                break;
            case FRAME_TOP:
                *FrameStack.AddNew() = FrameObject {last.offset_x, last.offset_y, last.width, offset, 0.0f, 0.0f};
                break;
            case FRAME_BOTTOM:
                *FrameStack.AddNew() = FrameObject {last.offset_x, last.offset_y + last.height - offset, last.width, offset, 0.0f, 0.0f};
                break;
            default:
                abort();
        }
    }
    
    void Frame(orientation frame_orient, float width, float height) {
        // TODO: implement the rest of the orientations
        if (frame_orient != FRAME_CENTER) abort();
        
        float& pos_x = FrameStack.top().offset_x;
        float& pos_y = FrameStack.top().offset_y;
        float& size_x = FrameStack.top().width;
        float& size_y = FrameStack.top().height;
        
        *FrameStack.AddNew() = FrameObject {((size_x - width) / 2.0f) + pos_x, ((size_y - height) / 2.0f) + pos_y, width, height, 0.0f, 0.0f};
    }
    
    void FrameBorder() {
        auto& t = FrameStack.top();
        const float s = 0.001f;
        glm::vec3 tl(t.offset_x * s, t.offset_y * s, 0.0f);
        glm::vec3 tr((t.offset_x + t.width) * s, t.offset_y * s, 0.0f);
        glm::vec3 bl(t.offset_x * s, (t.offset_y + t.height) * s, 0.0f);
        glm::vec3 br((t.offset_x + t.width) * s, (t.offset_y + t.height) * s, 0.0f);
        Render::AddLine(tl, tr, Render::COLOR_PINK);
        Render::AddLine(tr, br, Render::COLOR_PINK);
        Render::AddLine(br, bl, Render::COLOR_PINK);
        Render::AddLine(bl, tl, Render::COLOR_PINK);
    }
    
        
    void EndFrame() {
        FrameStack.Remove();
    }
    
    void FrameBreakLine() {
        FrameStack.top().cursor_x = 0.0f;
        FrameStack.top().cursor_y += LINE_HEIGHT;
    }
    
    inline bool IsCursored (float x, float y, float w, float h) {
        float n_x = FrameStack.top().offset_x + x;
        float n_y = FrameStack.top().offset_y + y;
        return UI::cur_x > n_x && UI::cur_y > n_y && UI::cur_x < n_x + w && UI::cur_y < n_y + h;
    }
    
    bool SmallButton(const symbol& glyph) {
        auto& f = UI::glyphinfo[0][glyph];
        float x = FrameStack.top().cursor_x;
        float y = FrameStack.top().cursor_y + ((LINE_HEIGHT - f.h) / 2.0f);
        bool isclick = false;
        
        if (IsCursored(x, y, f.w, f.h)) {
            cursor = UI::CURSOR_CLICK;
            
            if (UI::wasmouse_left){
                Glyph(glyph + PRESSED, x, y);
            } else {
                Glyph(glyph + SELECTED, x, y);
            }
            
            isclick = UI::isnotmouse_left;
        } else {
            Glyph(glyph, x, y);
        }
        
        FrameStack.top().cursor_x += f.w;
        return isclick;
    }
    
    void CheckBox(bool& check) {
        auto& f = UI::glyphinfo[0][BUTTON_CHECHBOX];
        float x = FrameStack.top().cursor_x;
        float y = FrameStack.top().cursor_y + ((LINE_HEIGHT - f.h) / 2.0f);
        
        if (IsCursored(x, y, f.w, f.h)) {
            cursor = UI::CURSOR_CLICK;
            
            if (UI::isnotmouse_left) check = !check;
        }
        
        if (check) {
            Glyph(BUTTON_CHECHBOX + PRESSED, x, y);
        } else {
            Glyph(BUTTON_CHECHBOX, x, y);
        }
        
        FrameStack.top().cursor_x += f.w;
    }
    
    void RadioButton(uint32_t& val, const uint32_t& this_val) {
        auto& f = UI::glyphinfo[0][BUTTON_RADIO];
        float x = FrameStack.top().cursor_x;
        float y = FrameStack.top().cursor_y + ((LINE_HEIGHT - f.h) / 2.0f);
        
        if (IsCursored(x, y, f.w, f.h)) {
            cursor = UI::CURSOR_CLICK;
            
            if (UI::isnotmouse_left) val = this_val;
        }
        
        if (val == this_val) {
            Glyph(BUTTON_RADIO + PRESSED, x, y);
        } else {
            Glyph(BUTTON_RADIO, x, y);
        }
        
        FrameStack.top().cursor_x += f.w;
    }
    
    // sets a string until null-terminator
    void GlyphText(char const* text, font_t font, float x, float y, float space) {
        for (char const* t = text; *t != '\0'; t++) {
            if (*t == ' ') { x += space; continue; }
            auto& f = UI::glyphinfo[font][(size_t)*t];
            Glyph(x, y-f.drop+(LINE_HEIGHT/2.0f)+4.0f, f.w, f.h, f.x, f.y, Render::COLOR_WHITE, font);
            x += f.w;
        }
    }
    
    // sets a string until an end pointer
    void GlyphText(char const* text, char const* end, font_t font, float x, float y, float space) {
        for (char const* t = text; t != end; t++) {
            if (*t == ' ') { x += space; continue; }
            auto& f = UI::glyphinfo[font][(size_t)*t];
            Glyph(x, y-f.drop+(LINE_HEIGHT/2.0f)+4.0f, f.w, f.h, f.x, f.y, Render::COLOR_WHITE, font);
            x += f.w;
        }
    }
    
    // measures a string until newline
    void GlyphMetrics(char const* text, font_t font, float& w, uint32_t& sp, char const*& nl) {
        char const* it = text; w = 0.0f; sp = 0;
        for (; *it != '\n' && *it != '\0'; it++) {
            if (*it == ' ') sp++;
            w += UI::glyphinfo[font][(size_t)*it].w;
        } nl = it;
    }
    
    void Text(char const* text, font_t font) {
        float x = FrameStack.top().cursor_x;
        float y = FrameStack.top().cursor_y;
        float a = FrameStack.top().width - x;
        
        //GlyphText(text, font, x, y, 3.0f);
        float w; uint32_t sp; char const* end;
        GlyphMetrics(text, font, w, sp, end);
        GlyphText(text, end, font, x, y, (a-w)/sp);
    }
    
    bool Button(char const* text) {
        auto& l = UI::glyphinfo[0][BUTTON_TEXT + LEFT];
        auto& m = UI::glyphinfo[0][BUTTON_TEXT + MIDDLE];
        auto& r = UI::glyphinfo[0][BUTTON_TEXT + RIGHT];
        float x = FrameStack.top().cursor_x;
        float y = FrameStack.top().cursor_y + ((LINE_HEIGHT - l.h) / SPACE_WIDTH);
        bool isclick = false;
        
        float text_w; uint32_t text_sp; char const* text_end;
        GlyphMetrics(text, 1, text_w, text_sp, text_end);
        text_w += text_sp * SPACE_WIDTH;
        uint32_t segs = (text_w - l.w) / m.w;
        float total_w = l.w + r.w + (m.w * segs);
        
        uint32_t mode = 0;
        if (IsCursored(x, y, total_w, l.h)) {
            cursor = UI::CURSOR_CLICK;
            
            if (UI::wasmouse_left){
                mode = PRESSED;
            } else {
                mode = SELECTED;
            }
            
            isclick = UI::isnotmouse_left;
        }
        
        Glyph(BUTTON_TEXT + LEFT + mode, x, y);
        Glyph(BUTTON_TEXT + RIGHT + mode, x + total_w - r.w, y);
        for (uint32_t i = 0; i < segs; i++)
            Glyph(BUTTON_TEXT + MIDDLE + mode, x + l.w + (m.w * i) , y);
            
        GlyphText(text, text_end, 1, std::round(x + ((total_w - text_w) / SPACE_WIDTH)) , y-1.0f, 2.0f);
        
        //std::cout << "total_w: " << total_w << " text_w: " << text_w << " x: " << x << " bongo: " << ((total_w - text_w) / 2.0f) << std::endl;
        
        FrameStack.top().cursor_x += total_w;
        return isclick;
    }
    
    void TextBox(char* text, uint32_t max_len) {
        auto& l = UI::glyphinfo[0][BUTTON_TEXTBOX + LEFT];
        auto& m = UI::glyphinfo[0][BUTTON_TEXTBOX + MIDDLE];
        auto& r = UI::glyphinfo[0][BUTTON_TEXTBOX + RIGHT];
        float x = FrameStack.top().cursor_x;
        float y = FrameStack.top().cursor_y + ((LINE_HEIGHT - l.h) / SPACE_WIDTH);
        //bool isclick = false;
        
        float text_w; uint32_t text_sp; char const* text_end;
        GlyphMetrics(text, 1, text_w, text_sp, text_end);
        text_w += text_sp * SPACE_WIDTH;
        //uint32_t segs = (text_w - l.w) / m.w;
        uint32_t segs = max_len / 3;
        float total_w = l.w + r.w + (m.w * segs);
        
        //uint32_t mode = 0;
        if (IsCursored(x, y, total_w, l.h)) {
            cursor = UI::CURSOR_CLICK;
            
            if (UI::ismouse_left){
                current_text = text;
                current_text_len = max_len;
            }
            
            //isclick = UI::isnotmouse_left;
        }
        
        Glyph(BUTTON_TEXTBOX + LEFT, x, y);
        Glyph(BUTTON_TEXTBOX + RIGHT, x + total_w - r.w, y);
        for (uint32_t i = 0; i < segs; i++)
            Glyph(BUTTON_TEXTBOX + MIDDLE, x + l.w + (m.w * i) , y);
            
        GlyphText(text, text_end, 1, x+5.0f , y-1.0f, SPACE_WIDTH);
        
        FrameStack.top().cursor_x += total_w;
    }
    
    void DropdownBox(char** texts, uint32_t len) {
        
        
    }
    
    
}