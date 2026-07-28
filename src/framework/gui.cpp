// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <templates/stack.h>

#include <framework/gui.h>

#include <framework/system.h>
#include <framework/logging.h>
#include <framework/event.h>
#include <framework/language.h>
#include <framework/settings.h>

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
 * First you need to register fonts, which are just Sprites. The glyphs are just
 * sprite frame indices. There are some default fonts in the Ext::Menu system,
 * if you don't want to make your own fonts.
 * 
 * To draw widgets, just call their respective functions once per frame.
 * 
 * To position the widgets, use the frame pushing and popping functions.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/gui.html
 */

namespace tram::GUI {

struct FrameObject {
    int32_t x, y, w, h;
    int32_t cursor_x, cursor_y;
    uint32_t stack_height;
};

static Stack<FrameObject> frame_stack("GUI Frame stack", 100);

static Render::Sprite* fonts[16] = {nullptr};

// glyphvertices are where we put all of the GUI triangle vertices in
// we upload them to the GPU each frame for drawing
static Render::vertexarray_t glyphvertices_vertex_array = {};
static Render::drawlistentry_t glyphvertices_entry;

static std::vector<Render::SpriteVertex> glyphvertices;

// instead of using these, use the SetCursorDelayed() to change the pointer
static UI::CursorType current_cursor = UI::CURSOR_DEFAULT;
static bool cursor_dirty = false;
static bool cursor_set = false;

// some users have screens with way too many pixels and they just keep
// complaining that they can't see anything, so that's why I made this scaling
// feature.
static Settings::Property<int32_t> scaling = {1, "gui-scale", Settings::NONE};

// here we put all the keycodes that come in. the keycodes depend on the
// keyboard layout, unlike regular UI keys. we use them for textboxes
static std::vector<uint16_t> keycode_queue;
static char* selected_text_string = nullptr;

/// Sets the text which is selected in a textbox.
/// The way the framework remembers which TextBox you have clicked on and are
/// editing is that once a TextBox is clicked on, the framework remembers the
/// pointer of the text that is being edited.
/// The actual TextBox string is updated only when the TextBox() function is
/// called.
/// If you want to deselect any TextBox, call this function with a `nullptr`.
void SetSelectedText(char* text) {
    selected_text_string = text;
}

// this is how we remember which colors the user has set.
static Stack<Render::color_t> text_color_stack("GUI Text Color stack", 100);
static Stack<Render::color_t> widget_color_stack("GUI Widget Color stack", 100);
static Stack<font_t> text_font_stack("GUI Text Font stack", 100);
static Stack<font_t> widget_font_stack("GUI Widget Font stack", 100);
static Stack<GlyphEffect> effect_stack("GUI Effect stack", 100);
static Render::color_t default_text_color = Render::COLOR_BLACK;
static Render::color_t default_widget_color = Render::COLOR_WHITE;
static font_t default_text_font = 1;
static font_t default_widget_font = 0;

// this is for remembering whether Begin() has been called and to check whether
// we don't call Update() twice in a frame, which would break everything
uint32_t last_update_frame = 0;
bool beginned = false;

// used for checking whether a mouse click has been handled. we don't want
// multiple buttons to be clickable at the same time, since that could cause
// lots of very bad errors in the user code
bool mouse_click_not_handled = true;
bool mouse_click_not_handled_late = true;

// true if last input method was keyboard/controller, false if pointer
static bool is_keyboard = false;
static bool allow_keyboard = true;

// variables for tracking keyboard/controller navigation
static int32_t switch_tab_direction = 0;
static int32_t switch_tab_absolute = 0;
static int32_t selected_group = 0;
static int32_t selected_index = 0;
static int32_t processing_group = 0;
static int32_t processing_index = 0;
static int32_t selection_group_last_index = 0;
static int32_t selected_group_last_index = 0;

// variables for tracking thing related to events
static bool selected_something_now = false;
static bool selected_something_still = false;
static bool highlighted_something = false;
static bool activated_something = false;

/// Sets the color.
/// Pushes color to color stack. Each glyph type gets its own color stack.
/// The previously set color can be restored using RestoreColor().
/// GlyphType::TEXT will set the text rendering color of the Text() function, as
/// well as the text that is drawn by the RadioButton() and CheckBox()
/// functions, on top of all Button() buttons and in TextBox() textboxes.
/// GlyphType::WIDGET will set the color that will be used when drawing non-text
/// widget glyphs. By default it is Render::COLOR_WHITE.
/// This stack is reset every time the GUI::Begin() function is called. The
/// default values can be overriden with SetGlyphDefaults().
/// @param color RGB color that will be set. Take a look at the Render system's
///              header or docs to see what kinds of predefined colors are
///              available.
/// @param type  The type of the glyph that this color will be applied to.
void SetColor(Render::color_t color, GlyphType type) {
    switch (type) {
        case TEXT:      text_color_stack.push(color);     break;
        case WIDGET:    widget_color_stack.push(color);   break;
    }
}

/// Sets the font.
/// Works the same as SetColor(), except for fonts. A font needs to be
/// registered with RegisterFont() first. Widget fonts need to use the widget
/// font layout to be displayed properly.
void SetFont(font_t font, GlyphType type) {
    switch (type) {
        case TEXT:      text_font_stack.push(font);   break;
        case WIDGET:    widget_font_stack.push(font); break;
    }
}

/// Sets the font effect.
void SetEffect(GlyphEffect effect) {
    effect_stack.push(effect);
}

/// Restores previous glyph color.
/// See SetColor().
void RestoreColor(GlyphType type) {
    switch (type) {
        case TEXT:      text_color_stack.pop();     break;
        case WIDGET:    widget_color_stack.pop();   break;
    }
}

/// Restores previous glyph font.
/// See SetFont().
void RestoreFont(GlyphType type) {
    switch (type) {
        case TEXT:      text_font_stack.pop();   break;
        case WIDGET:    widget_font_stack.pop(); break;
    }
}

void RestoreEffect() {
    effect_stack.pop();
}

/// Overrides glyph default colors and fonts.
/// Whenever GUI::Begin() is called, the color and font passed to this function
/// will be set as the color and font used for the glyph drawing functions.
/// If you are using the Ext::Menu extension and its default fonts, you don't
/// need to call this function.
void SetGlyphDefaults(Render::color_t color, font_t font, GlyphType type) {
    switch (type) {
        case TEXT:
            default_text_color = color;
            default_text_font = font;
            break;
        case WIDGET:
            default_widget_color = color;
            default_widget_font = font;
            break;
    }
}

/// Sets the scaling factor.
/// If set to `1`, the GUI will be drawn normally. If set to `2`, then
/// everything will be drawn twice as large. If set to `3`, then everything
/// will be drawn three times as large. I don't recommend going above that.
void SetScaling(int32_t scale) {
    scaling = scale;
}

/// Returns the scaling factor.
/// See SetScaling().
int32_t GetScaling() {
    return scaling;
}

/// Sets the cursor.
/// The difference between using this function and the cursor setting function
/// in the UI system, is that this function will only switch the cursor if it
/// has been changed, i.e. it will issue the UI::SetCursor() command only if
/// this function receives a new parameter.
/// This means that GUI widgets can change the cursor, but the it is also
/// possible to change it using the UI function and this function will not
/// override it, except if the cursor is placed over a widget.
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
    System::SetState(System::GUI, System::INIT);
    System::AssertDependency(System::RENDER);
    
