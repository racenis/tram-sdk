// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/meshtools/primitives.h>

/**
 * @namespace tram::Ext::Meshtools
 * 
 * Extension containing various tools for generating meshes.
 * 
 * Currently we have a sphere cube generation function. That's it. Might add
 * some more primitives later.
 */

namespace tram::Ext::Meshtools {

void AddQuad(MeshComponent& mesh,
             const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4,
             const vec3& n1, const vec3& n2, const vec3& n3, const vec3& n4,
             const vec2& uv1, const vec2& uv2, const vec2& uv3, const vec2& uv4) {
    MeshVertex v1(Render::VERTEX_DYNAMIC);
    MeshVertex v2(Render::VERTEX_DYNAMIC);
    MeshVertex v3(Render::VERTEX_DYNAMIC);
    MeshVertex v4(Render::VERTEX_DYNAMIC);
    
    v1.SetAttribute(3, 0); v2.SetAttribute(3, 0); v3.SetAttribute(3, 0); v4.SetAttribute(3, 0);
    v1.SetAttribute(4, vec4{1.0, 0.0, 0.0, 0.0}); v2.SetAttribute(4, vec4{1.0, 0.0, 0.0, 0.0});
    v3.SetAttribute(4, vec4{1.0, 0.0, 0.0, 0.0}); v4.SetAttribute(4, vec4{1.0, 0.0, 0.0, 0.0});
    v1.SetAttribute(5, 0); v2.SetAttribute(5, 0); v3.SetAttribute(5, 0); v4.SetAttribute(5, 0);
    
    v1.SetPosition(p1); v2.SetPosition(p2); v3.SetPosition(p3); v4.SetPosition(p4);
    v1.SetNormal(n1); v2.SetNormal(n2); v3.SetNormal(n3); v4.SetNormal(n4);
    v1.SetTextureUV(uv1); v2.SetTextureUV(uv2); v3.SetTextureUV(uv3); v4.SetTextureUV(uv4); 

    mesh.Add(v1, v2, v3);
    mesh.Add(v4, v3, v2);
}

vec3 SquishBalls(vec3 p) {
    float x = p.x * sqrtf(1.0f - (p.y * p.y / 2.0f) - (p.z * p.z / 2.0f) + (p.y * p.y * p.z * p.z / 2.0f));
    float y = p.y * sqrtf(1.0f - (p.x * p.x / 2.0f) - (p.z * p.z / 2.0f) + (p.x * p.x * p.z * p.z / 2.0f));
    float z = p.z * sqrtf(1.0f - (p.y * p.y / 2.0f) - (p.x * p.x / 2.0f) + (p.y * p.y * p.x * p.x / 2.0f));
    return glm::normalize(vec3{x, y, z});
}

void MakeBall(MeshComponent& mesh, float radius,
              const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4,
              const vec2& uv1, const vec2& uv2, const vec2& uv3, const vec2& uv4) {
    vec3 v1 = SquishBalls(p1);
    vec3 v2 = SquishBalls(p2);
    vec3 v3 = SquishBalls(p3);
    vec3 v4 = SquishBalls(p4);

    AddQuad(mesh, radius * v1, radius * v2, radius * v3, radius * v4, v1, v2, v3, v4, uv1, uv2, uv3, uv4);
}

void MakeCubeSphere(MeshComponent& mesh, size_t subdivisions, float radius) {
    mesh.Clear();
    
    const float RADIUS_UNIT = 0.5f;
    
    float step = (2.0f * RADIUS_UNIT) / (1 + subdivisions);
    
    float progress_x = -0.5f;
    float progress_y = -RADIUS_UNIT;
    for (size_t x = 0; x < subdivisions + 1; x++) {
        for (size_t y = 0; y < subdivisions + 1; y++) {
            vec3 p1 = {progress_x, progress_y, RADIUS_UNIT};
            vec3 p2 = {progress_x + step, progress_y, RADIUS_UNIT};
            vec3 p3 = {progress_x, progress_y + step, RADIUS_UNIT};
            vec3 p4 = {progress_x + step, progress_y + step, RADIUS_UNIT};
            vec3 n = {0, 0, 1};
            vec2 uv1 = {RADIUS_UNIT + progress_x, RADIUS_UNIT + progress_y};
            vec2 uv2 = {RADIUS_UNIT + progress_x + step, RADIUS_UNIT + progress_y};
            vec2 uv3 = {RADIUS_UNIT + progress_x, RADIUS_UNIT + progress_y + step};
            vec2 uv4 = {RADIUS_UNIT + progress_x + step, RADIUS_UNIT + progress_y + step};

            // BACK
            MakeBall(mesh, radius, p1, p2, p3, p4, uv1, uv2, uv3, uv4);
            
            p1.z = p2.z = p3.z = p4.z = -RADIUS_UNIT;
            
            // FRONT
            std::swap(p1, p4); std::swap(uv1, uv4);
            MakeBall(mesh, radius, p1, p2, p3, p4, uv1, uv2, uv3, uv4);
            
            p1 = {progress_x, RADIUS_UNIT, progress_y};
            p2 = {progress_x + step, RADIUS_UNIT, progress_y};
            p3 = {progress_x, RADIUS_UNIT, progress_y + step};
            p4 = {progress_x + step, RADIUS_UNIT, progress_y + step};
            n = {0, 1, 0};
            
            // TOP
            std::swap(p1, p4);
            MakeBall(mesh, radius, p1, p2, p3, p4, uv1, uv2, uv3, uv4);
            
            p1.y = p2.y = p3.y = p4.y = -RADIUS_UNIT;
            
            // BOTTOM
            std::swap(p1, p4); std::swap(uv1, uv4);
            MakeBall(mesh, radius, p1, p2, p3, p4, uv1, uv2, uv3, uv4);
            
            p1 = {RADIUS_UNIT, progress_x, progress_y};
            p2 = {RADIUS_UNIT, progress_x + step, progress_y};
            p3 = {RADIUS_UNIT, progress_x, progress_y + step};
            p4 = {RADIUS_UNIT, progress_x + step, progress_y + step};
            n = {1, 0, 0};
            
            // LEFT
            MakeBall(mesh, radius, p1, p2, p3, p4, uv1, uv2, uv3, uv4);
            
            p1.x = p2.x = p3.x = p4.x = -RADIUS_UNIT;
            
            // RIGHT
            std::swap(p1, p4); std::swap(uv1, uv4);
            MakeBall(mesh, radius, p1, p2, p3, p4, uv1, uv2, uv3, uv4);
 
            progress_y += step;
        }
        
        progress_y = -RADIUS_UNIT;
        progress_x += step;
    }
    
    mesh.Commit();
}


}

