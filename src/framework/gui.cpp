// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <templates/stack.h>

#include <framework/gui.h>

#include <framework/system.h>
#include <framework/logging.h>

#include <render/api.h>
#include <render/vertices.h>
#include <render/sprite.h>
#include <render/material.h>

// This whole file is a mess.
// TODO: fix

// A thing that could be done is that all of the floating-point coordinates
// could be converted to integers, so that it is easier to work with them.

// Also doing const float& all over the place wasn't the best idea.

namespace tram::GUI {

struct FrameObject {
    float offset_x = 0.0f;
    float offset_y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    float cursor_x = 0.0f;
    float cursor_y = 0.0f;
    float cursor_s = 0.0f;
    float* scroll_h = nullptr;
    
    int stack_height = 0;
};

const float LINE_HEIGHT = 24.0f;
const float SPACE_WIDTH = 2.0f;

UI::CursorType cursor;

float cursor_x = 0.0f;
float cursor_y = 0.0f;
bool is_mouse_left = false;
bool is_not_mouse_left = false;
bool was_mouse_left = false;


Stack<FrameObject> FrameStack ("GUI Frame stack", 100);

char* current_text = nullptr;
uint32_t current_text_len = 0;
char const** current_dropdown = nullptr;

Render::Sprite* fonts [16] = {nullptr};

uint32_t glyphvertices_vertex_array = 0;
uint32_t glyphvertices_vertex_buffer = 0;
Render::drawlistentry_t glyphvertices_entry;

std::vector<Render::SpriteVertex> glyphvertices;

/// Performs initialization of the GUI system.
/// Render system must be initialized first.
void Init() {
    assert(!System::IsInitialized(System::SYSTEM_GUI) && "GUI system already initialized!");
    assert(System::IsInitialized(System::SYSTEM_RENDER) && "Need to initialize Render system first!");
    
    Log (SEVERITY_INFO, System::SYSTEM_GUI, "Initializing GUI system.");
    
    using namespace tram::Render;
    
    CreateVertexArray(GetVertexDefinition(VERTEX_SPRITE), glyphvertices_vertex_buffer, glyphvertices_vertex_array);
    glyphvertices_entry = InsertDrawListEntry();
    SetDrawListVertexArray(glyphvertices_entry, glyphvertices_vertex_array);
    SetDrawListShader(glyphvertices_entry, VERTEX_SPRITE, MATERIAL_GLYPH);
    SetFlags(glyphvertices_entry, FLAG_RENDER /*| FLAG_NO_DEPTH_TEST*/);
    
    System::SetInitialized(System::SYSTEM_GUI, true);
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
        
        if (fonts[i]->GetStatus() != Resource::READY) {
            Log (SEVERITY_ERROR, System::SYSTEM_GUI, "Font {} is not loaded!", i);
        }
    }
    
    using namespace tram::Render;
    
    SetDrawListTextures(glyphvertices_entry, font_count, glyphvertices_textures);
}

/// Submits all of the glyphs for rendering.
void Update () {
    using namespace tram::Render;
    UpdateVertexArray(glyphvertices_vertex_buffer, glyphvertices.size() * sizeof(SpriteVertex), &glyphvertices[0]);
    SetDrawListIndexRange(glyphvertices_entry, 0, glyphvertices.size());
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
            sprite->AddReference();
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
void SetGlyph(float x, float y, float z, float w, float h, float tex_x, float tex_y, float tex_w, float tex_h, const glm::vec3& color, font_t font) {
    Render::SpriteVertex tleft;   // top left
    Render::SpriteVertex tright;  // top right
    Render::SpriteVertex bleft;   // bottom left
    Render::SpriteVertex bright;  // bottom right

    tleft.co.x = x;
    tleft.co.y = y;
    tleft.co.z = z;
    tleft.texco.x = tex_x;
    tleft.texco.y = tex_y;
    
    tright.co.x = x + w;
    tright.co.y = y;
    tright.co.z = z;
    tright.texco.x = tex_x + tex_w;
    tright.texco.y = tex_y;
    
    bleft.co.x = x;
    bleft.co.y = y + h;
    bleft.co.z = z;
    bleft.texco.x = tex_x;
    bleft.texco.y = tex_y + tex_h;
    
    bright.co.x = x + w;
    bright.co.y = y + h;
    bright.co.z = z;
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
        SetGlyph(n_x + t.offset_x, n_y + t.offset_y, FrameStack.top().stack_height, n_w, n_h, n_tex_x, n_tex_y, n_w, n_h, color, tex);
        return;
    }
    
    // send off to rendering
    SetGlyph(x + t.offset_x, y + t.offset_y, FrameStack.top().stack_height, w, h, tex_x, tex_y, w, h, color, tex);
}