    Log(System::GUI, "Initializing GUI system.");
    
    using namespace tram::Render;
    
    // in the future we might want to have two vertex arrays and drawlist entries,
    // one would be for alpha tested and the other for transparent non-blended
    // materials.
    // from this arises the unfortunate problem of sorting the glyphs.
    // also we should probably switch to using sprites here.
    CreateVertexArray(GetVertexDefinition(VERTEX_SPRITE), glyphvertices_vertex_array);
    glyphvertices_entry = InsertDrawListEntry();
    SetDrawListVertexArray(glyphvertices_entry, glyphvertices_vertex_array);
    SetDrawListShader(glyphvertices_entry, VERTEX_SPRITE, MATERIAL_GLYPH);
    SetFlags(glyphvertices_entry, FLAG_RENDER);
    SetLayer(glyphvertices_entry, LAYER_GUI);
    
    Event::AddListener(Event::KEYCHAR, [](Event& evt) {
        keycode_queue.push_back(evt.subtype);
    });
    
    System::SetState(System::GUI, System::READY);
}

/// Submits registered fonts to the renderer.
void UpdateDrawListFonts() {
    Render::material_t glyphvertices_textures[16];
    font_t font_count = 0;
    
    bool is_transparent = false;
    for (font_t i = 0; i < 16; i++) {
        if (!fonts[i]) {
            font_count = i;
            break;
        }
        
        if (fonts[i]->GetMaterial()->GetType() == Render::MATERIAL_GLYPH_BLEND) {
            is_transparent = true;
        }
        
        glyphvertices_textures[i] = fonts[i]->GetMaterial()->GetMaterial();
        
        if (fonts[i]->GetStatus() != Resource::READY) {
            Log(Severity::ERROR, System::GUI, "Font {} is not loaded!", i);
        }
    }
    
    using namespace tram::Render;
    
    uint32_t flags = FLAG_RENDER;
    if (is_transparent) flags |= FLAG_TRANSPARENT;
    SetFlags(glyphvertices_entry, flags);
    
    SetDrawListMaterials(glyphvertices_entry, font_count, glyphvertices_textures);
}

// checks for keyboard/controller navigation keys, switches between keyboard
// and pointer modes
static bool IsKey(UI::KeyboardKey key) {
    static uint32_t tick_pressed = 0;
    static float curx = 0.0f;
    
    // switch out of keyboard mode when cursor moves
    if (curx != UI::PollKeyboardAxis(UI::KEY_MOUSE_X)) {
        is_keyboard = false;
    }
    
    if (!UI::PollKeyboardKey(key)) return false;
    
    // we'll ignore the first keyboard input, instead using it to switch into
    // keyboard mode
    if (!is_keyboard) {
        curx = UI::PollKeyboardAxis(UI::KEY_MOUSE_X);
        is_keyboard = true;
        tick_pressed = GetTick();
        return false;
    }
    
    const bool can_key = tick_pressed + 1 < GetTick();
    tick_pressed = GetTick();
    
    return can_key;
}

