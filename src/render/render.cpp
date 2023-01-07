// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <framework/system.h>
#include <render/render.h>
#include <render/renderer.h>
#include <components/rendercomponent.h>
#include <components/spritecomponent.h>
#include <components/particlecomponent.h>

#include <render/opengl/renderer.h>

using namespace Core;

namespace Core::Render {
    glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat CAMERA_ROTATION = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    float SCREEN_WIDTH = 800.0f;
    float SCREEN_HEIGHT = 600.0f;

    bool THIRD_PERSON = false;
    bool DRAW_RENDER_DEBUG = false;
    
    glm::vec3 SUN_DIRECTION = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 SUN_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 AMBIENT_COLOR = glm::vec3(0.0f, 0.0f, 0.0f);

    // move these into gui.cpp?
    Material FONT_REGULAR;
    Material FONT_TITLE;
    Material FONT_SYMBOLS;
    Material GLYPH_GUI;
    Material GLYPH_TEXT;
    Material GLYPH_TEXT_BOLD;
    Material GLYPH_HEADERS;

    float FRAME_LIMIT = 60.0f;

    Pool<LightListObject> lightPool("lightpool", 100, true);
    Octree<uint32_t> lightTree;


    uint32_t colorlines_vertex_array = 0;
    uint32_t colorlines_vertex_buffer = 0;
    DrawListEntryHandle colorlines_entry;
    
    uint32_t textvertices_vertex_array = 0;
    uint32_t textvertices_vertex_buffer = 0;
    DrawListEntryHandle textvertices_entry;
    
    uint32_t glyphvertices_vertex_array = 0;
    uint32_t glyphvertices_vertex_buffer = 0;
    DrawListEntryHandle glyphvertices_entry;
    
    

    std::vector<LineVertex> colorlines;
    std::vector<SpriteVertex> textvertices;
    std::vector<SpriteVertex> glyphvertices;
    
    void Init(){
        assert(System::IsInitialized(System::SYSTEM_CORE));
        assert(System::IsInitialized(System::SYSTEM_UI));
        
        // since we only have the OpenGL renderer, we'll init that one
        OpenGL::Init();
        
        FONT_REGULAR = Material(UID("jost"), Material::TEXTURE_MSDF);  // futura knock-off
        FONT_TITLE = Material(UID("inter"), Material::TEXTURE_MSDF);    // helvetica
        FONT_SYMBOLS = Material(UID("symbols"), Material::TEXTURE_MSDF);  // ornamentation and other non-text glyphs
        GLYPH_GUI = Material(UID("glyph_symbols"), Material::TEXTURE_GLYPH);  // ornamentation and other non-text glyphs
        GLYPH_TEXT = Material(UID("glyph_text"), Material::TEXTURE_GLYPH);  // ornamentation and other non-text glyphs
        GLYPH_TEXT_BOLD = Material(UID("glyph_text_bold"), Material::TEXTURE_GLYPH);  // ornamentation and other non-text glyphs
        GLYPH_HEADERS = Material(UID("glyph_headers"), Material::TEXTURE_GLYPH);  // ornamentation and other non-text glyphs

        FONT_REGULAR.Load();
        FONT_TITLE.Load();
        FONT_SYMBOLS.Load();
        GLYPH_GUI.Load();
        GLYPH_TEXT.Load();
        GLYPH_TEXT_BOLD.Load();
        GLYPH_HEADERS.Load();
        
        // this is pain in my assholes
        
        CreateVertexArray (LINE_VERTEX_DEFINITION, colorlines_vertex_buffer, colorlines_vertex_array);
        colorlines_entry = InsertDrawListEntry ();
        SetDrawListVertexArray(colorlines_entry, colorlines_vertex_array);
        SetDrawListShader(colorlines_entry, Model::LINE_VERTEX, Material::FLAT_COLOR);
        SetFlags(colorlines_entry, FLAG_RENDER | FLAG_NO_DEPTH_TEST | FLAG_DRAW_LINES);
        
        CreateVertexArray(SPRITE_VERTEX_DEFINITION, textvertices_vertex_buffer, textvertices_vertex_array);
        textvertices_entry = InsertDrawListEntry();
        SetDrawListVertexArray(textvertices_entry, textvertices_vertex_array);
        SetDrawListShader(textvertices_entry, Model::SPRITE_VERTEX, Material::TEXTURE_MSDF);
        SetFlags(textvertices_entry, FLAG_RENDER | FLAG_NO_DEPTH_TEST);
        uint32_t textvertices_textures[5] = {FONT_REGULAR.GetTexture(), FONT_TITLE.GetTexture(), FONT_TITLE.GetTexture(), FONT_TITLE.GetTexture(), FONT_SYMBOLS.GetTexture()};
        SetDrawListTextures(textvertices_entry, 5, textvertices_textures);
        
        CreateVertexArray(SPRITE_VERTEX_DEFINITION, glyphvertices_vertex_buffer, glyphvertices_vertex_array);
        glyphvertices_entry = InsertDrawListEntry();
        SetDrawListVertexArray(glyphvertices_entry, glyphvertices_vertex_array);
        SetDrawListShader(glyphvertices_entry, Model::SPRITE_VERTEX, Material::TEXTURE_GLYPH);
        SetFlags(glyphvertices_entry, FLAG_RENDER | FLAG_NO_DEPTH_TEST);
        uint32_t glyphvertices_textures[4] = {GLYPH_GUI.GetTexture(), GLYPH_TEXT.GetTexture(), GLYPH_TEXT_BOLD.GetTexture(), GLYPH_HEADERS.GetTexture()};
        SetDrawListTextures(glyphvertices_entry, 4, glyphvertices_textures);
        
        System::SetInitialized(System::SYSTEM_RENDER, true);
    }

    void Render(){
        #ifndef ENGINE_EDITOR_MODE
        for (auto& it : PoolProxy<SpriteComponent>::GetPool()) it.Update();
        for (auto& it : PoolProxy<ParticleComponent>::GetPool()) it.Update();
        //GeometryBatch::Update();
        #endif // ENGINE_EDITOR_MODE
        
        UpdateVertexArray(colorlines_vertex_buffer, colorlines.size() * sizeof(LineVertex), &colorlines[0]);
        SetDrawListElements(colorlines_entry, 0, colorlines.size());
        colorlines.clear();
        
        UpdateVertexArray(textvertices_vertex_buffer, textvertices.size() * sizeof(SpriteVertex), &textvertices[0]);
        SetDrawListElements(textvertices_entry, 0, textvertices.size());
        textvertices.clear();
        
        UpdateVertexArray(glyphvertices_vertex_buffer, glyphvertices.size() * sizeof(SpriteVertex), &glyphvertices[0]);
        SetDrawListElements(glyphvertices_entry, 0, glyphvertices.size());
        glyphvertices.clear();
        
        RenderFrame();
    }
    
    void ScreenSize(float width, float height) {
        SCREEN_WIDTH = width;
        SCREEN_HEIGHT = height;
        SetScreenSize(width, height);
    }
}



