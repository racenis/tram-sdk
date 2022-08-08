// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// RENDER.CPP -- Rendering stuff.

#include <core.h>
#include <render.h>

#include <render_opengl.cpp>

using namespace Core;

namespace Core::Render {
    glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat CAMERA_ROTATION = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    float CAMERA_PITCH = 0.0f;
    float CAMERA_YAW = -90.0f;

    float SCREEN_WIDTH = 800.0f;
    float SCREEN_HEIGHT = 600.0f;

    bool THIRD_PERSON = false;
    
    glm::vec3 SUN_DIRECTION = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 SUN_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 AMBIENT_COLOR = glm::vec3(0.0f, 0.0f, 0.0f);

    Material FONT_REGULAR;
    Material FONT_TITLE;
    Material FONT_SYMBOLS;
    Material GLYPH_GUI;
    Material GLYPH_TEXT;

    bool DRAW_PHYSICS_DEBUG = false;
    bool DRAW_PATH_DEBUG = false;

    float FRAME_LIMIT = 60.0f;

    float time_of_day = 0.8f;

    

    void Init(){
        Init_OpenGL();
        
        FONT_REGULAR = Material(UID("jost"), Material::TEXTURE_MSDF);  // futura knock-off
        FONT_TITLE = Material(UID("inter"), Material::TEXTURE_MSDF);    // helvetica
        FONT_SYMBOLS = Material(UID("symbols"), Material::TEXTURE_MSDF);  // ornamentation and other non-text glyphs
        GLYPH_GUI = Material(UID("glyph_symbols"), Material::TEXTURE_GLYPH);  // ornamentation and other non-text glyphs
        GLYPH_TEXT = Material(UID("glyph_text"), Material::TEXTURE_GLYPH);  // ornamentation and other non-text glyphs

        FONT_REGULAR.Load();
        FONT_TITLE.Load();
        FONT_SYMBOLS.Load();
        GLYPH_GUI.Load();
        GLYPH_TEXT.Load();
    }

    void Render(){
        Render_OpenGL();
    }
    
    void AddLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color){
        LineVertex vert;

        vert.co[0] = from[0];
        vert.co[1] = from[1];
        vert.co[2] = from[2];
        vert.color[0] = color[0];
        vert.color[1] = color[1];
        vert.color[2] = color[2];

        colorlines.push_back(vert);

        vert.co[0] = to[0];
        vert.co[1] = to[1];
        vert.co[2] = to[2];
        vert.color[0] = color[0];
        vert.color[1] = color[1];
        vert.color[2] = color[2];

        colorlines.push_back(vert);
    }
    
    void SetGlyph(float x, float y, float tex_x, float tex_y, float width, float height, float scale_x, float scale_y, float thickness, uint32_t texture, const glm::vec3& color){
        TextVertex tleft;   // top left
        TextVertex tright;  // top right
        TextVertex bleft;   // bottom left
        TextVertex bright;  // bottom right

        thickness = thickness - 1.0f;

        glm::vec2 scale;
        scale.x = scale_x * 2;
        scale.y = scale_y * 2;

        tleft.co.x = x;
        tleft.co.y = y;
        tleft.texco.x = tex_x;
        tleft.texco.y = tex_y;

        tright.co.x = x + width * scale_x;
        tright.co.y = y;
        tright.texco.x = tex_x + width;
        tright.texco.y = tex_y;

        bleft.co.x = x;
        bleft.co.y = y + height * scale_y;
        bleft.texco.x = tex_x;
        bleft.texco.y = tex_y + height;

        bright.co.x = x + width * scale_x;
        bright.co.y = y + height * scale_y;
        bright.texco.x = tex_x + width;
        bright.texco.y = tex_y + height;

        tleft.color = color;
        tleft.scale = scale;
        tleft.texture = texture;
        tleft.thickness = thickness;
        tright.color = color;
        tright.scale = scale;
        tright.texture = texture;
        tright.thickness = thickness;
        bleft.color = color;
        bleft.scale = scale;
        bleft.texture = texture;
        bleft.thickness = thickness;
        bright.color = color;
        bright.scale = scale;
        bright.texture = texture;
        bright.thickness = thickness;

        textvertices.push_back(bleft);
        textvertices.push_back(bright);
        textvertices.push_back(tleft);
        textvertices.push_back(bright);
        textvertices.push_back(tright);
        textvertices.push_back(tleft);
    }
    
    void SetGlyph(const float& x, const float& y, const float& w, const float& h, const float& tex_x, const float& tex_y, const float& tex_w, const float& tex_h, const glm::vec3& color, const uint32_t& tex){
        GlyphVertex tleft;   // top left
        GlyphVertex tright;  // top right
        GlyphVertex bleft;   // bottom left
        GlyphVertex bright;  // bottom right

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
        tleft.texture = tex;
        tright.color = color;
        tright.texture = tex;
        bleft.color = color;
        bleft.texture = tex;
        bright.color = color;
        bright.texture = tex;

        glyphvertices.push_back(bleft);
        glyphvertices.push_back(bright);
        glyphvertices.push_back(tleft);
        glyphvertices.push_back(bright);
        glyphvertices.push_back(tright);
        glyphvertices.push_back(tleft);
    }

}


namespace Core {


}