/// Submits all of the glyphs for rendering.
/// Also updates cursor, if needed by SetCursorDelayed().
void Update() {
    using namespace tram::Render;
    
    // do some error checking on the part of the library user
    if (beginned) {
        Log(Severity::CRITICAL_ERROR, System::GUI, "Called GUI::Update() without first calling GUI::End()");
    }
    
    if (last_update_frame == GetFrame()) {
        Log(Severity::WARNING, System::GUI, "Called GUI::Update() more than once in a single frame");
    }
    
    last_update_frame = GetFrame();

    
    // upload the generated glyph vertex triangles to the GPU
    UpdateVertexArray(glyphvertices_vertex_array, glyphvertices.size() * sizeof(SpriteVertex), glyphvertices.data());
    SetDrawListIndexRange(glyphvertices_entry, 0, glyphvertices.size());
    glyphvertices.clear();
    keycode_queue.clear();
    
    // this logic is not very logical, but it should work
    if (cursor_dirty) {
        UI::SetCursor(current_cursor);
        cursor_dirty = false;
    } else {
        if (!cursor_set && current_cursor != UI::CURSOR_DEFAULT) {
            UI::SetCursor(UI::CURSOR_DEFAULT);
            current_cursor = UI::CURSOR_DEFAULT;
        }
    }
    cursor_set = false;
    
    // this is set up so that only a single button or other widget can handle
    // the mouse click for the duration of it.
    const bool virtual_click = !is_keyboard ? UI::PollKeyboardKey(UI::KEY_LEFTMOUSE) : UI::PollKeyboardKey(UI::KEY_ENTER) || UI::PollKeyboardKey(UI::KEY_CONTROLLER_A);
    static bool prev_mouse = false;
    if (virtual_click && !prev_mouse) {
        mouse_click_not_handled = true;
        
        // clicking outside of textbox cancels editing
        selected_text_string = nullptr;
    } else {
        mouse_click_not_handled = false;
    }
    if (!virtual_click && prev_mouse) {
        mouse_click_not_handled_late = true;
    } else {
        mouse_click_not_handled_late = false;
    }
    prev_mouse = virtual_click;
    
    // switching selection via keyboard/controller
    
    if (selected_index > selected_group_last_index) {
        selected_index = selected_group_last_index;
    }
    
    if (IsKey(UI::KEY_UP) || IsKey(UI::KEY_CONTROLLER_DPAD_UP)) {
        selected_index--;
    }
    if (IsKey(UI::KEY_DOWN) || IsKey(UI::KEY_CONTROLLER_DPAD_DOWN)) {
        selected_index++;
    }
    
    if (selected_index > selected_group_last_index) {
        selected_group++;
        selected_index = 0;
        selected_text_string = nullptr;
    }
    if (selected_index < 0) {
        selected_group--;
        selected_index = 4000; // this will get brought down on the next frame
        selected_text_string = nullptr;
    }
    
    if (IsKey(UI::KEY_LEFT) || IsKey(UI::KEY_CONTROLLER_DPAD_LEFT)) {
        selected_group--;
    }
    if (IsKey(UI::KEY_RIGHT) || IsKey(UI::KEY_CONTROLLER_DPAD_RIGHT)) {
        selected_group++;
    }
    
    if (selected_group < 0) {
        selected_group = selection_group_last_index;
        selected_index = 4000;
        selected_text_string = nullptr;
    }
    
    if (selected_group > selection_group_last_index) {
        selected_group = 0;
        selected_index = 0;
        selected_text_string = nullptr;
    }
    
    // tab switching
    if (IsKey(UI::KEY_TAB) || IsKey(UI::KEY_CONTROLLER_RIGHT_BUMPER)) {
        switch_tab_direction = 1;
    } else if (IsKey(UI::KEY_CONTROLLER_LEFT_BUMPER)) {
        switch_tab_direction = -1;
    } else {
        switch_tab_direction = 0;
    }
    
    switch_tab_absolute = 0;
    if (IsKey(UI::KEY_1)) switch_tab_absolute = 1;
    if (IsKey(UI::KEY_2)) switch_tab_absolute = 2;
    if (IsKey(UI::KEY_3)) switch_tab_absolute = 3;
    if (IsKey(UI::KEY_4)) switch_tab_absolute = 4;
    if (IsKey(UI::KEY_5)) switch_tab_absolute = 5;
    if (IsKey(UI::KEY_6)) switch_tab_absolute = 6;
    if (IsKey(UI::KEY_7)) switch_tab_absolute = 7;
    if (IsKey(UI::KEY_8)) switch_tab_absolute = 8;
    if (IsKey(UI::KEY_9)) switch_tab_absolute = 9;
    
    processing_group = 0;
    processing_index = -1; // all widgets increment
    selection_group_last_index = 0;
    selected_group_last_index = 0;
    
    static bool selected_something_still_last = false;
    static bool activated_something_last = false;
    
    Event event;
    event.poster = 0;
    event.type = Event::GUI;
    event.subtype = 0;
    
    if (selected_something_now) {
        event.subtype |= SELECTED;
    }
    
    if (selected_something_still_last && !selected_something_still) {
        event.subtype |= DESELECTED;
    }
    
    if (event.subtype && highlighted_something) {
        event.subtype |= HIGHLIGHTED;
    }
    
    if (activated_something_last && !activated_something) {
        event.subtype |= ACTIVATED;
    }
    
    if (event.subtype) {
        Event::Post(event);
    }
    
    selected_something_still_last = selected_something_still;
    activated_something_last = activated_something;
    
    selected_something_now = false;
    selected_something_still = false;
    highlighted_something = false;
    activated_something = false;
}

