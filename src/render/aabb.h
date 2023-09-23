// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_RENDER_AABB_H
#define TRAM_SDK_RENDER_AABB_H

#include <framework/math.h>
#include <render/model.h>

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
    
aabbleaf_t InsertLeaf(RenderComponent* component, vec3 position, quat rotation);    
void RemoveLeaf(aabbleaf_t leaf);

QueryResponse FindNearestFromRay(vec3 ray_pos, vec3 ray_dir, uint32_t mask);

void DebugDrawTree();

}

#endif // TRAM_SDK_RENDER_AABB_H
