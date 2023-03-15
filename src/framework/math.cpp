// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/math.h>

namespace tram {
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
}