/// Registers a font.
/// @return Font handle that can be used with all of the GUI widget functions.
font_t RegisterFont(Render::Sprite* sprite) {
    if (!System::IsInitialized(System::GUI)) {
        Log(Severity::ERROR, System::GUI, "GUI is not initialized, font {} was not registered!", sprite->GetName());
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
    
    Log(Severity::ERROR, System::GUI, "Ran out of font slots, font {} was not registered!", sprite->GetName());
    
    return -1;
}

/// Registers a font by replacing existing.
/// @param prev Font which will be replaced.
/// @param next Font that will instead take `prev`s place.
/// @return Font handle that can be used with all of the GUI widget functions.
font_t RegisterFontReplace(Render::Sprite* prev, Render::Sprite* next) {
    if (!System::IsInitialized(System::GUI)) {
        Log(Severity::ERROR, System::GUI, "GUI is not initialized, font {} was not registered!", next->GetName());
        return -1;
    }
    
    for (font_t i = 0; i < 16; i++) {
        if (fonts[i] == prev) {
            fonts[i] = next;
            next->AddReference();
            prev->RemoveReference();
            UpdateDrawListFonts();
            
            return i;
        }
    }
    
    Log(Severity::ERROR, System::GUI, "Replacable font {} was not found, font {} was not registered!", prev->GetName(), next->GetName());
    
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

void DrawGlyph(font_t font, glyph_t glyph, const vec3& color, int32_t x, int32_t y, uint32_t w = 0, uint32_t h = 0, int32_t rel_h = 0) {
    const auto& info = fonts[font]->GetFrames()[glyph];
    
    if (!w) w = info.width;
    if (!h) h = info.height;
    
    SetGlyph(x*scaling, y*scaling, (int32_t)frame_stack.top().stack_height + rel_h, w*scaling, h*scaling, info.offset_x, info.offset_y, info.width, info.height, color, font);
}

/// Draws a glyph from a font.
void Glyph(glyph_t glyph) {
    int32_t cursor_x = frame_stack.top().cursor_x;
    int32_t cursor_y = frame_stack.top().cursor_y;
    
    font_t font  = text_font_stack.top();
    
    DrawGlyph(font, glyph, widget_color_stack.top(), cursor_x, cursor_y);
    
    cursor_x += GlyphWidth(font, glyph);
    
    frame_stack.top().cursor_x = cursor_x;
    frame_stack.top().cursor_y = cursor_y;
}

/// Draws a glyph box.
/// Glyph boxes are set up so that the first glyph, i.e. the one that you pass 
/// in the glyph parameter is the top-left corner. Then the numerically next
/// glyph is take as the top-middle glpyh, then the top-right.
/// This continues going left-to-right, top-to-bottom until all 9 squares are
/// covered.
/// If you are using a default widget layout font, you can use the enum values
/// WIDGET_WINDOW, WIDGET_REVERSE_WINDOW, WIDGET_BUTTON, WIDGET_SELECT_BOX or
/// WIDGET_BORDER for the glyph index.
void DrawBox(font_t font, glyph_t glyph, int32_t x, int32_t y, uint32_t w, uint32_t h) {
    const glyph_t top_lft = glyph + 0;
    const glyph_t top_mid = glyph + 1;
    const glyph_t top_rgt = glyph + 2;
    const glyph_t mid_lft = glyph + 3;
    const glyph_t mid_mid = glyph + 4;
    const glyph_t mid_rgt = glyph + 5;
    const glyph_t btm_lft = glyph + 6;
    const glyph_t btm_mid = glyph + 7;
    const glyph_t btm_rgt = glyph + 8;
    
    font = widget_font_stack.top();
    
    // Draw in the corners
    DrawGlyph(font, top_lft, widget_color_stack.top(), x, y, 0, 0);
    DrawGlyph(font, top_rgt, widget_color_stack.top(), x+w-GlyphWidth(font, top_rgt), y, 0, 0);
    DrawGlyph(font, btm_lft, widget_color_stack.top(),  x, y+h-GlyphHeight(font, btm_lft), 0, 0);
    DrawGlyph(font, btm_rgt, widget_color_stack.top(), x+w-GlyphWidth(font, btm_rgt), y+h-GlyphHeight(font, btm_rgt),  0, 0);
    
    // Draw in the top and bottom sides
    DrawGlyph(font, top_mid, widget_color_stack.top(), x+GlyphWidth(font, top_lft), y, w-GlyphWidth(font, top_rgt)-GlyphWidth(font, top_lft), 0);
    DrawGlyph(font, btm_mid, widget_color_stack.top(), x+GlyphWidth(font, btm_lft), y+h-GlyphHeight(font, btm_mid), w-GlyphWidth(font, btm_rgt)-GlyphWidth(font, btm_lft), 0);
    
    // Draw in the left and right sides
    DrawGlyph(font, mid_lft, widget_color_stack.top(), x, y+GlyphHeight(font, top_rgt), 0, h-GlyphWidth(font, top_lft)-GlyphWidth(font, btm_lft));
    DrawGlyph(font, mid_rgt, widget_color_stack.top(), x+w-GlyphWidth(font, mid_rgt), y+GlyphHeight(font, top_rgt), 0, h-GlyphWidth(font, top_rgt)-GlyphWidth(font, btm_rgt));
    
    // Draw in the middle
    DrawGlyph(font, mid_mid, widget_color_stack.top(), x+GlyphWidth(font, mid_lft), y+GlyphHeight(font, top_mid), w-GlyphWidth(font, mid_lft)-GlyphWidth(font, mid_rgt), h-GlyphHeight(font, top_mid)-GlyphHeight(font, btm_mid));
}

// measures the width of the text, as if the text was being drawn, except it is
// not actually drawn and only the width of the text is returned, and it is
// identical to the widht of the text if it was, in fact, drawn
uint32_t TextWidth(font_t font, const char* text) {
    uint32_t width = 0;
    for (const unsigned char* c = (const unsigned char*)text; *c != '\0'; c++) {
        width += GlyphWidth(font, *c);
    }
    return width;
}

/// Draws text on the screen.
/// @param orientation Either TEXT_LEFT, TEXT_CENTER, TEXT_RIGHT or
///                    TEXT_JUSTIFIED. The default is TEXT_LEFT.
/// @param text        The text that will drawn to screen.
void Text(const char* text, uint32_t orientation) {
    int32_t cursor_x = frame_stack.top().cursor_x;
    int32_t cursor_y = frame_stack.top().cursor_y;
    // depending on alignment we could also choose other cursor_x and cursor_y
    
    font_t font  = text_font_stack.top();
    uint32_t effect = 0;
    if (effect_stack.size()) effect = effect_stack.top();

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
        if (*c=='\n') {
            NewLine(LINE_LOW);
            cursor_x = frame_stack.top().cursor_x;
            cursor_y = frame_stack.top().cursor_y;
            continue;
        }
        DrawGlyph(font, (unsigned char)*c, text_color_stack.top(), cursor_x, cursor_y);
        if (effect & UNDERLINE) {
            DrawGlyph(font, '_', text_color_stack.top(), cursor_x, cursor_y, GlyphWidth(font, (unsigned char)*c) + 2, 0, -1);
        }
        if (effect & SHADOW) {
            DrawGlyph(font, (unsigned char)*c, Render::COLOR_BLACK, cursor_x + 1, cursor_y + 1, 0, 0, -1);
        }
        
        cursor_x += GlyphWidth(font, (unsigned char)*c);
    }
    
    frame_stack.top().cursor_x = cursor_x;
    frame_stack.top().cursor_y = cursor_y;
}

/// Draws a horizontal bar.
/// Similar to how DrawBox() is used, except we only have 3 squares, the left,
/// middle and right.
void DrawBoxHorizontal(font_t font, glyph_t glyph,  int32_t x, int32_t y, uint32_t w) {
    const glyph_t lft = glyph + 0;
    const glyph_t mid = glyph + 1;
    const glyph_t rgt = glyph + 2;
    
    DrawGlyph(font, lft, widget_color_stack.top(), x, y, 0, 0);
    DrawGlyph(font, rgt, widget_color_stack.top(), x+w-GlyphWidth(font, lft)-GlyphWidth(font, rgt), y, 0, 0);
    DrawGlyph(font, mid, widget_color_stack.top(), x+GlyphWidth(font, lft), y, w-GlyphWidth(font, lft)-GlyphWidth(font, rgt), 0);
}

/// Pushes a frame.
/// The coordinates are in absolute screen cooordinates. This function could be
/// useful for something, I just can't think of anything.
void PushFrame(int32_t x, int32_t y, uint32_t w, uint32_t h) {
    uint32_t stack_height = frame_stack.top().stack_height;
    
    FrameObject new_frame;
    new_frame.x = x;
    new_frame.y = y;
    new_frame.w = w;
    new_frame.h = h;
    new_frame.cursor_x = x;
    new_frame.cursor_y = y;
    new_frame.stack_height = stack_height + 2;
    
    frame_stack.push(new_frame);
}

/// Pushes a frame releative to the previous.
/// Check the regulat HTML docs on the website for a graphical description with,
/// i.e. with images, to see how this works.
/// But essentially what this function does, is it takes the previous frame and
/// measures out the offset from the specified orientation and then pushes a new
/// frame with the are from the offset.
/// Initially the first frame is pushed, and it takes up the whole screen. If
/// you wanted to use make the topmost 200px part of the screen a frame, you 
/// would set the orientation to FRAME_TOP and offset to 200.
void PushFrameRelative(uint32_t orientation, uint32_t offset) {
    int32_t x = frame_stack.top().x;
    int32_t y = frame_stack.top().y;
    int32_t w = frame_stack.top().w;
    int32_t h = frame_stack.top().h;
    
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

/// Removes a frame from the frame stack.
void PopFrame() {
    frame_stack.pop();
}

/// Pushes a frame, but keeps the cursor in place.
/// Identical to PushFrameRelative(), except this function will not reset the
/// cursor to the top-left corner of the frame.
void PushFrameRelativeKeepCursor(uint32_t orientation, uint32_t offset, bool keep_x, bool keep_y) {
    int32_t x = frame_stack.top().cursor_x;
    int32_t y = frame_stack.top().cursor_y;
    PushFrameRelative(orientation, offset);
    if (keep_x) frame_stack.top().cursor_x = x;
    if (keep_y) frame_stack.top().cursor_y = y;
}

void PopFrameKeepCursor(bool keep_x, bool keep_y) {
    int32_t x = frame_stack.top().cursor_x;
    int32_t y = frame_stack.top().cursor_y;
    frame_stack.pop();
    if (keep_x) frame_stack.top().cursor_x = x;
    if (keep_y) frame_stack.top().cursor_y = y;
}

bool CursorOver(int32_t x, int32_t y, uint32_t w, uint32_t h) {
    bool is_over = false;
    if (!is_keyboard) {
        int32_t cur_x = UI::PollKeyboardAxis(UI::KEY_MOUSE_X) / scaling;
        int32_t cur_y = UI::PollKeyboardAxis(UI::KEY_MOUSE_Y) / scaling;
        
        is_over = cur_x > x && cur_y > y && cur_x < x + (int32_t)w && cur_y < y + (int32_t)h;
    } else {
        is_over = allow_keyboard && processing_group == selected_group && processing_index == selected_index;
    }
    
    // this will flip `selected_something_now` to true on the frame that a widget
    // is selected, or the selection changes.
    // we use the x and y of the widget as a proxy for identity.
    if (is_over) {
        static uint32_t tick_selected = 0;
        static int32_t x_selected = -1, y_selected = -1;
        
        // ignore overlapping selections
        if (tick_selected == GetTick()) {
            return is_over;
        }
        
        // widget already selected and selection didn't change
        if (tick_selected == GetTick() - 1 && x == x_selected && y == y_selected) {
            selected_something_still = true;
            tick_selected = GetTick();
            
            return is_over;
        }
        
        // selection changed
        selected_something_now = true;
        selected_something_still = true;
        
        x_selected = x;
        y_selected = y;
        
        tick_selected = GetTick();
    }
    
    return is_over;
}

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

// call this to check if user is holding click
bool Clicked() {
    if (is_keyboard) {
        return UI::PollKeyboardKey(UI::KEY_ENTER) || UI::PollKeyboardKey(UI::KEY_CONTROLLER_A);
    } else {
        return UI::PollKeyboardKey(UI::KEY_LEFTMOUSE);
    }
}

/// Draws a button.
/// @param text     Text of the button.
/// @param enabled  If set to false, the button won't be clickable.
/// @param width    Width of the button, in pixels.
/// @return         True if the button was clicked on.
bool Button(const char* text, bool enabled, uint32_t width) {
    int32_t x = frame_stack.top().cursor_x;
    int32_t y = frame_stack.top().cursor_y;
    uint32_t w = width ? width : TextWidth(2, text) + 16;
    uint32_t h = 22;
    
    glyph_t style = WIDGET_BUTTON;
    
    if (!enabled) {
        style = WIDGET_BUTTON_DISABLED;
    } else if (SelectionIndex(-1), CursorOver(x, y, w, h)) {
        if (Clicked()) {
            style = WIDGET_BUTTON_PRESSED;
            activated_something = true;
        } else {
            style = WIDGET_BUTTON_SELECTED_ENABLED;
            highlighted_something = true;
        }
        
        if (!is_keyboard) {
            SetCursorDelayed(UI::CURSOR_CLICK);
        }
    }
    
    DrawBox(0, style, x, y, w, h);
    
    PushFrame(x, y + 3, w, h);
    SetColor(enabled ? Render::COLOR_BLACK : Render::COLOR_GRAY);
    Text(text, TEXT_CENTER);
    PopFrame();
    
    frame_stack.top().cursor_x += w;
    
    RestoreColor();
    
    return enabled && CursorOver(x, y, w, h) && ClickHandledLate();
}

/// Draws a radio button.
/// @param index    Index of this button.
/// @param selected Index of the selected button.
/// @param text     Text next to the button.
/// @param enable   If set to true, the button won't be clickable
/// @return         True if the button was clicked on. The new selected button
///                 index will be written to the selected parameter.
bool RadioButton(uint32_t index, uint32_t& selected, const char* text, bool enabled) {
    int32_t x = frame_stack.top().cursor_x;
    int32_t y = frame_stack.top().cursor_y;
    
    glyph_t style = WIDGET_RADIO_BUTTON;
    
    if (enabled && index == selected) style += 1;
    if (!enabled && index == selected) style += 5;
    if (!enabled && index != selected) style += 4;
    
    DrawGlyph(0, style, widget_color_stack.top(), x, y);
    
    frame_stack.top().cursor_x += GlyphWidth(0, style);
    
    bool underline = false;
    int32_t text_start = frame_stack.top().cursor_x;
    
    if (text) Text(text, TEXT_LEFT);
    
    if (enabled && (SelectionIndex(-1), CursorOver(x, y, frame_stack.top().cursor_x - x, 24))) {
        if (ClickHandledLate()) {
            activated_something = true;
            selected = index;
            return true;
        }
        
        if (is_keyboard) {
            underline = true;
            highlighted_something = true;
        } else {
            SetCursorDelayed(UI::CURSOR_CLICK);
        }
    }
    
    if (underline && text) {
        frame_stack.top().cursor_x = text_start;
        SetEffect(UNDERLINE);
        Text(text, TEXT_LEFT);
        RestoreEffect();
    }
    
    return false;
}

/// Draws a checkbox.
/// @param selected If set to true, the button will be drawn checked.
/// @param text     Text of the button.
/// @param enabled  If set to false, the button won't be clickable.
/// @return         True if clicked on. The modified state of the button will be
///                 written out to the selected parameter.
bool CheckBox(bool& selected, const char* text, bool enabled) {
    int32_t x = frame_stack.top().cursor_x;
    int32_t y = frame_stack.top().cursor_y;
    
    glyph_t style = WIDGET_CHECK_BUTTON;
    
    if (enabled && selected) style += 1;
    if (!enabled && selected) style += 5;
    if (!enabled && !selected) style += 4;
    
    DrawGlyph(0, style, widget_color_stack.top(), x, y);
    
    frame_stack.top().cursor_x += GlyphWidth(0, style);
    
    bool underline = false;
    int32_t text_start = frame_stack.top().cursor_x;
    
    if (text) Text(text, TEXT_LEFT);
    
    if (enabled && (SelectionIndex(-1), CursorOver(x, y, frame_stack.top().cursor_x - x, 24))) {
        if (ClickHandledLate()) {
            activated_something = true;
            selected = !selected;
            return true;
        }
        
        if (is_keyboard) {
            underline = true;
            highlighted_something = true;
        } else {
            SetCursorDelayed(UI::CURSOR_CLICK);
        }
    }
    
    if (underline && text) {
        frame_stack.top().cursor_x = text_start;
        SetEffect(UNDERLINE);
        Text(text, TEXT_LEFT);
        RestoreEffect();
    }
    
    return false;
}

/// Draws a horizontal slider.
/// @param value    Value of the slider, between 0.0f and 1.0f.
/// @param enabled  If set to false, the slider won't be draggable.
/// @param width    Width of the slider, in pixels.
/// @return         True if the slider was clicked on. The slider's new value
///                 will be written out to the value variable.
bool Slider(float& value, bool enabled, uint32_t width) {
    int32_t x = frame_stack.top().cursor_x;
    int32_t y = frame_stack.top().cursor_y;
    uint32_t w = width ? width : 100;
    uint32_t h = 22;
    
    glyph_t style = WIDGET_SLIDER_HORIZONTAL;
    
    if (!enabled) {
        style += 4;
    } else if (SelectionIndex(-1), CursorOver(x, y, w, h)) {
        if (Clicked()) {
            style += 3;
            activated_something = true;
        } else {
            style += 2;
            highlighted_something = true;
        }
        
        if (!is_keyboard) {
            SetCursorDelayed(UI::CURSOR_CLICK);
        }
    }
    
    DrawBoxHorizontal(0, WIDGET_SLIDER_TRACK_HORIZONTAL, x, y + 8, w);
    
    PushFrame(x, y + 3, w, h);
    DrawGlyph(0, style, widget_color_stack.top(), x + (uint32_t)(value * w) - (GlyphWidth(0, style) / 2), y);
    PopFrame();
    
    frame_stack.top().cursor_x += w;
    
    if (enabled && !is_keyboard && CursorOver(x, y, w, h) && Clicked()) {
        uint32_t cur_x = UI::PollKeyboardAxis(UI::KEY_MOUSE_X) / scaling;
        uint32_t progress = cur_x - x;
        value = (float)progress / (float)w;
        return true;
    } else if (enabled && is_keyboard && CursorOver(x, y, w, h)) {
        float delta = 0.0f;
        if (IsKey(UI::KEY_LEFT) || IsKey(UI::KEY_CONTROLLER_DPAD_LEFT)) {
            delta -= 10.0f / (float)w;
        }
        if (IsKey(UI::KEY_RIGHT) || IsKey(UI::KEY_CONTROLLER_DPAD_RIGHT)) {
            delta += 10.0f / (float)w;
        }
        value += delta;
        if (value < 0.0f) value = 0.0f;
        if (value > 1.0f) value = 1.0f;
        return delta != 0.0f;
    } else {
        return false;
    }
}

/// Draws a new line.
/// Essentially just moves the cursor down a little bit and resets it to the
/// left side of the frame. Sort of like a line break.
/// @param line Line type. Either LINE_LOW, LINE_NORMAL or LINE_HIGH.
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

/// Draws a horizontal divider.
void HorizontalDivider() {
    NewLine();
    int32_t x = frame_stack.top().cursor_x;
    int32_t y = frame_stack.top().cursor_y;
    uint32_t w = frame_stack.top().cursor_x - frame_stack.top().x + frame_stack.top().w;
    DrawBoxHorizontal(0, WIDGET_DIVIDER_HORIZONTAL, x, y, w);
    frame_stack.top().cursor_x = frame_stack.top().x;
    frame_stack.top().cursor_y += GlyphHeight(0, WIDGET_DIVIDER_HORIZONTAL) + 4;
}

/// Fills the frame with a glyph.
/// Uses the DrawBox() function internally, so it works just like it, except
/// that it uses the topmost frame to determine the position and sixze of the
/// box.
void FillFrame(glyph_t glyph) {
    DrawBox(widget_font_stack.top(), glyph, frame_stack.top().x, 
                                     frame_stack.top().y, 
                                     frame_stack.top().w, 
                                     frame_stack.top().h);
    frame_stack.top().stack_height++;
}

/// Draws a textbox.
/// @param text     Text in the textbox. This will be editable.
/// @param length   Length of the text buffer.
/// @param enabled  If false, the textbox won't be editable.
/// @param w,h      Dimensions of the textbox, in pixels.
bool TextBox(char* text, uint32_t length, bool enabled, uint32_t w, uint32_t h) {
    int32_t x = frame_stack.top().cursor_x;
    int32_t y = frame_stack.top().cursor_y;
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
            activated_something = true;
        }
        
        SetCursorDelayed(UI::CURSOR_TEXT);
    }
    
    DrawBox(0, style, x, y, w, h);
    
    PushFrame(x + 4, y + 3, w - 8, h);
    Text(text, TEXT_LEFT);
    if (allow_typing && (GetTick() & 16)) Text("|", TEXT_LEFT);
    PopFrame();
    
    frame_stack.top().cursor_x += w;
    
    return text_changed;
}

