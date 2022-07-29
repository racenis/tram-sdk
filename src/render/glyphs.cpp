// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// GLYPHS.CPP -- rendering glyph assembly.

#include <core.h>
#include <render.h>

namespace Core::Render {
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

    void AddLineMarker(const glm::vec3& location, const glm::vec3& color){
        AddLine(location + glm::vec3(-1.0f, 0.0f, 0.0f), location + glm::vec3(1.0f, 0.0f, 0.0f), color);
        AddLine(location + glm::vec3(0.0f, -1.0f, 0.0f), location + glm::vec3(0.0f, 1.0f, 0.0f), color);
        AddLine(location + glm::vec3(0.0f, 0.0f, -1.0f), location + glm::vec3(0.0f, 0.0f, 1.0f), color);
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


}
