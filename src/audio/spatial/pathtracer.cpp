#include <audio/spatial/pathtracer.h>

#include <components/render.h>

#include <array>
#include <random>
#include <algorithm>

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
        if (source.result_paths[i].force < result.force &&
            glm::dot(source.result_paths[i].arrival_direction, result.arrival_direction) < 0.7f) {
            source.result_paths[i] = result;
            return;
        }
    }
    
    /*size_t smallest = 0;
    
    for (size_t i = 0; i < PATHS_FOR_RENDERING; i++) {
        if (source.result_paths[i].force < source.result_paths[smallest].force) {
            smallest = i;
        }
    }
    
    if (source.result_paths[smallest].force < result.force) {
        source.result_paths[smallest] = result;
    }*/
}

std::random_device device; 
std::mt19937 generator(device()); 

std::normal_distribution<float> bell_distribution(0.0f, 0.05f); 
std::uniform_real_distribution<float> uniform_distribution(0.0f, 1.0f);
std::uniform_real_distribution<float> uniform_distribution2(-1.0f, 1.0f);

void FindPaths(PathExplorationResult& result, bool metropolis, vec3 position) {
    vec3 sampling_direction = result.sampling_direction;
    
    std::vector<std::pair<vec3, vec3>> points;
    total_hits++;
    
    vec3 random_vector = {
        bell_distribution(generator),
        bell_distribution(generator),
        bell_distribution(generator)
    };
    
    // make a variation of the ray direction
    if (result.cycles_since_last_hit > 10 || uniform_distribution(generator) < 0.05f) {
        sampling_direction = glm::normalize(random_vector);
    } else {
        sampling_direction = glm::normalize(sampling_direction + random_vector);
    }
    
    vec3 ray_direction = sampling_direction;
    vec3 ray_position = position;
    
    vec3 arrival_direction = -ray_direction;
    
    // get a list of all the source that are playing
    int unconnected_sources[16]; // might want to bump this one up to max
    int source_count = 0;
    
    for (size_t i = 0; i < SOURCE_COUNT; i++) {
        if (~audiorenders[i].flags & SOURCE_PLAYING) continue;
        
        unconnected_sources[source_count++] = i;
    }
    
    // init counters
    float force = 1.0f;
    float distance = 0.0f;
    float total_force = 0.0f;
    
    vec3 reflections[LISTENER_DEPTH_LIMIT];
    uint32_t reflection_count = 0;
    
    // do path tracing
    for (size_t n = 0; n < LISTENER_DEPTH_LIMIT; n++) {
        auto [triangle, intersection, hit_wall] = NearestTriangleFromRay(ray_position, ray_direction);
    
        if (!hit_wall) break;
    
        force *= 0.9f; // TODO: actually sample absorption when loader is finish
        distance += glm::distance(ray_position, intersection);
        
        points.push_back({ray_position, intersection});
        
        // calculate reflected direction
        ray_direction = ray_direction - (2.0f * glm::dot(ray_direction, triangle.normal) * triangle.normal);
        ray_position = intersection;
        
        // save reflection point
        reflections[reflection_count++] = intersection;
        
        for (int i = 0; i < source_count; i++) {
            if (unconnected_sources[i] < 0) continue;
            
            auto& source = audiosources[unconnected_sources[i]];
            
            // check if possible to directly connect to audio source
            if (StraightPathBetweenPoints(source.position, ray_position)) {
                float connection_goodness = glm::dot(ray_direction, glm::normalize(source.position - ray_position));
                float path_force = force * connection_goodness;
                
                if (connection_goodness > 0.5f) {
                    total_force += path_force;

                    PathTracingResult new_result = {
                        path_force,
                        distance + glm::distance(source.position, ray_position),
                        0,
                        reflection_count,
                        arrival_direction,
                        force,
                        arrival_direction,
                        ray_direction
                    };
                    
                    for (uint32_t i = 0; i < reflection_count; i++) {
                        new_result.reflections[reflection_count - i - 1].point = reflections[i];
                    }
                    
                    SourceInsertNewPath(source, new_result);
                    
                    /*for (auto& p : points) {
                        AddLine(p.first, p.second, COLOR_PINK);
                    }
                    AddLine(ray_position, source.position, COLOR_PINK);*/
                    
                    succ_hits++;
                    
                    unconnected_sources[i] = -1;
                    goto success;
                }
            }
            
            // check if can connect to other bouncy paths
            for (uint32_t j = 0; j < SOURCE_DEPTH_LIMIT; j++) {
                for (uint32_t k = 0; k < PATHS_FOR_SOURCE; k++) {
                    if (!StraightPathBetweenPoints(source.paths[k].reflections[j].point, ray_position)) continue;
                    
                    float connection_goodness = glm::dot(ray_direction, -source.paths[k].reflections[j].direction);
                    float unconnected_force = force * source.paths[k].reflections[j].force;
                    float path_force = unconnected_force * connection_goodness;
                    
                    if (connection_goodness > 0.5f) {
                        total_force += path_force;
                        
                        PathTracingResult new_result = {
                            path_force,
                            distance + glm::distance(source.paths[k].reflections[j].point, ray_position),
                            0,
                            reflection_count + j + 1,
                            arrival_direction,
                            unconnected_force,
                            arrival_direction,
                            glm::normalize(source.position - source.paths[k].reflections[0].point)
                        };
                        
                        for (uint32_t i = 0; i < j + 1; i++) {
                            new_result.reflections[i].point = source.paths[k].reflections[i].point;
                        }
                        
                        for (uint32_t i = 0; i < reflection_count; i++) {
                            new_result.reflections[j + reflection_count - i].point = reflections[i];
                        }
                        
                        SourceInsertNewPath(source, new_result);
                        
                        /*for (auto& p : points) {
                            AddLine(p.first, p.second, COLOR_PINK);
                        }
                        
                        if (j == 0) {
                            AddLine(ray_position, source.paths[k].reflections[0].point, COLOR_PINK);
                            AddLine(source.paths[k].reflections[0].point, source.position, COLOR_PINK);
                        } else {
                            AddLine(ray_position, source.paths[k].reflections[1].point, COLOR_PINK);
                            AddLine(source.paths[k].reflections[1].point, source.paths[k].reflections[0].point, COLOR_PINK);
                            AddLine(source.paths[k].reflections[0].point, source.position, COLOR_PINK);
                        }*/
                        
                        succ_hits++;
                        
                        unconnected_sources[i] = -1;
                        goto success;
                    }
                }
            }
            
            result.cycles_since_last_hit++;
            goto unsuccess;
        success:;
            result.cycles_since_last_hit = 0;
        unsuccess:;
        }
    }
    
    float random = uniform_distribution(generator);
    float compar = glm::min(total_force / result.force, 1.0f);
    
    if (result.force == 0 || random >= compar) {
        result.sampling_direction = sampling_direction;
        result.force = total_force;
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

bool ValidatePathSegment(vec3 ray_pos, vec3 target, float& distance) {
    vec3 ray_dir = glm::normalize(target - ray_pos);
    
    auto [triangle, intersection, hit_wall] = NearestTriangleFromRay(ray_pos, ray_dir);
    
    if (!hit_wall) return false;
    if (glm::distance(target, intersection) > 0.2f) return false;
    
    distance += glm::distance(ray_pos, intersection);
    
    return true;
}

void ColorPath(PathTracingResult& result, vec3 position, vec3 color) {
    AddLine(position, result.reflections[0].point, color);
    
    for (uint32_t i = 1; i < result.reflection_count; i++) {
        AddLine(result.reflections[i-1].point, result.reflections[i].point, color);
    }
    
    AddLine(result.reflections[result.reflection_count-1].point, listener_position, color);
    
    AddLineMarker(position, COLOR_GREEN);
}

void ValidateResult(PathTracingResult& result, vec3 position) {
    if (result.reflection_count == 0) return;
    
    vec3 ray_pos = position;
    float distance = 0.0f;
    float connection_goodness = 1.0f;
    float goodness_compressed;
    
    if (!ValidatePathSegment(ray_pos, result.reflections[0].point, distance)) {
        goto fail;
    }
    
    for (uint32_t i = 1; i < result.reflection_count; i++) {
        if (!ValidatePathSegment(result.reflections[i-1].point, result.reflections[i].point, distance)) {
            goto fail;
        }
    }
    
    if (!ValidatePathSegment(listener_position, result.reflections[result.reflection_count-1].point, distance)) {
        goto fail;
    }
    
    connection_goodness *= glm::dot(result.source_ideal_angle, glm::normalize(position - result.reflections[0].point));
    connection_goodness *= glm::dot(result.listener_ideal_angle, glm::normalize(listener_position - result.reflections[result.reflection_count-1].point));

    if (connection_goodness < 0.25f) {
        goto fail;
    }
    
    goodness_compressed = ((connection_goodness - 0.25f) * 0.2f) + 0.8125f;
    
    result.distance = distance;
    result.force = result.reflection_absorption * goodness_compressed;//* connection_goodness;
    result.arrival_direction = listener_position - result.reflections[result.reflection_count - 1].point;
    
    //ColorPath(result, position, COLOR_GREEN);
    
    return;
fail:
    
    result.cycles_since_last_hit++;
    
    //ColorPath(result, position, COLOR_RED);
}

void RenderResult(PathTracingResult& result, vec3 position) {
    if (result.reflection_count == 0) return;
    
    AddLine(position, result.reflections[0].point, COLOR_CYAN);
    
    for (uint32_t i = 1; i < result.reflection_count; i++) {
        AddLine(result.reflections[i-1].point, result.reflections[i].point, COLOR_CYAN);
    }
    
    AddLine(result.reflections[result.reflection_count-1].point, listener_position, COLOR_CYAN);
}

void CheckReverb(PathExplorationResult* paths) {
    struct {
        float force;
        float distance;
    } result[5];
    
    for (size_t i = 0; i < PATHS_FOR_LISTENER; i++) {
        vec3 ray_position = listener_position;
        vec3 ray_direction = paths[i].sampling_direction;
        
         auto [triangle, intersection, hit_wall] = NearestTriangleFromRay(ray_position, ray_direction);
         
         if (!hit_wall) {
            result[i].force = 0.0f;
            result[i].distance = listener_reverb_distance[i];
            continue;
        }
        
        result[i].force = 0.9f;
        result[i].distance = glm::distance(ray_position, intersection);
    }
    
    std::sort(std::begin(result), std::end(result), [](auto& a, auto& b) { return a.force < b.force; });
    
    for (size_t i = 0; i < PATHS_FOR_LISTENER; i++) {
        
        // step reverb force towards result
        if (float diff = listener_reverb_force[i] - result[i].force;
            diff < -0.01f || diff > 0.01f) {
            if (listener_reverb_force[i] < result[i].force) {
                listener_reverb_force[i] += 0.02f;
            } else {
                listener_reverb_force[i] -= 0.02f;
            }
        }
        
        // step reverb distance towards result
        if (float diff = listener_reverb_distance[i] - result[i].distance;
            diff < -0.05f || diff > 0.05f) {
            if (listener_reverb_distance[i] < result[i].distance) {
                listener_reverb_distance[i] += 0.1f;
            } else {
                listener_reverb_distance[i] -= 0.1f;
            }
        }
        
    }
}

void MakeSomeSourcePaths(PathFromAudioSource& path, vec3 source_position) {
    vec3 ray_direction = glm::normalize(vec3 {
        bell_distribution(generator),
        bell_distribution(generator),
        bell_distribution(generator)
    });
    
    vec3 ray_position = source_position;
    float force = 1.0f;
    
    for (size_t n = 0; n < SOURCE_DEPTH_LIMIT; n++) {
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