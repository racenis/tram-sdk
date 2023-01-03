// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// CORE.CPP -- Core functions and globals.

#include <framework/core.h>
#include <render/render.h>
#include <render/armature.h>

#include <framework/ui.h>

#include <cstring>
#include <fstream>

namespace Core {
    char const* ENGINE_VERSION = "Tramway SDK v0.0.4-alpha";
    
    bool SHOULD_CLOSE = false;
    float FRAME_TIME = 0;


    // this one should actually go into the ?? uhh ??
    // the extension. right.
    // WTF is the extension???
    // oh, right..
    bool DRAW_PHYSICS_DEBUG = false;
    bool DRAW_RENDER_DEBUG = false;
    bool DRAW_PATH_DEBUG = false;
    
    size_t RESOURCE_VRAM_USAGE = 0;
    
    // TODO: these should be moved into their own file
    void QuatLookAt(glm::quat& quaternion, const glm::vec3& from, const glm::vec3& to){
        const glm::vec3 start = normalize(from);
        const glm::vec3 desu = normalize(to);

        const glm::vec3 axis = glm::cross(start, desu);

        const float square = sqrt((1.0f + glm::dot(start, desu)) * 2.0f);
        const float inverse = 1 / square;

        quaternion.x = axis.x * inverse;
        quaternion.y = axis.y * inverse;
        quaternion.z = axis.z * inverse;
        quaternion.w = square * 0.5f;
    }

    void ProjectLine(glm::vec3& point, const glm::vec3& from, const glm::vec3& to){
        const glm::vec3 projectable = point - from;
        const glm::vec3 line = to - from;
        point = from + (glm::dot(projectable, line) / glm::dot(line, line) * line);
    }

    void Init(){
        // set the 0th string to 'none'
        UID none("none");
        
        
        // these are all of the default systems, as of now.
        // feel free to extend the list, but do note that this list
        // is in the same order as the System::System enumeration.
        System::Register("Core", "CORE");
        System::Register("User Interface", "UI");
        System::Register("Async", "ASYNC");
        System::Register("Rendering", "RENDER");
        System::Register("Physics", "PHYSICS");
        System::Register("Audio", "AUDIO");
        System::Register("Misc", "MISC");
        
        // this will register all of the default events
        for (event_t i = 0; i < Event::LAST_EVENT; i++) {
            Event::Register();
        }
    }




}


