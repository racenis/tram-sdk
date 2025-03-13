// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_OPENGL_DRAWLIST_H
#define TRAM_SDK_RENDER_OPENGL_DRAWLIST_H

#include <render/render.h>
#include <render/renderer.h>

#include <render/opengl/material.h>

namespace tram::Render::API {

struct GLDrawListEntry {
    uint32_t flags = 0;

    Pose* pose = nullptr;
    
    mat4 matrix = mat4(1.0f);

    uint32_t lights[4] = {0};
    
    uint32_t layer = 0;
    uint32_t lightmap = 0;
    uint32_t environmentmap = 0;
    uint32_t vao = 0;
    uint32_t eboLen = 0; // in primitive count
    uint32_t eboOff = 0; // in primitive count 
    uint32_t shader = 0;
    uint32_t texCount = 0;
    vec4 colors[15];
    vec4 texture_transforms[15];
    GLMaterial* materials[15];
    
    vec3 aabb_min = {0.0f, 0.0f, 0.0f};
    vec3 aabb_max = {0.0f, 0.0f, 0.0f};
    
    float fade_near = 0.0f;
    float fade_far = INFINITY;

    sphericalharmonic_t harmonic;

    GLDrawListEntry() {
        for (int i = 0; i < 15; i++) {
            materials[i] = nullptr;
            colors[i] = {1.0f, 1.0f, 1.0f, 1.0f};
            texture_transforms[i]  = {0.0f, 0.0f, 0.0f, 0.0f};
        }
        
        harmonic = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
    }

    /// Assembles a key for sorting
    uint64_t CalcSortKey (const vec3& cameraPosition) {
        vec3 location = matrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
        uint64_t sortkey = 0;
        sortkey = flags & FLAG_TRANSPARENT ? 1 : 0;
        sortkey = sortkey << 60;
        sortkey = sortkey | (((uint64_t)layer) << 61);    // 3 bits for the layer number
        sortkey = sortkey | (((uint64_t)shader) << 48);   // 12 bits for the shader
        sortkey = sortkey | (((uint64_t)vao) << 32);      // 16 bits or the vertex array number
        // TODO: reverse the distance if FLAG_REVERSE_SORT is set
        // also i think that the bitmask for the distance thing is incorrect
        sortkey = sortkey | (((uint64_t)(glm::distance(cameraPosition, location) * 3000000.0f)) & 0x00000000FFFFFFFF); // 32 bits for the distance
        return sortkey;
    }
};

}

#endif // TRAM_SDK_RENDER_OPENGL_DRAWLIST_H