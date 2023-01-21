// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// GUI.CPP -- graphical user interface implementation.

#include <templates/stack.h>

#include <framework/gui.h>

#include <framework/system.h>
#include <framework/logging.h>

#include <render/renderer.h>

namespace Core::GUI {
    struct FrameObject {
        float offset_x;
        float offset_y;
        float width;
        float height;
        float cursor_x;
        float cursor_y;
        float cursor_s;
        float* scroll_h = nullptr;
    };
    
    const float LINE_HEIGHT = 24.0f;
    const float SPACE_WIDTH = 2.0f;
    
    UI::CursorType cursor;
    
    Stack<FrameObject> FrameStack ("GUI Frame stack", 100);
    
    char* current_text = nullptr;
    uint32_t current_text_len = 0;
    char const** current_dropdown = nullptr;
    
    Render::Sprite* fonts [16] = {nullptr};
    
    uint32_t glyphvertices_vertex_array = 0;
    uint32_t glyphvertices_vertex_buffer = 0;
    Render::DrawListEntryHandle glyphvertices_entry;
    
    std::vector<Render::SpriteVertex> glyphvertices;
    
    /// Performs initialization of the GUI system.
    /// Render system must be initialized first.
    void Init() {
        // TODO: add a check for other systems

        
        Log (System::SYSTEM_GUI, SEVERITY_INFO, "Initializing GUI system.");
        
        using namespace Core::Render;
        
        CreateVertexArray(SPRITE_VERTEX_DEFINITION, glyphvertices_vertex_buffer, glyphvertices_vertex_array);
        glyphvertices_entry = InsertDrawListEntry();
        SetDrawListVertexArray(glyphvertices_entry, glyphvertices_vertex_array);
        SetDrawListShader(glyphvertices_entry, Model::SPRITE_VERTEX, Material::TEXTURE_GLYPH);
        SetFlags(glyphvertices_entry, FLAG_RENDER | FLAG_NO_DEPTH_TEST);
    }
    
    /// Submits registered fonts to the renderer.
    void UpdateDrawListFonts () {
        uint32_t glyphvertices_textures[16];
        font_t font_count = 0;
        
        for (font_t i = 0; i < 16; i++) {
            if (!fonts[i]) {
                font_count = i;
                break;
            }
            
            glyphvertices_textures[i] = fonts[i]->GetMaterial()->GetTexture();
            
            if (fonts[i]->GetStatus() != Resource::LOADED) {
                Log (System::SYSTEM_GUI, SEVERITY_INFO, "Font {} is not loaded!", font_count);
            }
        }
        
        using namespace Core::Render;
        
        SetDrawListTextures(glyphvertices_entry, font_count, glyphvertices_textures);
    }
    
    /// Submits all of the glyphs for rendering.
    void Update () {
        using namespace Core::Render;
        UpdateVertexArray(glyphvertices_vertex_buffer, glyphvertices.size() * sizeof(SpriteVertex), &glyphvertices[0]);
        SetDrawListElements(glyphvertices_entry, 0, glyphvertices.size());
        glyphvertices.clear();
    }
    
    /// Registers a font.
    /// @return     Font handle that can be used with all of the GUI widget functions.
    font_t RegisterFont (Render::Sprite* sprite) {
        if (!System::IsInitialized (System::SYSTEM_GUI)) {
            Log (System::SYSTEM_GUI, SEVERITY_ERROR, "GUI is not initialized, font {} was not registered!", sprite->GetName());
            return -1;
        }
        
        for (font_t i = 0; i < 16; i++) {
            if (fonts[i] == sprite) return i;
        }
        
        for (font_t i = 0; i < 16; i++) {
            if (fonts[i] == nullptr) {
                fonts[i] = sprite;
                sprite->AddRef();
                UpdateDrawListFonts();
                
                return i;
            }
        }
        
        Log(System::SYSTEM_GUI, SEVERITY_ERROR, "Ran out of font slots, font {} was not registered!", sprite->GetName());
        
        return -1;
    }
    