void GlyphNoclip(const float& x, const float& y, const float& w, const float& h, const float& tex_x, const float& tex_y, const float& tex_w, const float& tex_h, const glm::vec3& color, const uint32_t& tex) {
    auto& t = FrameStack.top();
    SetGlyph(x + t.offset_x, y + t.offset_y, FrameStack.top().stack_height, w, h, tex_x, tex_y, tex_w, tex_h, color, tex);
}

void Glyph(const uint32_t& symbol, const float& x, const float& y) {
    //auto& f = UI::glyphinfo[0][symbol]; // yeah, this will have to go
    auto& f = fonts[0]->GetFrames()[symbol];
    Glyph(x, y, f.width, f.height, f.offset_x, f.offset_y, Render::COLOR_WHITE, 0);
}

void Begin() {
    // reset the frames
    FrameStack.Reset();
    *FrameStack.AddNew() = FrameObject {.width = UI::GetScreenWidth(), .height = UI::GetScreenHeight()};
    
    // reset the cursor
    cursor = UI::CURSOR_DEFAULT;
    
    // poll mouse status
    cursor_x = UI::PollKeyboardAxis(UI::KEY_MOUSE_X);
    cursor_y = UI::PollKeyboardAxis(UI::KEY_MOUSE_Y);
    
    bool mouse_status_left = UI::PollKeyboardKey(UI::KEY_LEFTMOUSE);
    is_mouse_left = !was_mouse_left && mouse_status_left;
    is_not_mouse_left = was_mouse_left && !mouse_status_left;
    was_mouse_left = mouse_status_left;
}


void End() {
    // if cursor changed, tell UI to display new cursor
    static UI::CursorType cursor_last = cursor;
    if (cursor != cursor_last && UI::GetInputState() == UI::STATE_MENU_OPEN) UI::SetCursor(cursor);
    cursor_last = cursor;
    
    // if input text ptr changed, tell UI
    static char* last_text = current_text;
    if (last_text != current_text) {
        UI::SetTextInput(current_text, current_text_len);
    } else if (current_text && is_mouse_left) { 
        current_text = nullptr;
        UI::SetTextInput(current_text, current_text_len);
    } last_text = current_text;
    
    // reset dropdowns on click
    static char const** last_dropdown = nullptr;
    if (is_mouse_left && last_dropdown == current_dropdown) current_dropdown = nullptr;
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
    
    FrameStack.top().stack_height = last.stack_height + 1;
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
    auto& last = FrameStack.top();
    FrameObject new_frame = last;
    new_frame.stack_height += 1;
    *FrameStack.AddNew() = new_frame;
}

void FillFrame(const float& tex_x, const float& tex_y, const float& tex_w, const float& tex_h, const glm::vec3& color, const uint32_t& tex) {
    auto& f = FrameStack.top();
    GlyphNoclip(0.0f, 0.0f, f.width, f.height, tex_x, tex_y, tex_w, tex_h, color, tex);
}

inline bool IsCursored (float x, float y, float w, float h) {
    float n_x = FrameStack.top().offset_x + x;
    float n_y = FrameStack.top().offset_y + y;
    return cursor_x > n_x && cursor_y > n_y && cursor_x < n_x + w && cursor_y < n_y + h;
}

