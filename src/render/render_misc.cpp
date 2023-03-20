// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <render/render.h>
#include <render/renderer.h>
#include <render/vertices.h>

namespace tram::Render {
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


        SetSunDirection(glm::normalize(glm::mix(suns[t].sundir, suns[nt].sundir, mixf)));
        SetSunColor(glm::mix(suns[t].suncolor, suns[nt].suncolor, mixf) / 256.0f*1.2f);
        SetAmbientColor(glm::mix(suns[t].ambientcolor, suns[nt].ambientcolor, mixf) / 256.0f*1.2f);
    }
    
    // TODO: make this function less poopy
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
}