    /// Adds a glyph to rendering list.
    /// Triangularizes a glyph from its params and then it get sent off to
    /// rendering via the glyph rendering list.
    void SetGlyph (float x, float y, float w, float h, float tex_x, float tex_y, float tex_w, float tex_h, const glm::vec3& color, font_t font) {
        Render::SpriteVertex tleft;   // top left
        Render::SpriteVertex tright;  // top right
        Render::SpriteVertex bleft;   // bottom left
        Render::SpriteVertex bright;  // bottom right

        tleft.co.x = x;
        tleft.co.y = y;
        tleft.texco.x = tex_x;
        tleft.texco.y = tex_y;
        
        tright.co.x = x + w;
        tright.co.y = y;
        tright.texco.x = tex_x + tex_w;
        tright.texco.y = tex_y;
        
        bleft.co.x = x;
        bleft.co.y = y + h;
        bleft.texco.x = tex_x;
        bleft.texco.y = tex_y + tex_h;
        
        bright.co.x = x + w;
        bright.co.y = y + h;
        bright.texco.x = tex_x + tex_w;
        bright.texco.y = tex_y + tex_h;
        
        tleft.color = color;
        tleft.texture = font;
        tright.color = color;
        tright.texture = font;
        bleft.color = color;
        bleft.texture = font;
        bright.color = color;
        bright.texture = font;

        glyphvertices.push_back(bleft);
        glyphvertices.push_back(bright);
        glyphvertices.push_back(tleft);
        glyphvertices.push_back(bright);
        glyphvertices.push_back(tright);
        glyphvertices.push_back(tleft);
    }
    
    void Glyph (const float& x, const float& y, const float& w, const float& h, const float& tex_x, const float& tex_y, const glm::vec3& color, const uint32_t& tex) {
        auto& t = FrameStack.top();
        
        // check if glyph needs to be clipped
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
        
        // send off to rendering
        Render::SetGlyph(x + t.offset_x, y + t.offset_y, w, h, tex_x, tex_y, w, h, color, tex);
    }
    
    void GlyphNoclip(const float& x, const float& y, const float& w, const float& h, const float& tex_x, const float& tex_y, const float& tex_w, const float& tex_h, const glm::vec3& color, const uint32_t& tex) {
        auto& t = FrameStack.top();
        Render::SetGlyph(x + t.offset_x, y + t.offset_y, w, h, tex_x, tex_y, tex_w, tex_h, color, tex);
    }
    
