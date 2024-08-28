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
    uint32_t x, y, w, h;
    uint32_t cursor_x, cursor_y;
};

Stack<FrameObject> frame_stack("GUI Frame stack", 100);

Render::Sprite* fonts [16] = {nullptr};

Render::vertexarray_t glyphvertices_vertex_array = {.generic = 0};
Render::drawlistentry_t glyphvertices_entry;

std::vector<Render::SpriteVertex> glyphvertices;

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


void DrawGlyph(font_t font, glyph_t glyph, uint32_t x, uint32_t y, uint32_t w = 0, uint32_t h = 0) {
    const auto& info = fonts[font]->GetFrames()[glyph];
    
    if (!w) w = info.width;
    if (!h) h = info.height;
    
    SetGlyph(x, y, 0, w, h, info.offset_x, info.offset_y, info.width, info.height, Render::COLOR_WHITE, font);
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


void Begin() {
    
    // The first frame takes up the whole screen.
    // After the first frame is created, the user will be able to split it into
    // smaller frames and use them to position the widgets on the screen.
    FrameObject* first_frame = frame_stack.AddNew();
    
    first_frame->x = 0;
    first_frame->y = 0;
    first_frame->w = UI::GetScreenWidth();
    first_frame->h = UI::GetScreenWidth();
    first_frame->cursor_x = 0;
    first_frame->cursor_y = 0;
    
    SetGlyph(0, 0, 0, 256, 256, 0, 0, 256, 256, Render::COLOR_WHITE, 0);
    
    DrawBox(0, WIDGET_TEXT_BOX, 300, 300, 100, 100);
    DrawBox(0, WIDGET_TEXT_BOX_DISABLED, 430, 300, 100, 20);
    DrawBox(0, WIDGET_BORDER, 600, 100, 30, 120);
}

void End() {
    frame_stack.Reset();
}


}