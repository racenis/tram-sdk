// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_SCENE_H
#define TRAM_SDK_RENDER_SCENE_H

#include <framework/math.h>
#include <render/model.h>

#include <functional>

namespace tram {
    class RenderComponent;
}

namespace tram::Render {
    typedef void* aabbleaf_t;
}

namespace tram::Render::AABB {

enum ReferenceType : uint32_t {
    REFERENCE_RENDERCOMPONENT = 1
};
    
struct QueryResponse {
    ReferenceType type;
    void* data;
    
    vec3 intersection;
    AABBTriangle triangle;
};
    
aabbleaf_t InsertLeaf(RenderComponent* component, vec3 position, quat rotation, vec3 scale);    
void RemoveLeaf(aabbleaf_t leaf);

QueryResponse FindNearestFromRay(vec3 ray_pos, vec3 ray_dir, uint32_t mask);

void FindAllIntersectionsFromAABB(vec3 min, vec3 max, std::function<void(ReferenceType, EntityComponent*)> callback);

void DebugDrawTree();

}

namespace tram::Render::LightTree {
    
void AddLight(light_t light, vec3 pos, float dist);
void FindLights(vec3 position, light_t* dest);
void RemoveLight(light_t light);
    
}

#endif // TRAM_SDK_RENDER_SCENE_H
