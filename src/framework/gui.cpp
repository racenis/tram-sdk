// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <templates/stack.h>

#include <framework/gui.h>

#include <framework/system.h>
#include <framework/logging.h>
#include <framework/event.h>
#include <framework/language.h>

#include <render/api.h>
#include <render/vertices.h>
#include <render/sprite.h>
#include <render/material.h>

#include <cstring>

/**
 * @namespace tram::GUI
 * 
 * Immediate mode GUI system.
 * 
 * If you don't like it, you can always integrate imgui, but I personally very
 * much prefer this way of building GUIs.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/gui.html
 */

namespace tram::GUI {

struct FrameObject {
    uint32_t x, y, w, h;
    uint32_t cursor_x, cursor_y;
    uint32_t stack_height;
};

Stack<FrameObject> frame_stack("GUI Frame stack", 100);

Render::Sprite* fonts [16] = {nullptr};

Render::vertexarray_t glyphvertices_vertex_array = {.generic = 0};
Render::drawlistentry_t glyphvertices_entry;

UI::CursorType current_cursor = UI::CURSOR_DEFAULT;
bool cursor_dirty = false;
bool cursor_set = false;

static uint32_t scaling = 1;

std::vector<Render::SpriteVertex> glyphvertices;

static std::vector<uint16_t> keycode_queue;
static char* selected_text_string = nullptr;

void SetSelectedText(char* text) {
    selected_text_string = text;
}

static vec3 user_color = Render::COLOR_WHITE;
static vec3 glyph_color = Render::COLOR_WHITE;

static void RestoreUserColor() {
    glyph_color = user_color;
}

static void GlyphColor(vec3 color) {
    glyph_color = color;
}

void SetColor(vec3 color) {
    user_color = color;
    glyph_color = color;
}

void SetScaling(uint32_t scale) {
    scaling = scale;
}

uint32_t GetScaling() {
    return scaling;
}

void SetCursorDelayed(UI::CursorType cursor) {
    if (current_cursor != cursor) {
        current_cursor = cursor;
        cursor_dirty = true;
    }
    cursor_set = true;
}

using namespace tram::Render::API;

/// Performs initialization of the GUI system.
/// Render system must be initialized first.
void Init() {
    assert(!System::IsInitialized(System::SYSTEM_GUI) && "GUI system already initialized!");
    assert(System::IsInitialized(System::SYSTEM_RENDER) && "Need to initialize Render system first!");
    
    Log (SEVERITY_INFO, System::SYSTEM_GUI, "Initializing GUI system.");
    
    using namespace tram::Render;
    
    CreateVertexArray(GetVertexDefinition(VERTEX_SPRITE), glyphvertices_vertex_array);
    glyphvertices_entry = InsertDrawListEntry();
    SetDrawListVertexArray(glyphvertices_entry, glyphvertices_vertex_array);
    SetDrawListShader(glyphvertices_entry, VERTEX_SPRITE, MATERIAL_GLYPH);
    SetFlags(glyphvertices_entry, FLAG_RENDER /*| FLAG_NO_DEPTH_TEST*/);
    SetLayer(glyphvertices_entry, 2);
    
    Event::AddListener(Event::KEYCHAR, [](Event& evt) {
        keycode_queue.push_back(evt.subtype);
    });
    
    System::SetInitialized(System::SYSTEM_GUI, true);
}

/// Submits registered fonts to the renderer.
void UpdateDrawListFonts() {
    Render::texturehandle_t glyphvertices_textures[16];
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
void Update() {
    using namespace tram::Render;
    UpdateVertexArray(glyphvertices_vertex_array, glyphvertices.size() * sizeof(SpriteVertex), &glyphvertices[0]);
    SetDrawListIndexRange(glyphvertices_entry, 0, glyphvertices.size());
    glyphvertices.clear();
    keycode_queue.clear();
    
    // this logic is not very logical, but it should work
    if (cursor_dirty) {
        UI::SetCursor(current_cursor);
        std::cout << "setting cursor to: " << current_cursor << std::endl;
        cursor_dirty = false;
    } else {
        if (!cursor_set && current_cursor != UI::CURSOR_DEFAULT) {
            UI::SetCursor(UI::CURSOR_DEFAULT);
            current_cursor = UI::CURSOR_DEFAULT;
        }
    }
    cursor_set = false;
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
void SetGlyph(float x, float y, float z, float w, float h, float tex_x, float tex_y, float tex_w, float tex_h, const vec3& color, font_t font) {
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


uint32_t GlyphWidth(font_t font, glyph_t glyph) {
    return fonts[font]->GetFrames()[glyph].width;
}

uint32_t GlyphHeight(font_t font, glyph_t glyph) {
    return fonts[font]->GetFrames()[glyph].height;
}

uint32_t GlyphBorderV(font_t font, glyph_t glyph) {
    return fonts[font]->GetFrames()[glyph].border_v;
}

void DrawGlyph(font_t font, glyph_t glyph, uint32_t x, uint32_t y, uint32_t w = 0, uint32_t h = 0) {
    const auto& info = fonts[font]->GetFrames()[glyph];
    
    if (!w) w = info.width;
    if (!h) h = info.height;
    
    SetGlyph(x*scaling, y*scaling, frame_stack.top().stack_height, w*scaling, h*scaling, info.offset_x, info.offset_y, info.width, info.height, glyph_color, font);
}

void Glyph(font_t font, glyph_t glyph) {
    uint32_t cursor_x = frame_stack.top().cursor_x;
    uint32_t cursor_y = frame_stack.top().cursor_y;
    
    DrawGlyph(font, glyph, cursor_x, cursor_y);
    
    cursor_x += GlyphWidth(font, glyph);
    
    frame_stack.top().cursor_x = cursor_x;
    frame_stack.top().cursor_y = cursor_y;
}

/// Draws a glyph box.
void DrawBox(font_t font, glyph_t glyph, uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    const glyph_t top_lft = glyph + 0;
    const glyph_t top_mid = glyph + 1;
    const glyph_t top_rgt = glyph + 2;
    const glyph_t mid_lft = glyph + 3;
    const glyph_t mid_mid = glyph + 4;
    const glyph_t mid_rgt = glyph + 5;
    const glyph_t btm_lft = glyph + 6;
    const glyph_t btm_mid = glyph + 7;
    const glyph_t btm_rgt = glyph + 8;
    
    // Draw in the corners
    DrawGlyph(font, top_lft, x, y, 0, 0);
    DrawGlyph(font, top_rgt, x+w-GlyphWidth(font, top_rgt), y, 0, 0);
    DrawGlyph(font, btm_lft, x, y+h-GlyphHeight(font, btm_lft), 0, 0);
    DrawGlyph(font, btm_rgt, x+w-GlyphWidth(font, btm_rgt), y+h-GlyphHeight(font, btm_rgt),  0, 0);
    
    // Draw in the top and bottom sides
    DrawGlyph(font, top_mid, x+GlyphWidth(font, top_lft), y, w-GlyphWidth(font, top_rgt)-GlyphWidth(font, top_lft), 0);
    DrawGlyph(font, btm_mid, x+GlyphWidth(font, btm_lft), y+h-GlyphHeight(font, btm_mid), w-GlyphWidth(font, btm_rgt)-GlyphWidth(font, btm_lft), 0);
    
    // Draw in the left and right sides
    DrawGlyph(font, mid_lft, x, y+GlyphHeight(font, top_rgt), 0, h-GlyphWidth(font, top_lft)-GlyphWidth(font, btm_lft));
    DrawGlyph(font, mid_rgt, x+w-GlyphWidth(font, mid_rgt), y+GlyphHeight(font, top_rgt), 0, h-GlyphWidth(font, top_rgt)-GlyphWidth(font, btm_rgt));
    
    // Draw in the middle
    DrawGlyph(font, mid_mid, x+GlyphWidth(font, mid_lft), y+GlyphHeight(font, top_mid), w-GlyphWidth(font, mid_lft)-GlyphWidth(font, mid_rgt), h-GlyphHeight(font, top_mid)-GlyphHeight(font, btm_mid));
}

uint32_t TextWidth(font_t font, const char* text) {
    uint32_t width = 0;
    for (const char* c = text; *c != '\0'; c++) {
        width += GlyphWidth(font, *c);
    }
    return width;
}

void Text(font_t font, const char* text, uint32_t orientation) {
    uint32_t cursor_x = frame_stack.top().cursor_x;
    uint32_t cursor_y = frame_stack.top().cursor_y;
    // depending on alignment we could also choose other cursor_x and cursor_y
    
    switch (orientation) {
        case TEXT_LEFT:
        default: 
            break;
        case TEXT_CENTER:
            cursor_x += (frame_stack.top().w - TextWidth(font, text)) / 2;
            break;
        case TEXT_RIGHT:
            cursor_x += frame_stack.top().w - TextWidth(font, text);
            break;
        case TEXT_JUSTIFIED:
            // TODO: implement
            break;
    }
    
    for (const char* c = text; *c != '\0'; c++) {
        if (*c=='\n'){ NewLine(LINE_LOW);
            cursor_x = frame_stack.top().cursor_x;
        cursor_y = frame_stack.top().cursor_y;
            continue;}
        DrawGlyph(font, (unsigned char)*c, cursor_x, cursor_y);
        cursor_x += GlyphWidth(font, (unsigned char)*c);
    }
    
    frame_stack.top().cursor_x = cursor_x;
    frame_stack.top().cursor_y = cursor_y;
}

void DrawBoxHorizontal(font_t font, glyph_t glyph,  uint32_t x, uint32_t y, uint32_t w) {
    const glyph_t lft = glyph + 0;
    const glyph_t mid = glyph + 1;
    const glyph_t rgt = glyph + 2;
    
    DrawGlyph(font, lft, x, y, 0, 0);
    DrawGlyph(font, rgt, x+w-GlyphWidth(font, lft)-GlyphWidth(font, rgt), y, 0, 0);
    DrawGlyph(font, mid, x+GlyphWidth(font, lft), y, w-GlyphWidth(font, lft)-GlyphWidth(font, rgt), 0);
}

void PushFrame(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    uint32_t stack_height = frame_stack.top().stack_height;
    
    FrameObject* new_frame = frame_stack.AddNew();
    
    new_frame->x = x;
    new_frame->y = y;
    new_frame->w = w;
    new_frame->h = h;
    new_frame->cursor_x = x;
    new_frame->cursor_y = y;
    new_frame->stack_height = stack_height + 1;
}

void PushFrameRelative(uint32_t orientation, uint32_t offset) {
    uint32_t x = frame_stack.top().x;
    uint32_t y = frame_stack.top().y;
    uint32_t w = frame_stack.top().w;
    uint32_t h = frame_stack.top().h;
    
    switch (orientation) {
        default: break;
        case FRAME_LEFT:
            w = offset;
            break;
        case FRAME_RIGHT:
            x += w - offset;
            w = offset;
            break;
        case FRAME_TOP:
            h = offset;
            break;
        case FRAME_BOTTOM:
            y += h - offset;
            h = offset;
            break;
        case FRAME_LEFT_INV:
            x += offset;
            w -= offset;
            break;
        case FRAME_RIGHT_INV:
            w -= offset;
            break;
        case FRAME_TOP_INV:
            y += offset;
            h -= offset;
            break;
        case FRAME_BOTTOM_INV:
            h -= offset;
            break;
        case FRAME_INSET:
            x += offset;
            y += offset;
            w -= offset * 2;
            h -= offset * 2;
            break;
        case FRAME_CENTER_HORIZONTAL:
            x += (w - offset) / 2;
            w = offset;
            break;
        case FRAME_CENTER_VERTICAL:
            y += (h - offset) / 2;
            h = offset;
            break;
    }
    
    PushFrame(x, y, w, h);
}

void PopFrame() {
    frame_stack.Remove();
}

void PushFrameRelativeKeepCursor(uint32_t orientation, uint32_t offset, bool keep_x, bool keep_y) {
    uint32_t x = frame_stack.top().cursor_x;
    uint32_t y = frame_stack.top().cursor_y;
    PushFrameRelative(orientation, offset);
    if (keep_x) frame_stack.top().cursor_x = x;
    if (keep_y) frame_stack.top().cursor_y = y;
}

void PopFrameKeepCursor(bool keep_x, bool keep_y) {
    uint32_t x = frame_stack.top().cursor_x;
    uint32_t y = frame_stack.top().cursor_y;
    frame_stack.Remove();
    if (keep_x) frame_stack.top().cursor_x = x;
    if (keep_y) frame_stack.top().cursor_y = y;
}

bool CursorOver(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    uint32_t cur_x = UI::PollKeyboardAxis(UI::KEY_MOUSE_X) / scaling;
    uint32_t cur_y = UI::PollKeyboardAxis(UI::KEY_MOUSE_Y) / scaling;
    
    return cur_x > x && cur_y > y && cur_x < x + w && cur_y < y + h;
}

bool mouse_click_not_handled = true;
bool mouse_click_not_handled_late = true;

// call this to check if user just pressed click
bool ClickHandled() {
    if (mouse_click_not_handled) {
        mouse_click_not_handled = false;
        return true;
    } else {
        return false;
    }
}

// call this to check if user released click
bool ClickHandledLate() {
    if (mouse_click_not_handled_late) {
        mouse_click_not_handled_late = false;
        return true;
    } else {
        return false;
    }
}

bool Clicked() {
    return UI::PollKeyboardKey(UI::KEY_LEFTMOUSE);
}

bool Button(const char* text, bool enabled, uint32_t width) {
    uint32_t x = frame_stack.top().cursor_x;
    uint32_t y = frame_stack.top().cursor_y;
    uint32_t w = width ? width : TextWidth(2, text) + 16;
    uint32_t h = 22;
    
    glyph_t style = WIDGET_BUTTON;
    
    if (!enabled) {
        style = WIDGET_BUTTON_DISABLED;
    } else if (CursorOver(x, y, w, h)) {
        if (Clicked()) {
            style = WIDGET_BUTTON_PRESSED;
        } else {
            style = WIDGET_BUTTON_SELECTED_ENABLED;
        }
        
        SetCursorDelayed(UI::CURSOR_CLICK);
    }
    
    GlyphColor(Render::COLOR_WHITE);
    DrawBox(0, style, x, y, w, h);
    
    PushFrame(x, y + 3, w, h);
    GlyphColor(enabled ? Render::COLOR_BLACK : Render::COLOR_GRAY);
    Text(1, text, TEXT_CENTER);
    PopFrame();
    
    frame_stack.top().cursor_x += w;
    
    RestoreUserColor();
    
    return enabled && CursorOver(x, y, w, h) && ClickHandledLate();
}

bool RadioButton(uint32_t index, uint32_t& selected, const char* text, bool enabled) {
    uint32_t x = frame_stack.top().cursor_x;
    uint32_t y = frame_stack.top().cursor_y;
    
    glyph_t style = WIDGET_RADIO_BUTTON;
    
    if (enabled && index == selected) style += 1;
    if (!enabled && index == selected) style += 5;
    if (!enabled && index != selected) style += 4;
    
    GlyphColor(Render::COLOR_WHITE);
    DrawGlyph(0, style, x, y);
    RestoreUserColor();
    
    frame_stack.top().cursor_x += GlyphWidth(0, style);
    
    if (text) Text(1, text, TEXT_LEFT);
    
    if (enabled && CursorOver(x, y, frame_stack.top().cursor_x - x, 24)) {
        if (ClickHandledLate()) {
            selected = index;
            return true;
        }
        
        SetCursorDelayed(UI::CURSOR_CLICK);
    }
    
    return false;
}

bool CheckBox(bool& selected, const char* text, bool enabled) {
    uint32_t x = frame_stack.top().cursor_x;
    uint32_t y = frame_stack.top().cursor_y;
    
    glyph_t style = WIDGET_CHECK_BUTTON;
    
    if (enabled && selected) style += 1;
    if (!enabled && selected) style += 5;
    if (!enabled && !selected) style += 4;
    
    GlyphColor(Render::COLOR_WHITE);
    DrawGlyph(0, style, x, y);
    RestoreUserColor();
    
    frame_stack.top().cursor_x += GlyphWidth(0, style);
    
    if (text) Text(1, text, TEXT_LEFT);
    
    if (enabled && CursorOver(x, y, frame_stack.top().cursor_x - x, 24)) {
        if (ClickHandledLate()) {
            selected = !selected;
            return true;
        }
        
        SetCursorDelayed(UI::CURSOR_CLICK);
    }
    
    return false;
}

bool Slider(float& value, bool enabled, uint32_t width) {
    uint32_t x = frame_stack.top().cursor_x;
    uint32_t y = frame_stack.top().cursor_y;
    uint32_t w = width ? width : 100;
    uint32_t h = 22;
    
    glyph_t style = WIDGET_SLIDER_HORIZONTAL;
    
    if (!enabled) {
        style += 4;
    } else if (CursorOver(x, y, w, h)) {
        if (Clicked()) {
            style += 3;
        } else {
            style += 2;
        }
        
        SetCursorDelayed(UI::CURSOR_CLICK);
    }
    
    GlyphColor(Render::COLOR_WHITE);
    //DrawBox(0, style, x, y, w, h);
    DrawBoxHorizontal(0, WIDGET_SLIDER_TRACK_HORIZONTAL, x, y + 8, w);
    
    PushFrame(x, y + 3, w, h);
    //GlyphColor(enabled ? Render::COLOR_BLACK : Render::COLOR_GRAY);
    //Text(1, text, TEXT_CENTER);
    DrawGlyph(0, style, x + (uint32_t)(value * w) - (GlyphWidth(0, style) / 2), y);
    PopFrame();
    
    frame_stack.top().cursor_x += w;
    
    RestoreUserColor();
    
    if (enabled && CursorOver(x, y, w, h) && Clicked()) {
        
        uint32_t cur_x = UI::PollKeyboardAxis(UI::KEY_MOUSE_X) / scaling;
        uint32_t progress = cur_x - x;
        value = (float)progress / (float)w;
        std::cout << cur_x << " prog " << progress << std::endl;
        return true;
    } else {
        return false;
    }
}

void NewLine(uint32_t line) {
    frame_stack.top().cursor_x = frame_stack.top().x;
    switch (line) {
        case LINE_NORMAL:
        default:
            frame_stack.top().cursor_y += 24;
            break;
        case LINE_LOW:
            frame_stack.top().cursor_y += 16;
            break;
        case LINE_HIGH:
            frame_stack.top().cursor_y += 32;
            break;
    }
}

void HorizontalDivider() {
    NewLine();
    uint32_t x = frame_stack.top().cursor_x;
    uint32_t y = frame_stack.top().cursor_y;
    uint32_t w = frame_stack.top().cursor_x - frame_stack.top().x + frame_stack.top().w;
    GlyphColor(Render::COLOR_WHITE);
    DrawBoxHorizontal(0, WIDGET_DIVIDER_HORIZONTAL, x, y, w);
    RestoreUserColor();
    frame_stack.top().cursor_x = frame_stack.top().x;
    frame_stack.top().cursor_y += GlyphHeight(0, WIDGET_DIVIDER_HORIZONTAL) + 4;
}

void FillFrame(font_t font, glyph_t glyph) {
    GlyphColor(Render::COLOR_WHITE);
    DrawBox(font, glyph, frame_stack.top().x, 
                         frame_stack.top().y, 
                         frame_stack.top().w, 
                         frame_stack.top().h);
    frame_stack.top().stack_height++;
    RestoreUserColor();
}

bool TextBox(char* text, uint32_t length, bool enabled, uint32_t w, uint32_t h) {
    uint32_t x = frame_stack.top().cursor_x;
    uint32_t y = frame_stack.top().cursor_y;
    if (w == 0) w = 100;
    if (h == 0) h = 22;
    
    glyph_t style = enabled ? WIDGET_TEXT_BOX : WIDGET_TEXT_BOX_DISABLED;
    
    bool allow_typing = selected_text_string == text;
    bool text_changed = false;
    
    if (allow_typing) {
        uint32_t current_length = strlen(text);
        
        for (uint16_t code : keycode_queue) {
            if (code == 8 && current_length) {
                text[current_length - 1] = '\0';
                text_changed = true;
                continue;
            }
            if (code == 10) {
                selected_text_string = nullptr;
                text_changed = true;
                continue;
            }
            if (code < 256 && current_length + 1 < length) {
                text[current_length] = code;
                text[current_length + 1] = '\0';
                text_changed = true;
                continue;
            }
            if (current_length + 1 < length) {
                text[current_length] = Language::UTF16ToLatin7(code);
                text[current_length + 1] = '\0';
                text_changed = true;
            }
        }
    }
    
    if (CursorOver(x, y, w, h)) {
        if (ClickHandled()) {
            selected_text_string = text;
        }
        
        SetCursorDelayed(UI::CURSOR_TEXT);
    }
    
    GlyphColor(Render::COLOR_WHITE);
    DrawBox(0, style, x, y, w, h);
    
    PushFrame(x + 4, y + 3, w - 8, h);
    GlyphColor(Render::COLOR_BLACK);
    Text(1, text, TEXT_LEFT);
    if (allow_typing && (GetTick() & 16)) Text(1, "|", TEXT_LEFT);
    PopFrame();
    
    frame_stack.top().cursor_x += w;
    
    RestoreUserColor();
    
    return text_changed;
}

void TextBox(const char* text, uint32_t w, uint32_t h) {
    uint32_t x = frame_stack.top().cursor_x;
    uint32_t y = frame_stack.top().cursor_y;
    if (w == 0) w = 100;
    if (h == 0) h = 22;

    if (CursorOver(x, y, w, h)) {
        SetCursorDelayed(UI::CURSOR_TEXT);
    }
    
    GlyphColor(Render::COLOR_WHITE);
    DrawBox(0, WIDGET_TEXT_BOX_DISABLED, x, y, w, h);
    
    PushFrame(x + 4, y + 3, w - 8, h);
    GlyphColor(Render::COLOR_BLACK);
    Text(1, text, TEXT_LEFT);
    PopFrame();
    
    frame_stack.top().cursor_x += w;
    
    RestoreUserColor();
}

void Begin() {
    
    // The first frame takes up the whole screen.
    // After the first frame is created, the user will be able to split it into
    // smaller frames and use them to position the widgets on the screen.
    FrameObject* first_frame = frame_stack.AddNew();
    
    first_frame->x = 0;
    first_frame->y = 0;
    first_frame->w = UI::GetScreenWidth() / scaling;
    first_frame->h = UI::GetScreenHeight() / scaling;
    first_frame->cursor_x = 0;
    first_frame->cursor_y = 0;
    first_frame->stack_height = 0;
    
    // Resets cursor -> widgets might change it to something else.
    //UI::SetCursor(UI::CURSOR_DEFAULT);
    
    // This is set up so that only a single button or other widget can handle
    // the mouse click for the duration of it.
    static bool prev_mouse = false;
    if (UI::PollKeyboardKey(UI::KEY_LEFTMOUSE) && !prev_mouse) {
        mouse_click_not_handled = true;
        
        // clicking outside of textbox cancels editing
        selected_text_string = nullptr;
    } else {
        mouse_click_not_handled = false;
    }
    if (!UI::PollKeyboardKey(UI::KEY_LEFTMOUSE) && prev_mouse) {
        mouse_click_not_handled_late = true;
    } else {
        mouse_click_not_handled_late = false;
    }
    prev_mouse = UI::PollKeyboardKey(UI::KEY_LEFTMOUSE);
    
    //SetGlyph(0, 0, 0, 256, 256, 0, 0, 256, 256, Render::COLOR_WHITE, 0);
    
    //GUI::PushFrameRelative(FRAME_TOP_INV, 100);
    //static char e[100] = "textanto";
    //TextBox(e, 100);
    //static float slider = 0.0f;
    //Slider(slider, true, 100);
    //GUI::PopFrame();

    return;
    //Text(2, "New text Font this is text!");
    
    //if (Button("this is a button")) std::cout << "yeea" << std::endl;
    
    static uint32_t selected = 0;
    static bool sected = false;
    RadioButton(0, selected, "ee");
    //NewLine();
    RadioButton(1, selected, "uuuu");
    CheckBox(sected, "tooting");
    
    //NewLine();
    HorizontalDivider();
    //NewLine();
    Text(1, "choochking toopi nnot", TEXT_LEFT);
    
    PushFrame(200, 200, 100, 50);
    FillFrame(0, WIDGET_BUTTON);
    PushFrameRelative(FRAME_INSET, 5);
    Text(1, "toop ing tooting", TEXT_LEFT);
    PopFrame();
    PopFrame();
}

void End() {
    frame_stack.Reset();
}


}