    void Glyph(const uint32_t& symbol, const float& x, const float& y) {
        auto& f = UI::glyphinfo[0][symbol]; // yeah, this will have to go
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
        
        // reset dropdowns on click
        static char const** last_dropdown = nullptr;
        if (UI::ismouse_left && last_dropdown == current_dropdown) current_dropdown = nullptr;
        last_dropdown = current_dropdown;
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
    
    void Frame() {
        *FrameStack.AddNew() = FrameObject {0.0f, 0.0f, Render::SCREEN_WIDTH, Render::SCREEN_HEIGHT, 0.0f, 0.0f};
    }
    
    void FillFrame(const float& tex_x, const float& tex_y, const float& tex_w, const float& tex_h, const glm::vec3& color, const uint32_t& tex) {
        auto& f = FrameStack.top();
        GlyphNoclip(0.0f, 0.0f, f.width, f.height, tex_x, tex_y, tex_w, tex_h, color, tex);
    }
    
    inline bool IsCursored (float x, float y, float w, float h) {
        float n_x = FrameStack.top().offset_x + x;
        float n_y = FrameStack.top().offset_y + y;
        return UI::cur_x > n_x && UI::cur_y > n_y && UI::cur_x < n_x + w && UI::cur_y < n_y + h;
    }
    
    void ScrollBar(float& scroll, float& height) {
        auto& tb = UI::glyphinfo[0][BUTTON_UP];
        auto& bb = UI::glyphinfo[0][BUTTON_DOWN];
        auto& tr = UI::glyphinfo[0][SCROLL_TRACK];
        auto& st = UI::glyphinfo[0][SCROLL_BAR];
        auto& sb = UI::glyphinfo[0][SCROLL_BAR+1];
        auto& sm = UI::glyphinfo[0][SCROLL_BAR+2];
        auto& f = FrameStack.top();
        float x = f.width - tb.w;
        float y = 0.0f;
        float h = f.height;
        f.scroll_h = &height;
        f.cursor_y -= scroll;
        f.cursor_s = f.cursor_y;
        
        GlyphNoclip(x, y+tb.h, tb.w, h-tb.h-bb.h, tr.x, tr.y, tr.w, tr.h, Render::COLOR_WHITE, 0);
        if (height > f.height) {
            if (IsCursored(x, y, tb.w, tb.h)) {
                cursor = UI::CURSOR_CLICK;
                if (UI::wasmouse_left) {
                    Glyph(BUTTON_UP + PRESSED, x, y);
                } else {
                    Glyph(BUTTON_UP + SELECTED, x, y);
                }
                if (UI::isnotmouse_left) scroll -= 8.0f;
            } else {
                Glyph(BUTTON_UP, x, y);
            }
            
            if (IsCursored(x, y+h-bb.h, bb.w, bb.h)) {
                cursor = UI::CURSOR_CLICK;
                if (UI::wasmouse_left) {
                    Glyph(BUTTON_DOWN + PRESSED, x, y+h-bb.h);
                } else {
                    Glyph(BUTTON_DOWN + SELECTED, x, y+h-bb.h);
                }
                if (UI::isnotmouse_left) scroll += 8.0f;
            } else {
                Glyph(BUTTON_DOWN, x, y+h-bb.h);
            }
            
            if (IsCursored(0.0f, 0.0f, f.width, f.height) && UI::mouse_scroll) {
                scroll += UI::mouse_scroll * -8.0f;
            }
            
            if (scroll < 0.0f) scroll = 0.0f;
            if (scroll+h > height) scroll = height - h;
            
            float trk_h = h-tb.h-bb.h;
            float scr_h = trk_h * h / height;
            if (scr_h < 8.0f) scr_h = 8.0f;
            float scr_y = tb.h + ((trk_h - scr_h) * (scroll / (height-h)));
            
            float brr = scr_h - st.h - sb.h;
            Glyph(SCROLL_BAR, x, scr_y);
            Glyph(SCROLL_BAR+1, x, scr_y+sb.h+brr);
            GlyphNoclip(x, scr_y+sb.h, tb.w, brr, sm.x, sm.y, sm.w, sm.h, Render::COLOR_WHITE, 0);
        } else {
            Glyph(BUTTON_UP + DISABLED, x, y);
            Glyph(BUTTON_DOWN + DISABLED, x, y+h-bb.h);
        }
        
        f.width -= tb.w;
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
        auto &f = FrameStack.top();
        if (f.scroll_h) *f.scroll_h = f.cursor_y - f.cursor_s;
        FrameStack.Remove();
    }
    
    void FrameBreakLine() {
        FrameStack.top().cursor_x = 0.0f;
        FrameStack.top().cursor_y += LINE_HEIGHT;
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
    void GlyphText(char const* text, font_t font, float x, float y, float space, const glm::vec3& color ) {
        for (char const* t = text; *t != '\0'; t++) {
            if (*t == ' ') { x += space; continue; }
            auto& f = UI::glyphinfo[font][(size_t)*t];
            Glyph(x, y-f.drop+(LINE_HEIGHT/2.0f)+4.0f, f.w, f.h, f.x, f.y, color, font);
            x += f.w;
        }
    }
    
    // sets a string until an end pointer
    void GlyphText(char const* text, char const* end, font_t font, float x, float y, float space, const glm::vec3& color = Render::COLOR_WHITE) {
        for (char const* t = text; t != end; t++) {
            if (*t == ' ') { x += space; continue; }
            auto& f = UI::glyphinfo[font][(size_t)*t];
            Glyph(x, y-f.drop+(LINE_HEIGHT/2.0f)+4.0f, f.w, f.h, f.x, f.y, color, font);
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
    
    // projects a world-space point onto the screen, then sets a text there
    void DebugText(char const* text, const glm::vec3& location, const glm::vec3& color) {
        glm::vec3 screen_location;
        Render::Project(location, screen_location);
        float w; uint32_t sp; char const* end;
        GlyphMetrics(text, 1, w, sp, end);
        
        Frame();
        GlyphText(text, 1, std::floor(screen_location.x - ((w+SPACE_WIDTH*sp)/2.0f)), std::floor(screen_location.y), SPACE_WIDTH, color);
        EndFrame();
    }
    
    void Text(char const* text, font_t font, orientation alignment, const glm::vec3& color) {
        float x = FrameStack.top().cursor_x;
        float y = FrameStack.top().cursor_y;
        float a = FrameStack.top().width - x;
        
        //GlyphText(text, font, x, y, 3.0f);
        float w; uint32_t sp; char const* end;
        GlyphMetrics(text, font, w, sp, end);
        switch (alignment) {
            case TEXT_LEFT:
            default:
                GlyphText(text, end, font, x, y, SPACE_WIDTH, color);
                break;
            case TEXT_CENTER:
                GlyphText(text, end, font, x+((a-(w+SPACE_WIDTH*sp))/2.0f), y, SPACE_WIDTH, color);
                break;
            case TEXT_RIGHT:
                GlyphText(text, end, font, x+(a-(w+SPACE_WIDTH*sp)), y, SPACE_WIDTH, color);
                break;
            case TEXT_JUSTIFIED:
                GlyphText(text, end, font, x, y, (a-w)/sp, color);
                break;
        }
    }
    
    bool Button(char const* text) {
        auto& l = UI::glyphinfo[0][BUTTON_TEXT + LEFT];
        auto& m = UI::glyphinfo[0][BUTTON_TEXT + MIDDLE];
        auto& r = UI::glyphinfo[0][BUTTON_TEXT + RIGHT];
        float x = FrameStack.top().cursor_x;
        float y = FrameStack.top().cursor_y + ((LINE_HEIGHT - l.h) / SPACE_WIDTH);
        bool isclick = false;
        
        float text_w; uint32_t text_sp; char const* text_end;
        GlyphMetrics(text, 2, text_w, text_sp, text_end);
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
            
        GlyphText(text, text_end, 2, std::round(x + ((total_w - text_w) / SPACE_WIDTH)) , y-1.0f, 2.0f/*, glm::vec3(0.667f, 0.667f, 0.667f)*/);
        
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
    
    void DropdownBox(char const** texts, uint32_t len, uint32_t& selected) {
        auto& l = UI::glyphinfo[0][BUTTON_DROPDOWN + LEFT];
        auto& m = UI::glyphinfo[0][BUTTON_DROPDOWN + MIDDLE];
        auto& r = UI::glyphinfo[0][BUTTON_DROPDOWN + RIGHT];
        float x = FrameStack.top().cursor_x;
        float y = FrameStack.top().cursor_y + ((LINE_HEIGHT - l.h) / SPACE_WIDTH);
        //bool isclick = false;
        
        float text_w; uint32_t text_sp; char const* text_end;
        GlyphMetrics(texts[0], 1, text_w, text_sp, text_end);
        text_w += text_sp * SPACE_WIDTH;
        uint32_t segs = (text_w - l.w) / m.w;
        float total_w = l.w + r.w + r.w + (m.w * segs);
        
        uint32_t mode = 0;
        if (IsCursored(x, y, total_w, r.h)) {
            cursor = UI::CURSOR_CLICK;
            
            if (UI::ismouse_left) {
                current_dropdown = texts;
            }
            
            mode = SELECTED;
        }
        
        if (current_dropdown == texts) {
            auto& tl = UI::glyphinfo[0][BOX_PLAIN + CORNER_TOP_LEFT];
            auto& tr = UI::glyphinfo[0][BOX_PLAIN + CORNER_TOP_RIGHT];
            auto& bl = UI::glyphinfo[0][BOX_PLAIN + CORNER_BOTTOM_LEFT];
            auto& br = UI::glyphinfo[0][BOX_PLAIN + CORNER_BOTTOM_RIGHT];
            auto& l = UI::glyphinfo[0][BOX_PLAIN + CORNER_LEFT];
            auto& r = UI::glyphinfo[0][BOX_PLAIN + CORNER_RIGHT];
            auto& t = UI::glyphinfo[0][BOX_PLAIN + CORNER_TOP];
            auto& b = UI::glyphinfo[0][BOX_PLAIN + CORNER_BOTTOM];
            auto& c = UI::glyphinfo[0][BOX_PLAIN + CORNER_CENTER];
            float b_y = y + m.h;
            float total_h = len * 16.0f;
            float mid_w = total_w - tl.w - tr.w;
            float mid_h = total_h - tl.h - bl.h;
            
            GlyphNoclip(x, b_y, tl.w, tl.h, tl.x, tl.y, tl.w, tl.h, Render::COLOR_WHITE, 0);
            GlyphNoclip(x+total_w-tr.w, b_y, tr.w, tr.h, tr.x, tr.y, tr.w, tr.h, Render::COLOR_WHITE, 0);
            GlyphNoclip(x, b_y+total_h-bl.h, bl.w, bl.h, bl.x, bl.y, bl.w, bl.h, Render::COLOR_WHITE, 0);
            GlyphNoclip(x+total_w-br.w, b_y+total_h-br.h, br.w, br.h, br.x, br.y, br.w, br.h, Render::COLOR_WHITE, 0);
            
            GlyphNoclip(x+tl.w, b_y, mid_w, t.h, t.x, t.y, t.w, t.h, Render::COLOR_WHITE, 0);
            GlyphNoclip(x+tl.w, b_y+tl.h+mid_h, mid_w, b.h, b.x, b.y, b.w, b.h, Render::COLOR_WHITE, 0);
            
            GlyphNoclip(x, b_y+tl.h, l.w, mid_h, l.x, l.y, l.w, l.h, Render::COLOR_WHITE, 0);
            GlyphNoclip(x+tl.h+mid_w, b_y+tl.h, r.w, mid_h, r.x, r.y, r.w, r.h, Render::COLOR_WHITE, 0);
            
            GlyphNoclip(x+tl.w, b_y+tl.h, mid_w, mid_h, c.x, c.y, c.w, c.h, Render::COLOR_WHITE, 0);
            
            for (uint32_t i = 0; i < len; i++) {
                float text_w; uint32_t text_sp; char const* text_end;
                GlyphMetrics(texts[i], 1, text_w, text_sp, text_end);
                float pred_sp = text_w + (text_sp * SPACE_WIDTH);
                GlyphText(texts[i], text_end, 1, x+5.0f , b_y - 4.0f + i*16.0f, pred_sp < total_w ? SPACE_WIDTH : total_w - pred_sp / text_sp);
                if (IsCursored(x, b_y + i*16.0f, total_w, 16.0f)) {
                    cursor = UI::CURSOR_CLICK;
                    if (UI::ismouse_left) selected = i;
                }
            }
            
            mode = PRESSED;
        }
        
        Glyph(BUTTON_DROPDOWN + LEFT, x, y);
        Glyph(BUTTON_DROPDOWN + RIGHT + mode, x + total_w - r.w , y);
        for (uint32_t i = 0; i < segs+1; i++)
            Glyph(BUTTON_DROPDOWN + MIDDLE, x + l.w + (m.w * i) , y);
            
        GlyphMetrics(texts[selected], 1, text_w, text_sp, text_end);
        GlyphText(texts[selected], text_end, 1, x+5.0f , y-1.0f, 2.0f);
        
        FrameStack.top().cursor_x += total_w;
    }
    
    
}