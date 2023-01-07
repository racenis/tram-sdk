// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_MATH_H
#define FRAMEWORK_MATH_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Core {
    typedef glm::vec3 vec3;
    typedef glm::quat quat;
    typedef glm::mat4 mat4;
    typedef glm::vec4 vec4;
    typedef glm::vec2 vec2;
    typedef glm::ivec3 ivec3;
    typedef glm::ivec4 ivec4;
    
    const vec3 DIRECTION_FORWARD    (0.0f,  0.0f, -1.0f);
    const vec3 DIRECTION_SIDE       (1.0f,  0.0f,  0.0f);
    const vec3 DIRECTION_UP         (0.0f,  1.0f,  0.0f);
    
    void QuatLookAt(quat& quaternion, const vec3& from, const vec3& to);
    void ProjectLine(vec3& point, const vec3& from, const vec3& to);
}

#endif // FRAMEWORK_MATH_H