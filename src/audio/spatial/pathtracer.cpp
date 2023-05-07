#include <audio/spatial/pathtracer.h>

#include <components/rendercomponent.h>

#include <array>
#include <random>

namespace tram::Audio::Spatial {
using namespace tram::Render;

uint32_t xorshift() {
    static uint32_t x = 123456789;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x * 123456789;
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


static std::tuple<AABBTriangle, vec3, bool> NearestTriangleFromRay (vec3 ray_pos, vec3 ray_dir) {
    std::vector<RenderComponent*> intersected_render_components;
    std::vector<AABBTriangle> intersected_triangles;
    intersected_render_components.reserve(10);
    intersected_triangles.reserve(10);
    
    RenderComponent::FindAllFromRay(ray_pos, ray_dir, intersected_render_components);
    
    float nearest_distance = INFINITY;
    AABBTriangle nearest_triangle;
    vec3 nearest_intersection = {0.0f, 0.0f, 0.0f};
    
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
                nearest_intersection = comp_position + (comp_rotation * intersection);
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
    
    /*if (nearest_distance != INFINITY) {
        AddLine(nearest_triangle.point1, nearest_triangle.point2, COLOR_WHITE);
        AddLine(nearest_triangle.point2, nearest_triangle.point3, COLOR_WHITE);
        AddLine(nearest_triangle.point3, nearest_triangle.point1, COLOR_WHITE);
    }*/
    
    return {nearest_triangle, nearest_intersection - (ray_dir * 0.01f), nearest_distance != INFINITY};
}

bool RaySphereIntersection(vec3 ray_pos, vec3 ray_dir, vec3 sphere_pos, float sphere_radius) {
    vec3 L = sphere_pos - ray_pos;
    
    float tc = glm::dot(L, ray_dir);
    float d2 = glm::dot(L, L) - (tc * tc);
    
    return d2 < sphere_radius * sphere_radius && tc >= 0.0f;
}

static void FindSomePaths(std::vector<PathSegment>& segments, vec3 ray_pos, vec3 ray_dir, uint32_t iterations) {
    auto [triangle, intersection, hit_wall] = NearestTriangleFromRay(ray_pos, ray_dir);
    
    bool hit_listener = RaySphereIntersection(ray_pos, ray_dir, listener_position, 0.5f);
    
    if (!hit_wall || hit_listener || iterations > 9) {
        if (hit_listener && (!hit_wall || glm::distance(ray_pos, listener_position) < glm::distance(ray_pos, intersection))) {
            segments.push_back({ray_pos, listener_position});
        } else {
            segments.clear();
        }
        
        //AddLine(ray_pos, ray_dir * 10.0f, COLOR_CYAN);
        return;
    }
    
    //vec3 intersection = RayIntersectsTriangle(ray_pos, ray_dir, triangle.point1, triangle.point2, triangle.point3).first;
    
    if (intersection == vec3{0.0f,0.0f,0.0f}) {
        std::cout << "wut " << RayIntersectsTriangle(ray_pos, ray_dir, triangle.point1, triangle.point2, triangle.point3).second << std::endl;
    }
    
    vec3 new_dir = ray_dir - (2.0f * glm::dot(ray_dir, triangle.normal) * triangle.normal);
    
    //AddLine(ray_pos, intersection, COLOR_CYAN);
    //AddLineMarker(intersection, COLOR_PINK);
    
    segments.push_back({ray_pos, intersection});
    
    
    
    FindSomePaths(segments, intersection, new_dir, iterations + 1);
}
    
std::vector<PathSegment> all_segments;

uint32_t total_hits = 0;
uint32_t succ_hits = 0;

/*void FindPaths(std::vector<PathTracingResult>& paths, vec3 position) {
    std::vector<PathSegment> segments;
    segments.reserve(100);
    
    for (int i = 0; i < 25; i++) {
        float x = ((int32_t) xorshift()) - 2147483647;
        float y = ((int32_t) xorshift()) - 2147483647;
        float z = ((int32_t) xorshift()) - 2147483647;
        
        vec3 direction = glm::normalize(vec3 {x, y, z});
        
        FindSomePaths(segments, position, direction, 0);
        
        total_hits++;
        
        if (segments.size() == 0) continue;
        
        float distance = 0.0f;
        float force = 1.0f;
        
        for (auto& segment : segments) {
            distance += glm::distance(segment.segment_start, segment.segment_end);
            force *= 0.9f; // this would depend on the materials etc.
        }
        
        /*if (all_segments.size() < 400 && GetTick() > 400) {
            for (auto& segment : segments) {
                all_segments.push_back(segment);
            }
        }*//*
        
        float attenuation = (25.0f - distance) / 25.0f;
        
        if (attenuation < 0.0f) attenuation = 0.0f;
        
        //std::cout << attenuation << std::endl;
        
        force *= attenuation;
        
        vec3 end_direction = glm::normalize(segments.back().segment_end - segments.back().segment_start);
        
        (void)end_direction;
        
        succ_hits++;
        
        //paths.push_back({force, distance, end_direction, end_direction});
    }
    
    if (GetTick() % 200 == 100) {
        float ftotal = total_hits;
        float fsucc = succ_hits;
        
        total_hits = 0;
        succ_hits = 0;
        
        std::cout << "succ hits " << fsucc / ftotal << std::endl;
    }
    
        for (auto& segment : all_segments) {
            AddLine(segment.segment_start, segment.segment_end, COLOR_CYAN);
        }
}*/

bool StraightPathBetweenPoints(vec3 point1, vec3 point2) {
    vec3 direction = glm::normalize(point2 - point1);
    
    auto [triangle, intersection, hit_wall] = NearestTriangleFromRay(point1, direction);
    
    if (hit_wall) {
        return glm::distance(point1, intersection) - glm::distance(point1, point2) > -0.02f;
    }
    
    return true;
}

void SourceInsertNewPath(AudioSource& source, PathTracingResult result) {
    for (size_t i = 0; i < PATHS_FOR_RENDERING; i++) {
        // TODO: instead of replacing the first result, we might
        // actually replace the result with the lowest force.
        // could sound better
        if (source.result_paths[i].force < result.force) {
            source.result_paths[i] = result;
            return;
        }
    }
}

std::random_device device; 
std::mt19937 generator(device()); 

std::normal_distribution<float> bell_distribution(0.0f, 0.1f); 
std::uniform_real_distribution<float> uniform_distribution(0.0f, 1.0f);

void FindPaths(PathExplorationResult& result, bool metropolis, vec3 position) {
    vec3 ray_direction = result.sampling_direction;
    vec3 ray_position = position;
    
    std::vector<std::pair<vec3, vec3>> points;
    total_hits++;
    
    // make a variation of the ray direction
    if (metropolis) {
        vec3 random_vector = {
            bell_distribution(generator),
            bell_distribution(generator),
            bell_distribution(generator)
        };
        
        ray_direction = glm::normalize(ray_direction + random_vector);
    }
    
    // get a list of all the source that are playing
    int unconnected_sources[16]; // might want to bump this one up to max
    int source_count;
    
    for (size_t i = 0; i < SOURCE_COUNT; i++) {
        if (~audiorenders[i].flags & SOURCE_PLAYING) continue;
        
        unconnected_sources[source_count++] = i;
    }
    
    // init counters
    float force = 1.0f;
    float distance = 0.0f;
    float total_force = 0.0f;
    
    // do path tracing
    for (size_t n = 0; n < LISTENER_DEPTH_LIMIT; n++) {
        auto [triangle, intersection, hit_wall] = NearestTriangleFromRay(ray_position, ray_direction);
    
        if (!hit_wall) break;
    
        if (intersection == vec3{0.0f,0.0f,0.0f}) {
            std::cout << "wut " << RayIntersectsTriangle(ray_position, ray_direction, triangle.point1, triangle.point2, triangle.point3).second << std::endl;
        }
        
        force *= 0.9f; // TODO: actually sample absorption
        distance += glm::distance(ray_position, intersection);
        
        points.push_back({ray_position, intersection});
        
        ray_direction = ray_direction - (2.0f * glm::dot(ray_direction, triangle.normal) * triangle.normal);
        ray_position = intersection;
        
        
        
        
        for (int i = 0; i < source_count; i++) {
            if (unconnected_sources[i] < 0) continue;
            
            auto& source = audiosources[unconnected_sources[i]];
            
            // check if possible to directly connect to audio source
            if (StraightPathBetweenPoints(source.position, ray_position)) {
                total_force += force;
                
                SourceInsertNewPath(source, {
                    force,
                    distance + glm::distance(source.position, ray_position),
                    0,
                    glm::normalize(ray_position - source.position),
                    result.sampling_direction
                });
                
                for (auto& p : points) {
                    AddLine(p.first, p.second, COLOR_CYAN);
                }
                AddLine(ray_position, source.position, COLOR_CYAN);
                
                succ_hits++;
                
                unconnected_sources[i] = -1;
                goto success;
            }
            
            // check if can connect to other bouncy paths
            for (size_t j = 0; j < SOURCE_DEPTH_LIMIT; j++) {
                for (size_t k = 0; k < PATHS_FOR_SOURCE; k++) {
                    if (!StraightPathBetweenPoints(source.paths[k].reflections[j].point, ray_position)) continue;
                    
                    float path_force = force * source.paths[k].reflections[j].force;
                    total_force += path_force;
                    
                    SourceInsertNewPath(source, {
                        path_force,
                        distance + glm::distance(source.paths[k].reflections[j].point, ray_position),
                        0,
                        source.paths[k].reflections[j].direction,
                        result.sampling_direction
                    });
                    
                    for (auto& p : points) {
                        AddLine(p.first, p.second, COLOR_CYAN);
                    }
                    
                    if (j == 0) {
                        AddLine(ray_position, source.paths[k].reflections[0].point, COLOR_CYAN);
                        AddLine(source.paths[k].reflections[0].point, source.position, COLOR_CYAN);
                    } else {
                        AddLine(ray_position, source.paths[k].reflections[1].point, COLOR_CYAN);
                        AddLine(source.paths[k].reflections[1].point, source.paths[k].reflections[0].point, COLOR_CYAN);
                        AddLine(source.paths[k].reflections[0].point, source.position, COLOR_CYAN);
                    }
                    
                    succ_hits++;
                    
                    unconnected_sources[i] = -1;
                    goto success;
                }
            }
            
            success:;
        }
    }
    
    if (uniform_distribution(generator) >= glm::min(total_force / result.force, 1.0f)) {
        result.sampling_direction = ray_direction;
        result.force = force;
    }
    
    static uint32_t lastgood = 1;
    
    if (GetTick() % 200 == 100 && GetTick() != lastgood) {
        float ftotal = total_hits;
        float fsucc = succ_hits;
        lastgood = GetTick();
        
        total_hits = 0;
        succ_hits = 0;
        
        std::cout << "succ hits " << fsucc / ftotal << std::endl;
    }
}

/*
void FindPathsMetropolis(PathExplorationResult* exploration, PathTracingResult* result, vec3 position) {
    std::vector<PathSegment> segments;
    segments.reserve(100);
    
    for (size_t i = 0; i < PATHS_FOR_EXPLORATION; i++) {
        vec3 random_vector = {
            bell_distribution(generator),
            bell_distribution(generator),
            bell_distribution(generator)
        };
        
        vec3 direction = glm::normalize(exploration[i].sampling_direction + random_vector);
        
        FindSomePaths(segments, position, direction, 0);
        
        total_hits++;
        
        if (segments.size() == 0) {
            exploration[i].sampling_direction = direction;
            exploration[i].force = 0.0f;
            
            exploration[i].cycles_since_last_hit++;
            
            if (exploration[i].cycles_since_last_hit > 10) {
                exploration[i].sampling_direction = glm::normalize(random_vector);
            }
            
            //std::cout << exploration[i].cycles_since_last_hit << std::endl;
            continue;
        }
        
        exploration[i].cycles_since_last_hit = 0;
        
        float distance = 0.0f;
        float force = 1.0f;
        
        for (auto& segment : segments) {
            distance += glm::distance(segment.segment_start, segment.segment_end);
            force *= 0.9f; // this would depend on the materials etc.
        }
        
        /*if (all_segments.size() < 400 && GetTick() > 400) {
            for (auto& segment : segments) {
                all_segments.push_back(segment);
            }
        }*//*
        
        float attenuation = (25.0f - distance) / 25.0f;
        
        if (attenuation < 0.0f) attenuation = 0.0f;
        
        //std::cout << attenuation << std::endl;
        
        force *= attenuation;
        
        vec3 end_direction = glm::normalize(segments.back().segment_end - segments.back().segment_start);
        
        succ_hits++;
        
        // Metropolising
        if (uniform_distribution(generator) >= glm::min(force / exploration[i].force, 1.0f)) {
            exploration[i].sampling_direction = direction;
            exploration[i].force = force;
        }
        
        // check if result is better than existing results
        for (size_t k = 0; k < PATHS_FOR_RENDERING; k++) {
            if (result[k].force >= force) continue;
            
            result[k].force = force;
            result[k].distance = distance;
            result[k].sampling_direction = direction;
            result[k].arrival_direction = end_direction;
            result[k].cycles_since_last_hit = 0;
            
            break;
        }
    }
    
    if (GetTick() % 200 == 100) {
        float ftotal = total_hits;
        float fsucc = succ_hits;
        
        total_hits = 0;
        succ_hits = 0;
        
        std::cout << "succ hits " << fsucc / ftotal << std::endl;
    }
    
    for (auto& segment : all_segments) {
        AddLine(segment.segment_start, segment.segment_end, COLOR_CYAN);
    }
}*/

void ValidateResult(PathTracingResult& result, vec3 position) {
    
}

void MakeSomeSourcePaths(PathFromAudioSource& path, vec3 source_position) {
    vec3 ray_direction = glm::normalize(vec3 {
        bell_distribution(generator),
        bell_distribution(generator),
        bell_distribution(generator)
    });
    
    vec3 ray_position = source_position;
    float force = 1.0f;
    
    for (int n = 0; n < 2; n++) {
        auto [triangle, intersection, hit_wall] = NearestTriangleFromRay(ray_position, ray_direction);
    
        if (intersection == vec3{0.0f,0.0f,0.0f}) {
            std::cout << "wut " << RayIntersectsTriangle(ray_position, ray_direction, triangle.point1, triangle.point2, triangle.point3).second << std::endl;
        }
        
        force *= 0.9f; // TODO: actually sample absorption
        
        ray_direction = ray_direction - (2.0f * glm::dot(ray_direction, triangle.normal) * triangle.normal);
        ray_position = intersection;
        
        path.reflections[n].force = force;
        path.reflections[n].point = ray_position;
        path.reflections[n].direction = ray_direction;
    }
    
}

void InitExplorationPaths(PathExplorationResult* paths) {
    for (size_t i = 0; i < PATHS_FOR_LISTENER; i++) {
        paths[i].sampling_direction = glm::normalize(vec3 {
            bell_distribution(generator),
            bell_distribution(generator),
            bell_distribution(generator)
        });
        
        paths[i].force = 0.0f;
        paths[i].cycles_since_last_hit = 0;
    }
}

}