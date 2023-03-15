// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_H
#define RENDER_H

#include <vector>

#include <render/armature.h>

namespace tram::Render {
    class Model;

    extern bool DRAW_RENDER_DEBUG;

    extern vec3 CAMERA_POSITION;
    extern quat CAMERA_ROTATION;
    
    extern vec3 SUN_DIRECTION;
    extern vec3 SUN_COLOR;
    extern vec3 AMBIENT_COLOR;

    extern float SCREEN_WIDTH;
    extern float SCREEN_HEIGHT;

    extern bool THIRD_PERSON;

    // these are for the OpenGL renderer only.
    // when more renderers will be added, we'll
    // probably switch these from int to void*
    typedef uint32_t vertexhandle_t;
    typedef uint32_t texturehandle_t;
    struct DrawListEntryHandle {
        void* draw_list_entries[6] = { nullptr };
    };

    const uint32_t MAX_MATERIALS_PER_MODEL = 15;
    
    const vec3 COLOR_WHITE  (1.0f, 1.0f, 1.0f);
    const vec3 COLOR_RED    (1.0f, 0.0f, 0.0f);
    const vec3 COLOR_GREEN  (0.0f, 1.0f, 0.0f);
    const vec3 COLOR_BLUE   (0.0f, 0.0f, 1.0f);
    const vec3 COLOR_YELLOW (1.0f, 1.0f, 0.0f);
    const vec3 COLOR_PINK   (1.0f, 0.0f, 1.0f);
    const vec3 COLOR_CYAN   (0.0f, 1.0f, 1.0f);
    const vec3 COLOR_BLACK  (0.0f, 0.0f, 0.0f);

    const vec3 COLOR_GRAY   (0.3f, 0.3f, 0.3f);

    extern float FRAME_LIMIT;

    void Init();
    void Render();
    void ScreenSize(float width, float height);
    //void SetScreenClear(vec3 clear_color, bool clear);
    void Project(const glm::vec3& point, glm::vec3& result);

    void SetSun(float timeOfDay);

    void AddLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color);
    void AddLineMarker(const glm::vec3& location, const glm::vec3& color);
}

#endif // RENDER_H