/// Draws a textbox.
/// Essentially identical to the other TextBox() function, except this one
/// accepts a `const char*` text. This also means that the text in the textbox
/// won't be editable and it will be drawn as disabled.
void TextBox(const char* text, uint32_t w, uint32_t h) {
    int32_t x = frame_stack.top().cursor_x;
    int32_t y = frame_stack.top().cursor_y;
    if (w == 0) w = 100;
    if (h == 0) h = 22;

    if (CursorOver(x, y, w, h)) {
        SetCursorDelayed(UI::CURSOR_TEXT);
    }
    
    DrawBox(0, WIDGET_TEXT_BOX_DISABLED, x, y, w, h);
    
    PushFrame(x + 4, y + 3, w - 8, h);
    Text(text, TEXT_LEFT);
    PopFrame();
    
    frame_stack.top().cursor_x += w;
}

/// Sets the selection group of the following widgets.
/// The index can be an absolute value, or it can be set to `-1` to increment
/// the existing value. The indices should begin at zero and have no gaps
/// between them.
/// Additionally, calling this function also resets the selection index to zero.
/// If interlacing widgets of different groups, make sure to restore the
/// previous selection indices by calling the `SelectionIndex()` function.
/// After calling `GUI::Begin()`, the selection group is automatically reset to
/// zero for all of that frame's widgets.
void SelectionGroup(int32_t index) {
    if (index == processing_group) {
        return;
    }
    
    if (index < 0) {
        processing_group++;
    } else {
        processing_group = index;
    }
    
    if (processing_group >= selection_group_last_index) {
        selection_group_last_index = processing_group;
    }
    
    processing_index = -1;
}

