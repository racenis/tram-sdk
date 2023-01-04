// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// CORE.H -- Engine core definitons
// Basic engine datatypes, templates, useful functions.

#ifndef TRAM_CORE_H
#define TRAM_CORE_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

//#include <framework/entity.h>
//#include <framework/entitycomponent.h>
//#include <framework/etc.h>
//#include <framework/event.h>
//#include <framework/message.h>
//#include <framework/resource.h>
//#include <framework/stats.h>
//#include <framework/system.h>
//#include <framework/uid.h>
//#include <framework/worldcell.h>

namespace Core {
    extern bool SHOULD_CLOSE;           // TODO: rename
    extern float FRAME_TIME;            // TODO: rename
    
    // TODO: move these into an extension or something
    extern bool DRAW_PHYSICS_DEBUG;
    extern bool DRAW_RENDER_DEBUG;
    extern bool DRAW_PATH_DEBUG;
    
    // TODO: move this into stats
    extern size_t RESOURCE_VRAM_USAGE;
    
    extern char const* ENGINE_VERSION;

    // TODO: check which ones can be yeeted
    class WorldCell;
    class EntityComponent;
    class SerializedEntityData;
    struct Path;
    struct Navmesh;
    
    void Init();


    // TODO: move these into math
    const glm::vec3 DIRECTION_FORWARD = glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 DIRECTION_SIDE = glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 DIRECTION_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    
    void QuatLookAt(glm::quat& quaternion, const glm::vec3& from, const glm::vec3& to);
    void ProjectLine(glm::vec3& point, const glm::vec3& from, const glm::vec3& to);

    
    // TODO: move these into gui or something
    namespace UI {
        void SetText(const char* text, uint32_t x, uint32_t y, float scale, float width, bool justify, bool stretch, uint32_t font, const glm::vec3& color);
        void SetDebugText(const char* text, const glm::vec3& location, const glm::vec3& color);
    }
}

#endif
