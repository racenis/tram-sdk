// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <render/render.h>
#include <render/renderer.h>

namespace Core::Render {
    void AddLineMarker(const glm::vec3& location, const glm::vec3& color){
        AddLine(location + glm::vec3(-1.0f, 0.0f, 0.0f), location + glm::vec3(1.0f, 0.0f, 0.0f), color);
        AddLine(location + glm::vec3(0.0f, -1.0f, 0.0f), location + glm::vec3(0.0f, 1.0f, 0.0f), color);
        AddLine(location + glm::vec3(0.0f, 0.0f, -1.0f), location + glm::vec3(0.0f, 0.0f, 1.0f), color);
    }

    // the implementation of this function is extremely inefficient
    // ignore that, i'll eventually make a better function
    void SetSun(float timeOfDay){
        struct sunparam {
            float time;
            glm::vec3 sundir;
            glm::vec3 suncolor;
            glm::vec3 ambientcolor;
        };

        // glm::vec3(0.0f, 1.0f, 0.0f)  <- sun directly above
        // glm::vec3(0.0f, 0.0f, 1.0f)  <- sun directly west
        // glm::vec3(0.0f, 0.0f, -1.0f) <- sun directly east
        // glm::vec3(1.0f, 0.0f, 0.0f)  <- sun directly south
        // glm::vec3(-1.0f, 0.0f, 0.0f) <- sun directly north

        sunparam suns[8];

        // midnight
        suns[0].time = 0.0f;
        suns[0].sundir = glm::vec3(0.0f, 1.0f, 0.0f);
        suns[0].suncolor = glm::vec3(230.0f*0.7f, 246.0f*0.7f, 255.0f*0.7f);
        suns[0].ambientcolor = suns[0].suncolor * 0.2f;

        // 5.00, just before dawn
        suns[1].time = 0.20833f;
        suns[1].sundir = glm::vec3(-0.3f, 1.0f, 0.0f);
        suns[1].suncolor = glm::vec3(202.0f, 216.0f, 224.0f);
        suns[1].ambientcolor = suns[1].suncolor * 0.2f;

        // 6.00, dawn
        suns[2].time = 0.25f;
        suns[2].sundir = glm::vec3(0.0f, 0.0f, -1.0f);
        suns[2].suncolor = glm::vec3(255.0f, 214.0f, 163.0f);
        suns[2].ambientcolor = suns[2].suncolor * 0.2f;

        // midday
        suns[3].time = 0.5f;
        suns[3].sundir = glm::vec3(0.0f, 1.0f, 0.0f);
        suns[3].suncolor = glm::vec3(255.0f, 255.0f, 255.0f);
        suns[3].ambientcolor = suns[3].suncolor * 0.2f;

        // 17.00 evening
        suns[4].time = 0.70833f;
        suns[4].sundir = glm::vec3(0.0f, 1.0f, 0.5f);
        suns[4].suncolor = glm::vec3(255.0f, 237.0f, 184.0f);
        suns[4].ambientcolor = suns[4].suncolor * 0.2f;

        // 22.00 sunset
        suns[5].time = 0.91666;
        suns[5].sundir = glm::vec3(0.0f, 0.0f, 1.0f);
        suns[5].suncolor = glm::vec3(250.0f, 214.0f, 165.0f);
        suns[5].ambientcolor = suns[5].suncolor * 0.2f;

        // 23.00 darkness
        suns[6].time = 0.95833f;
        suns[6].sundir = glm::vec3(0.0f, 1.0f, 0.3f);
        suns[6].suncolor = glm::vec3(230.0f*0.5f, 246.0f*0.5f, 255.0f*0.5f);
        suns[6].ambientcolor = suns[6].suncolor * 0.2f;

        // midnight, again
        suns[7].time = 1.0f;
        suns[7].sundir = suns[0].sundir;
        suns[7].suncolor = suns[0].suncolor;
        suns[7].ambientcolor = suns[0].ambientcolor;

        timeOfDay = fmod(timeOfDay, 1.0f);

        size_t t = 0;
        for (size_t i = 0; i < 7; i++){
            if(suns[i].time > timeOfDay) break;
            t = i;
        }

        size_t nt = (t + 1) % 8;

        float mixf = (timeOfDay - suns[t].time) / (suns[nt].time - suns[t].time);


        SUN_DIRECTION = glm::normalize(glm::mix(suns[t].sundir, suns[nt].sundir, mixf));
        SUN_COLOR = glm::mix(suns[t].suncolor, suns[nt].suncolor, mixf) / 256.0f*1.2f;
        AMBIENT_COLOR = glm::mix(suns[t].ambientcolor, suns[nt].ambientcolor, mixf) / 256.0f*1.2f;

        //Render::modelMatrices.sunDirection = glm::vec4(sundir.x, sundir.y, sundir.z, 1.0f);
        //Render::modelMatrices.sunColor = glm::vec4(suncolor.x, suncolor.y, suncolor.z, 1.0f);
        //Render::modelMatrices.ambientColor = glm::vec4(ambientcolor.x, ambientcolor.y, ambientcolor.z, 1.0f);
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
        SpriteVertex tleft;   // top left
        SpriteVertex tright;  // top right
        SpriteVertex bleft;   // bottom left
        SpriteVertex bright;  // bottom right

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
        tleft.voffset = scale;
        tleft.texture = texture;
        tleft.verticality = thickness;
        tright.color = color;
        tright.voffset = scale;
        tright.texture = texture;
        tright.verticality = thickness;
        bleft.color = color;
        bleft.voffset = scale;
        bleft.texture = texture;
        bleft.verticality = thickness;
        bright.color = color;
        bright.voffset = scale;
        bright.texture = texture;
        bright.verticality = thickness;

        textvertices.push_back(bleft);
        textvertices.push_back(bright);
        textvertices.push_back(tleft);
        textvertices.push_back(bright);
        textvertices.push_back(tright);
        textvertices.push_back(tleft);
    }
    
    void SetGlyph(const float& x, const float& y, const float& w, const float& h, const float& tex_x, const float& tex_y, const float& tex_w, const float& tex_h, const glm::vec3& color, const uint32_t& tex){
        SpriteVertex tleft;   // top left
        SpriteVertex tright;  // top right
        SpriteVertex bleft;   // bottom left
        SpriteVertex bright;  // bottom right

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