/// Sets the selection index of the following widget.
/// The indices for each group should begin at zero and have no gaps between
/// them. The selection index is automatically incremented for each widget, e.g.
/// if you set a widget to have an index of 5, the following widgets two will
/// have the indices of 6 and 7 respectively.
void SelectionIndex(int32_t index) {
    if (index < 0 && allow_keyboard) {
        processing_index++;
    } else {
        processing_index = index;
    }
    
    if (processing_group == selected_group) {
        if (processing_index >= selected_group_last_index) {
            selected_group_last_index = processing_index;
        }
    }
}

/// Changes selected tab.
/// The `selected` index is handled in the same way as for radio buttons.
/// @param selected  Index that tracks the selected tab.
/// @param tab_count Total number of tabs.
void TabSelection(uint32_t& selected, uint32_t tab_count) {
    if (switch_tab_absolute) {
        selected = switch_tab_absolute - 1;
        ResetSelection();
    }
    
    selected += switch_tab_direction;
    
    if (selected < 0) selected = tab_count - 1, ResetSelection();
    if (selected >= tab_count) selected = 0, ResetSelection();
}

/// Resets the keyboard/controller selection.
void ResetSelection() {
    selected_group = 0;
    selected_index = 0;
}

/// Toggles keyboard/controller selection.
void AllowKeyboard(bool allow) {
    allow_keyboard = allow;
}