void ScrollBar(float& scroll, float& height) {
    //auto& tb = UI::glyphinfo[0][BUTTON_UP];
    //auto& bb = UI::glyphinfo[0][BUTTON_DOWN];
    //auto& tr = UI::glyphinfo[0][SCROLL_TRACK];
    //auto& st = UI::glyphinfo[0][SCROLL_BAR];
    //auto& sb = UI::glyphinfo[0][SCROLL_BAR+1];
    //auto& sm = UI::glyphinfo[0][SCROLL_BAR+2];
    auto& tb = fonts[0]->GetFrames()[BUTTON_UP];
    auto& bb = fonts[0]->GetFrames()[BUTTON_DOWN];
    auto& tr = fonts[0]->GetFrames()[SCROLL_TRACK];
    auto& st = fonts[0]->GetFrames()[SCROLL_BAR];
    auto& sb = fonts[0]->GetFrames()[SCROLL_BAR+1];
    auto& sm = fonts[0]->GetFrames()[SCROLL_BAR+2];

    auto& f = FrameStack.top();
    float x = f.width - tb.width;
    float y = 0.0f;
    float h = f.height;
    f.scroll_h = &height;
    f.cursor_y -= scroll;
    f.cursor_s = f.cursor_y;
    
    GlyphNoclip(x, y+tb.height, tb.width, h-tb.height-bb.height, tr.offset_x, tr.offset_y, tr.width, tr.height, Render::COLOR_WHITE, 0);
    if (height > f.height) {
        if (IsCursored(x, y, tb.width, tb.height)) {
            cursor = UI::CURSOR_CLICK;
            if (was_mouse_left) {
                Glyph(BUTTON_UP + PRESSED, x, y);
            } else {
                Glyph(BUTTON_UP + SELECTED, x, y);
            }
            if (is_not_mouse_left) scroll -= 8.0f;
        } else {
            Glyph(BUTTON_UP, x, y);
        }
        
        if (IsCursored(x, y+h-bb.height, bb.width, bb.height)) {
            cursor = UI::CURSOR_CLICK;
            if (was_mouse_left) {
                Glyph(BUTTON_DOWN + PRESSED, x, y+h-bb.height);
            } else {
                Glyph(BUTTON_DOWN + SELECTED, x, y+h-bb.height);
            }
            if (is_not_mouse_left) scroll += 8.0f;
        } else {
            Glyph(BUTTON_DOWN, x, y+h-bb.height);
        }
        
        if (IsCursored(0.0f, 0.0f, f.width, f.height) && UI::PollKeyboardAxis(UI::KEY_MOUSE_SCROLL)) {
            scroll += UI::PollKeyboardAxis(UI::KEY_MOUSE_SCROLL) * -8.0f;
        }
        
        if (scroll < 0.0f) scroll = 0.0f;
        if (scroll+h > height) scroll = height - h;
        
        float trk_h = h-tb.height-bb.height;
        float scr_h = trk_h * h / height;
        if (scr_h < 8.0f) scr_h = 8.0f;
        float scr_y = tb.height + ((trk_h - scr_h) * (scroll / (height-h)));
        
        float brr = scr_h - st.height - sb.height;
        Glyph(SCROLL_BAR, x, scr_y);
        Glyph(SCROLL_BAR+1, x, scr_y+sb.height+brr);
        GlyphNoclip(x, scr_y+sb.height, tb.width, brr, sm.offset_x, sm.offset_y, sm.width, sm.height, Render::COLOR_WHITE, 0);
    } else {
        Glyph(BUTTON_UP + DISABLED, x, y);
        Glyph(BUTTON_DOWN + DISABLED, x, y+h-bb.height);
    }
    
    f.width -= tb.width;
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
    //auto& f = UI::glyphinfo[0][glyph];
    auto& f = fonts[0]->GetFrames()[glyph];
    float x = FrameStack.top().cursor_x;
    float y = FrameStack.top().cursor_y + ((LINE_HEIGHT - f.height) / 2.0f);
    bool isclick = false;
    
    if (IsCursored(x, y, f.width, f.height)) {
        cursor = UI::CURSOR_CLICK;
        
        if (was_mouse_left){
            Glyph(glyph + PRESSED, x, y);
        } else {
            Glyph(glyph + SELECTED, x, y);
        }
        
        isclick = is_not_mouse_left;
    } else {
        Glyph(glyph, x, y);
    }
    
    FrameStack.top().cursor_x += f.width;
    return isclick;
}

