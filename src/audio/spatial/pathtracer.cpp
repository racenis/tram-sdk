#include <audio/spatial/pathtracer.h>

#include <components/rendercomponent.h>

namespace tram::Audio::Spatial {
using namespace tram::Render;

uint32_t xorshift() {
    static uint32_t x = 42069;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

struct PathSegment {
    vec3 segment_start;
    vec3 segment_end;
};

static std::pair<vec3, bool> RayIntersectsTriangle(vec3 ray_pos, vec3 ray_dir, vec3 point1, vec3 point2, vec3 point3) {
    const float epsilon = 0.000001;
    
    const vec3 p1_p2 = point2 - point1;
    const vec3 p1_p3 = point3 - point1;
    
    const vec3 p_vec = glm::cross(ray_dir, p1_p3);
    const float det = glm::dot(p1_p2, p_vec);

    if (fabs(det) < epsilon) {
        return {vec3{}, false};
    }

    const float inv_det = 1.0 / det;
    const vec3 t_vec = ray_pos - point1;
    const float u = inv_det * glm::dot(t_vec, p_vec);

    if (u < 0.0 || u > 1.0) {
        return {vec3{}, false};
    }
    
    const vec3 q_vec = glm::cross(t_vec, p1_p2);
    const float v = inv_det * glm::dot(ray_dir, q_vec);

    if (v < 0.0 || u + v > 1.0) {
        return {vec3{}, false};
    }

    const float t = inv_det * glm::dot(p1_p3, q_vec);

    if (t > epsilon) {
        return {ray_pos + (ray_dir * t), true};
    } else {
        return {vec3{}, false};
    }

}


static std::pair<AABBTriangle, bool> NearestTriangleFromRay (vec3 ray_pos, vec3 ray_dir) {
    std::vector<RenderComponent*> intersected_render_components;
    std::vector<AABBTriangle> intersected_triangles;
    intersected_render_components.reserve(10);
    intersected_triangles.reserve(10);
    
    RenderComponent::FindAllFromRay(ray_pos, ray_dir, intersected_render_components);
    
    float nearest_distance = INFINITY;
    AABBTriangle nearest_triangle;
    
    for (auto comp : intersected_render_components) {
        auto comp_position = comp->GetLocation();
        auto comp_rotation = comp->GetRotation();
        
        vec3 ray_pos_local = glm::inverse(comp_rotation) * (ray_pos - comp_position);
        vec3 ray_dir_local = glm::inverse(comp_rotation) * ray_dir;
        
        
        std::vector<AABBTriangle> intersected_triangles;
        intersected_triangles.reserve(10);

        Model::Find(comp->GetModel())->FindAllFromRay(ray_pos_local, ray_dir_local, intersected_triangles);
        
        
        for (auto& tri : intersected_triangles) {
            auto [intersection, happened] = RayIntersectsTriangle(ray_pos_local, ray_dir_local, tri.point1, tri.point2, tri.point3);
            
            if (!happened) continue;
            
            float intersection_distance = glm::distance(ray_pos_local, intersection);
            
            if (nearest_distance > intersection_distance) {
                nearest_distance = intersection_distance;
                nearest_triangle = {
                    comp_position + (comp_rotation * tri.point1),
                    comp_position + (comp_rotation * tri.point2),
                    comp_position + (comp_rotation * tri.point3),
                    comp_rotation * tri.normal,
                    tri.material
                };
            }
        }
    }
    
    if (nearest_distance != INFINITY) {
        AddLine(nearest_triangle.point1, nearest_triangle.point2, COLOR_WHITE);
        AddLine(nearest_triangle.point2, nearest_triangle.point3, COLOR_WHITE);
        AddLine(nearest_triangle.point3, nearest_triangle.point1, COLOR_WHITE);
    }
    
    return {nearest_triangle, nearest_distance != INFINITY};
}

bool RaySphereIntersection(vec3 ray_pos, vec3 ray_dir, vec3 sphere_pos, float sphere_radius) {
    vec3 L = sphere_pos - ray_pos;
    
    float tc = glm::dot(L, ray_dir);
    float d2 = glm::dot(L, L) - (tc * tc);
    
    return d2 < sphere_radius * sphere_radius && tc >= 0.0f;
}

static void FindSomePaths(std::vector<PathSegment>& segments, vec3 ray_pos, vec3 ray_dir, uint32_t iterations) {
    auto [triangle, hit_wall] = NearestTriangleFromRay(ray_pos, ray_dir);
    
    bool hit_listener = RaySphereIntersection(ray_pos, ray_dir, listener_position, 0.5f);
    
    if (!hit_wall) {
        if (hit_listener) {
            segments.push_back({ray_pos, listener_position});
        } else {
            segments.clear();
        }
        
        AddLine(ray_pos, ray_dir * 10.0f, COLOR_CYAN);
        return;
    } 
    
    vec3 intersection = RayIntersectsTriangle(ray_pos, ray_dir, triangle.point1, triangle.point2, triangle.point3).first;
    
    vec3 new_dir = ray_dir - (2.0f * glm::dot(ray_dir, triangle.normal) * triangle.normal);
    
    AddLine(ray_pos, intersection, COLOR_CYAN);
    //AddLineMarker(intersection, COLOR_PINK);
    
    segments.push_back({ray_pos, intersection});
    
    if (iterations < 10) {
        FindSomePaths(segments, intersection, new_dir, iterations + 1);
    }
}
    


void FindPaths(std::vector<PathResult>& paths, vec3 position) {
    std::vector<PathSegment> segments;
    segments.reserve(10);
    
    for (int i = 0; i < 10; i++) {
        float x = ((int32_t) xorshift()) - 2147483647;
        float y = ((int32_t) xorshift()) - 2147483647;
        float z = ((int32_t) xorshift()) - 2147483647;
        
        vec3 direction = glm::normalize(vec3 {x, y, z});
        
        FindSomePaths(segments, position, direction, 0);
        
        if (segments.size() == 0) continue;
        
        float distance = 0.0f;
        float force = 1.0f;
        
        for (auto& segment : segments) {
            distance += glm::distance(segment.segment_start, segment.segment_end);
            force *= 0.9f;
        }
        
        vec3 end_direction = glm::normalize(segments.back().segment_end - segments.back().segment_start);
        
        paths.push_back({force, distance, end_direction});
    }
}

}