/// Begins the GUI commands for the frame.
/// Pushes the first frame to the frame stack. This function has to be called
/// first, before the End() and the Update() functions.
/// You can call Begin() as much as you want, you just have to call an End()
/// before you can call Begin() again.
/// This should help dealing a bit with the global state that the system has.
void Begin() {
    if (beginned) {
        Log(Severity::CRITICAL_ERROR, System::GUI, "Called GUI::Begin() again without first calling GUI::End()");
    }
    
    beginned = true;
    
    // The first frame takes up the whole screen.
    // After the first frame is created, the user will be able to split it into
    // smaller frames and use them to position the widgets on the screen.
    FrameObject first_frame;
    first_frame.x = 0;
    first_frame.y = 0;
    first_frame.w = UI::GetScreenWidth() / scaling;
    first_frame.h = UI::GetScreenHeight() / scaling;
    first_frame.cursor_x = 0;
    first_frame.cursor_y = 0;
    first_frame.stack_height = 0;
    
    frame_stack.push(first_frame);
    
    // pushing in the default colors
    widget_color_stack.push(default_widget_color);
    text_color_stack.push(default_text_color);
    text_font_stack.push(default_text_font);
    widget_font_stack.push(default_widget_font);
}

/// Ends the GUI commands for the frame.
/// This has to be called sometime after Begin(), but before Update().
void End() {
    if (!beginned) {
        Log(Severity::CRITICAL_ERROR, System::GUI, "Called GUI::End() without first calling GUI::Begin()");
    }
    
    beginned = false;
    
    if (frame_stack.size() < 1) {
        Log(Severity::WARNING, System::GUI, "No frames in the framestack, called GUI::PopFrame() too much");
    }
    
    if (frame_stack.size() > 1) {
        Log(Severity::WARNING, System::GUI, "Thera are {} too many frames in the framestack", frame_stack.size() - 1);
    }
    
    frame_stack.reset();
    widget_color_stack.reset();
    text_color_stack.reset();
    text_font_stack.reset();
    widget_font_stack.reset();
}

}