void CheckBox(bool& check) {
    //auto& f = UI::glyphinfo[0][BUTTON_CHECHBOX];
    auto& f = fonts[0]->GetFrames()[BUTTON_CHECHBOX];
    float x = FrameStack.top().cursor_x;
    float y = FrameStack.top().cursor_y + ((LINE_HEIGHT - f.height) / 2.0f);
    
    if (IsCursored(x, y, f.width, f.height)) {
        cursor = UI::CURSOR_CLICK;
        
        if (is_not_mouse_left) check = !check;
    }
    
    if (check) {
        Glyph(BUTTON_CHECHBOX + PRESSED, x, y);
    } else {
        Glyph(BUTTON_CHECHBOX, x, y);
    }
    
    FrameStack.top().cursor_x += f.width;
}

void RadioButton(uint32_t& val, const uint32_t& this_val) {
    //auto& f = UI::glyphinfo[0][BUTTON_RADIO];
    auto& f = fonts[0]->GetFrames()[BUTTON_RADIO];
    float x = FrameStack.top().cursor_x;
    float y = FrameStack.top().cursor_y + ((LINE_HEIGHT - f.height) / 2.0f);
    
    if (IsCursored(x, y, f.width, f.height)) {
        cursor = UI::CURSOR_CLICK;
        
        if (is_not_mouse_left) val = this_val;
    }
    
    if (val == this_val) {
        Glyph(BUTTON_RADIO + PRESSED, x, y);
    } else {
        Glyph(BUTTON_RADIO, x, y);
    }
    
    FrameStack.top().cursor_x += f.width;
}

// sets a string until null-terminator
void GlyphText(char const* text, font_t font, float x, float y, float space, const glm::vec3& color ) {
    for (char const* t = text; *t != '\0'; t++) {
        if (*t == ' ') { x += space; continue; }
        //auto& f = UI::glyphinfo[font][(size_t)*t];
        auto& f = fonts[font]->GetFrames()[(size_t)*t];
        Glyph(x, y-f.drop+(LINE_HEIGHT/2.0f)+4.0f, f.width, f.height, f.offset_x, f.offset_y, color, font);
        x += f.width;
    }
}

// sets a string until an end pointer
void GlyphText(char const* text, char const* end, font_t font, float x, float y, float space, const glm::vec3& color = Render::COLOR_WHITE) {
    for (char const* t = text; t != end; t++) {
        if (*t == ' ') { x += space; continue; }
        //auto& f = UI::glyphinfo[font][(size_t)*t];
        auto& f = fonts[font]->GetFrames()[(size_t)*t];
        Glyph(x, y-f.drop+(LINE_HEIGHT/2.0f)+4.0f, f.width, f.height, f.offset_x, f.offset_y, color, font);
        x += f.width;
    }
}


