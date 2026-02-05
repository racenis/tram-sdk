// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/math.h>

#include <cmath>

/**
 * @file framework/math.cpp
 * 
 * GLM typedefs and useful math functions.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/math.html
 */

namespace tram {

// TODO: create QuatLookAt that returns the quaternion as return instead of reference param 
/// Rotates a quaternion to face a point.
/// Useful for pointing objects at things.
void QuatLookAt(quat& quaternion, const vec3& from, const vec3& to) {
    quaternion = glm::quatLookAt(glm::normalize(to - from), DIRECTION_UP);
}

/// Projects a point on a line.
/// @param [in,out] point Point that will be projected.
/// @param [in]     from  First endpoint of the line.
/// @param [in]     to    Second endpoint of the line.
void ProjectLine(vec3& point, const vec3& from, const vec3& to) {
    const vec3 projectable = point - from;
    const vec3 line = to - from;
    point = from + (glm::dot(projectable, line) / glm::dot(line, line) * line);
}

/// Finds the intersection between a ray and a triangle.
/// If the ray intersects the triangle, the returned vector will be the 3D
/// coordinates of the intersection. If the ray does not intersect the triangle,
/// the function will return a vector with all of its components set to the
/// floating-point INFINITY.
/// @param ray_pos              Ray origin, 3D coordinate.
/// @param ray_dir              Ray direction, normal vector. Needs to be normalized.
/// @param point1,point2,point3 Triangle vertex coordinates.
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

// TODO: create an AABB struct and use that instead of the min/max
// this kind of signature: AABB RotateAABB(AABB box, quat rotation)

/// Rotates an AABB box by the given rotation.
void RotateAABB(vec3& min, vec3& max, quat rotation) {
    vec3 extents[8] = {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {min.x, max.y, min.z},
        {min.x, min.y, max.z},
        {max.x, max.y, min.z},
        {max.x, min.y, max.z},
        {max.x, max.y, max.z},
        {min.x, max.y, max.z}
    };
    
    for (auto& extent : extents) {
        extent = rotation * extent;
    }
    
    min = extents[0];
    max = extents[0];
    
    for (auto& extent : extents) {
        min = MergeAABBMin(min, extent);
        max = MergeAABBMax(max, extent);
    }
}

void RotateAABB(vec3& min, vec3& max, mat4 rotation) {
    vec3 extents[8] = {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {min.x, max.y, min.z},
        {min.x, min.y, max.z},
        {max.x, max.y, min.z},
        {max.x, min.y, max.z},
        {max.x, max.y, max.z},
        {min.x, max.y, max.z}
    };
    
    for (auto& extent : extents) {
        extent = rotation * vec4(extent, 1.0f);
    }
    
    min = extents[0];
    max = extents[0];
    
    for (auto& extent : extents) {
        min = MergeAABBMin(min, extent);
        max = MergeAABBMax(max, extent);
    }
}

// TODO: switch these to pass by value, not reference?

/// Generates a transform matrix.
mat4 PositionRotationToMatrix(const vec3& position, const quat& rotation) {
    mat4 matrix = mat4(1.0f);
    matrix = glm::translate(matrix, position);
    matrix *= glm::toMat4(rotation);
    return matrix;
}

/// Generates a transform matrix.
mat4 PositionRotationScaleToMatrix(const vec3& position, const quat& rotation, const vec3& scale) {
    mat4 matrix = mat4(1.0f);
    matrix = glm::translate(matrix, position);
    matrix *= glm::toMat4(rotation);
    matrix = glm::scale(matrix, scale);
    return matrix;
}

static vec3 euler_normalize(vec3 euler) {
    const float tupi = 2.0f * glm::pi<float>();
    const float bias = 0.02f;

    float x = fmodf(euler.x, tupi);
    float y = fmodf(euler.y, tupi);
    float z = fmodf(euler.z, tupi);

    if (x < 0.0f) x += tupi;
    if (y < 0.0f) y += tupi;
    if (z < 0.0f) z += tupi;
    
    if (x > tupi - bias) x = 0.0f;
    if (y > tupi - bias) y = 0.0f;
    if (z > tupi - bias) z = 0.0f;

    return  {x, y, z};
}

static vec3 euler_alt_normalize(vec3 euler) {
    float x = atan2f(sinf(euler.x), cosf(euler.x));
    float y = atan2f(sinf(euler.y), cosf(euler.y));
    float z = atan2f(sinf(euler.z), cosf(euler.z));

    return  {x, y, z};
}

static float euler_error(vec3 a, vec3 b) {
    return pow(a.x - b.x, 4.0f) + pow(a.y - b.y, 4.0f) + pow(a.z - b.z, 4.0f);
}

/// Extracts euler angles from a quaternion.
/// Since multiple sets of euler angles correspond to the same quaternion
/// rotation, this function will attempt to find a set of euler angles that is
/// nearest to some previous set of euler angles. It also gracefully handles
/// gimbal locking.
vec3 EulerFromQuat(quat rotation, vec3 previous) {
    const float pi = glm::pi<float>();
    const float epsilon = 0.02f;

    const bool previous_given = !std::isnan(previous.x);

    vec3 euler = glm::eulerAngles(rotation);
    vec3 euler_alt = {euler.x + pi, pi - euler.y, euler.z + pi};

    euler = euler_alt_normalize(euler);
    euler_alt = euler_alt_normalize(euler_alt);

    if (!previous_given) {
        previous = {0.0f, 0.0f, 0.0f};
    }

    if (euler_error(previous, euler) > euler_error(previous, euler_alt)) {
        std::swap(euler, euler_alt);
    }

    const bool gimbal_lock = fabsf(euler.y - 0.5f * pi) < epsilon
        || fabsf(euler.y - 1.5f * pi) < epsilon;

    if (!previous_given) {
        if (gimbal_lock) {
            euler.x = euler.x - euler.z;
            euler.z = 0.0f;
        }
    } else {
        if (gimbal_lock) {
            euler.x = euler.x - euler.z;
            euler.z = 0.0f;
            
            euler.z = previous.x - euler.x;
            euler.x = previous.x;
        }
    }

    return euler_normalize(euler);
}

}