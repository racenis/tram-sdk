// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_MATH_H
#define TRAM_SDK_FRAMEWORK_MATH_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace tram {

typedef glm::vec3 vec3;
typedef glm::quat quat;
typedef glm::mat2 mat2;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec4 vec4;
typedef glm::vec2 vec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;
typedef glm::uvec3 uvec3;
typedef glm::uvec4 uvec4;

const vec3 DIRECTION_FORWARD    (0.0f,  0.0f, -1.0f);
const vec3 DIRECTION_SIDE       (1.0f,  0.0f,  0.0f);
const vec3 DIRECTION_UP         (0.0f,  1.0f,  0.0f);

const vec3 IDENTITY_POSITION    (0.0f, 0.0f, 0.0f);
const quat IDENTITY_ROTATION    (1.0f, 0.0f, 0.0f, 0.0f);
const vec3 IDENTITY_SCALE       (1.0f, 1.0f, 1.0f);

void QuatLookAt(quat& quaternion, const vec3& from, const vec3& to);
void ProjectLine(vec3& point, const vec3& from, const vec3& to);

vec3 RayTriangleIntersection(vec3 ray_pos, vec3 ray_dir, vec3 point1, vec3 point2, vec3 point3);

vec3 MergeAABBMin(vec3 a, vec3 b);
vec3 MergeAABBMax(vec3 a, vec3 b);

void RotateAABB(vec3& min, vec3& max, quat rotation);
void RotateAABB(vec3& min, vec3& max, mat4 rotation);

mat4 PositionRotationToMatrix(const vec3& position, const quat& rotation);
mat4 PositionRotationScaleToMatrix(const vec3& position, const quat& rotation, const vec3& scale);

}

#endif // TRAM_SDK_FRAMEWORK_MATH_H