// measures a string until newline
void GlyphMetrics(char const* text, font_t font, float& w, uint32_t& sp, char const*& nl) {
    char const* it = text; w = 0.0f; sp = 0;
    for (; *it != '\n' && *it != '\0'; it++) {
        if (*it == ' ') sp++;
        //w += UI::glyphinfo[font][(size_t)*it].w;
        w += fonts[font]->GetFrames()[(size_t)*it].width;
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
    auto& l = fonts[0]->GetFrames()[BUTTON_TEXT + LEFT];
    auto& m = fonts[0]->GetFrames()[BUTTON_TEXT + MIDDLE];
    auto& r = fonts[0]->GetFrames()[BUTTON_TEXT + RIGHT];
    float x = FrameStack.top().cursor_x;
    float y = FrameStack.top().cursor_y + ((LINE_HEIGHT - l.height) / SPACE_WIDTH);
    bool isclick = false;
    
    float text_w; uint32_t text_sp; char const* text_end;
    GlyphMetrics(text, 2, text_w, text_sp, text_end);
    text_w += text_sp * SPACE_WIDTH;
    uint32_t segs = (text_w - l.width) / m.width;
    float total_w = l.width + r.width + (m.width * segs);
    
    uint32_t mode = 0;
    if (IsCursored(x, y, total_w, l.height)) {
        cursor = UI::CURSOR_CLICK;
        
        if (was_mouse_left){
            mode = PRESSED;
        } else {
            mode = SELECTED;
        }
        
        isclick = is_not_mouse_left;
    }
    
    Glyph(BUTTON_TEXT + LEFT + mode, x, y);
    Glyph(BUTTON_TEXT + RIGHT + mode, x + total_w - r.width, y);
    for (uint32_t i = 0; i < segs; i++)
        Glyph(BUTTON_TEXT + MIDDLE + mode, x + l.width + (m.width * i) , y);
        
    FrameStack.top().stack_height++; // hack
    GlyphText(text, text_end, 2, std::round(x + ((total_w - text_w) / SPACE_WIDTH)) , y-1.0f, 2.0f/*, glm::vec3(0.667f, 0.667f, 0.667f)*/);
    FrameStack.top().stack_height--; // hack
    
    //std::cout << "total_w: " << total_w << " text_w: " << text_w << " x: " << x << " bongo: " << ((total_w - text_w) / 2.0f) << std::endl;
    
    FrameStack.top().cursor_x += total_w;
    return isclick;
}

void TextBox(char* text, uint32_t max_len) {
    //auto& l = UI::glyphinfo[0][BUTTON_TEXTBOX + LEFT];
    //auto& m = UI::glyphinfo[0][BUTTON_TEXTBOX + MIDDLE];
    //auto& r = UI::glyphinfo[0][BUTTON_TEXTBOX + RIGHT];        
    auto& l = fonts[0]->GetFrames()[BUTTON_TEXTBOX + LEFT];
    auto& m = fonts[0]->GetFrames()[BUTTON_TEXTBOX + MIDDLE];
    auto& r = fonts[0]->GetFrames()[BUTTON_TEXTBOX + RIGHT];
    float x = FrameStack.top().cursor_x;
    float y = FrameStack.top().cursor_y + ((LINE_HEIGHT - l.height) / SPACE_WIDTH);
    //bool isclick = false;
    
    float text_w; uint32_t text_sp; char const* text_end;
    GlyphMetrics(text, 1, text_w, text_sp, text_end);
    text_w += text_sp * SPACE_WIDTH;
    //uint32_t segs = (text_w - l.w) / m.w;
    uint32_t segs = max_len / 3;
    float total_w = l.width + r.width + (m.width * segs);
    
    //uint32_t mode = 0;
    if (IsCursored(x, y, total_w, l.height)) {
        cursor = UI::CURSOR_CLICK;
        
        if (is_mouse_left){
            current_text = text;
            current_text_len = max_len;
        }
        
        //isclick = UI::isnotmouse_left;
    }
    
    Glyph(BUTTON_TEXTBOX + LEFT, x, y);
    Glyph(BUTTON_TEXTBOX + RIGHT, x + total_w - r.width, y);
    for (uint32_t i = 0; i < segs; i++)
        Glyph(BUTTON_TEXTBOX + MIDDLE, x + l.width + (m.width * i) , y);
        
    GlyphText(text, text_end, 1, x+5.0f , y-1.0f, SPACE_WIDTH);
    
    FrameStack.top().cursor_x += total_w;
}

void DropdownBox(char const** texts, uint32_t len, uint32_t& selected) {
    auto& l = fonts[0]->GetFrames()[BUTTON_DROPDOWN + LEFT];
    auto& m = fonts[0]->GetFrames()[BUTTON_DROPDOWN + MIDDLE];
    auto& r = fonts[0]->GetFrames()[BUTTON_DROPDOWN + RIGHT];
    float x = FrameStack.top().cursor_x;
    float y = FrameStack.top().cursor_y + ((LINE_HEIGHT - l.height) / SPACE_WIDTH);
    
    float text_w; uint32_t text_sp; char const* text_end;
    GlyphMetrics(texts[0], 1, text_w, text_sp, text_end);
    text_w += text_sp * SPACE_WIDTH;
    uint32_t segs = (text_w - l.width) / m.width;
    float total_w = l.width + r.width + r.width + (m.width * segs);
    
    uint32_t mode = 0;
    if (IsCursored(x, y, total_w, r.height)) {
        cursor = UI::CURSOR_CLICK;
        
        if (is_mouse_left) {
            current_dropdown = texts;
        }
        
        mode = SELECTED;
    }
    
    FrameStack.top().stack_height++; // hack
    FrameStack.top().stack_height++; // hack
    if (current_dropdown == texts) {          
        auto& tl = fonts[0]->GetFrames()[BOX_PLAIN + CORNER_TOP_LEFT];
        auto& tr = fonts[0]->GetFrames()[BOX_PLAIN + CORNER_TOP_RIGHT];
        auto& bl = fonts[0]->GetFrames()[BOX_PLAIN + CORNER_BOTTOM_LEFT];
        auto& br = fonts[0]->GetFrames()[BOX_PLAIN + CORNER_BOTTOM_RIGHT];
        auto& l = fonts[0]->GetFrames()[BOX_PLAIN + CORNER_LEFT];
        auto& r = fonts[0]->GetFrames()[BOX_PLAIN + CORNER_RIGHT];
        auto& t = fonts[0]->GetFrames()[BOX_PLAIN + CORNER_TOP];
        auto& b = fonts[0]->GetFrames()[BOX_PLAIN + CORNER_BOTTOM];
        auto& c = fonts[0]->GetFrames()[BOX_PLAIN + CORNER_CENTER];
        float b_y = y + m.height;
        float total_h = len * 16.0f;
        float mid_w = total_w - tl.width - tr.width;
        float mid_h = total_h - tl.height - bl.height;
        
        GlyphNoclip(x, b_y, tl.width, tl.height, tl.offset_x, tl.offset_y, tl.width, tl.height, Render::COLOR_WHITE, 0);
        GlyphNoclip(x+total_w-tr.width, b_y, tr.width, tr.height, tr.offset_x, tr.offset_y, tr.width, tr.height, Render::COLOR_WHITE, 0);
        GlyphNoclip(x, b_y+total_h-bl.height, bl.width, bl.height, bl.offset_x, bl.offset_y, bl.width, bl.height, Render::COLOR_WHITE, 0);
        GlyphNoclip(x+total_w-br.width, b_y+total_h-br.height, br.width, br.height, br.offset_x, br.offset_y, br.width, br.height, Render::COLOR_WHITE, 0);
        
        GlyphNoclip(x+tl.width, b_y, mid_w, t.height, t.offset_x, t.offset_y, t.width, t.height, Render::COLOR_WHITE, 0);
        GlyphNoclip(x+tl.width, b_y+tl.height+mid_h, mid_w, b.height, b.offset_x, b.offset_y, b.width, b.height, Render::COLOR_WHITE, 0);
        
        GlyphNoclip(x, b_y+tl.height, l.width, mid_h, l.offset_x, l.offset_y, l.width, l.height, Render::COLOR_WHITE, 0);
        GlyphNoclip(x+tl.height+mid_w, b_y+tl.height, r.width, mid_h, r.offset_x, r.offset_y, r.width, r.height, Render::COLOR_WHITE, 0);
        
        GlyphNoclip(x+tl.width, b_y+tl.height, mid_w, mid_h, c.offset_x, c.offset_y, c.width, c.height, Render::COLOR_WHITE, 0);
        
        FrameStack.top().stack_height++; // hack
        for (uint32_t i = 0; i < len; i++) {
            float text_w; uint32_t text_sp; char const* text_end;
            GlyphMetrics(texts[i], 1, text_w, text_sp, text_end);
            float pred_sp = text_w + (text_sp * SPACE_WIDTH);
            GlyphText(texts[i], text_end, 1, x+5.0f , b_y - 4.0f + i*16.0f, pred_sp < total_w ? SPACE_WIDTH : total_w - pred_sp / text_sp);
            if (IsCursored(x, b_y + i*16.0f, total_w, 16.0f)) {
                cursor = UI::CURSOR_CLICK;
                if (is_mouse_left) selected = i;
            }
        }
        FrameStack.top().stack_height--; // hack
        
        mode = PRESSED;
    }
    FrameStack.top().stack_height--; // hack
    
    Glyph(BUTTON_DROPDOWN + LEFT, x, y);
    Glyph(BUTTON_DROPDOWN + RIGHT + mode, x + total_w - r.width , y);
    for (uint32_t i = 0; i < segs+1; i++)
        Glyph(BUTTON_DROPDOWN + MIDDLE, x + l.width + (m.width * i) , y);
        
    GlyphMetrics(texts[selected], 1, text_w, text_sp, text_end);
    FrameStack.top().stack_height++; // hack
    GlyphText(texts[selected], text_end, 1, x+5.0f , y-1.0f, 2.0f);
    FrameStack.top().stack_height--; // hack
    
    FrameStack.top().stack_height--; // hack
    
    FrameStack.top().cursor_x += total_w;
}

}