// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/math.h>

namespace tram {

void QuatLookAt (glm::quat& quaternion, const glm::vec3& from, const glm::vec3& to) {
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

void ProjectLine (glm::vec3& point, const glm::vec3& from, const glm::vec3& to) {
    const glm::vec3 projectable = point - from;
    const glm::vec3 line = to - from;
    point = from + (glm::dot(projectable, line) / glm::dot(line, line) * line);
}

vec3 RayTriangleIntersection(vec3 ray_pos, vec3 ray_dir, vec3 point1, vec3 point2, vec3 point3) {
    const float epsilon = 0.000001;
    
    const vec3 p1_p2 = point2 - point1;
    const vec3 p1_p3 = point3 - point1;
    
    const vec3 p_vec = glm::cross(ray_dir, p1_p3);
    const float det = glm::dot(p1_p2, p_vec);

    if (fabs(det) < epsilon) {
        return {INFINITY, INFINITY, INFINITY};
    }

    const float inv_det = 1.0 / det;
    const vec3 t_vec = ray_pos - point1;
    const float u = inv_det * glm::dot(t_vec, p_vec);

    if (u < 0.0 || u > 1.0) {
        return {INFINITY, INFINITY, INFINITY};
    }
    
    const vec3 q_vec = glm::cross(t_vec, p1_p2);
    const float v = inv_det * glm::dot(ray_dir, q_vec);

    if (v < 0.0 || u + v > 1.0) {
        return {INFINITY, INFINITY, INFINITY};
    }

    const float t = inv_det * glm::dot(p1_p3, q_vec);

    if (t > epsilon) {
        return ray_pos + (ray_dir * t);
    } else {
        return {INFINITY, INFINITY, INFINITY};
    }

}

}