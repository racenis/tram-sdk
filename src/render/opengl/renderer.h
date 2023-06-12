// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_OPENGL_RENDERER_H
#define RENDER_OPENGL_RENDERER_H

#include <render/render.h>
#include <render/renderer.h>

namespace tram::Render::OpenGL {
    
void CompileShaders();

uint32_t FindShader(vertexformat_t format, materialtype_t type);

void BindUniformBlock (const char* name, uint32_t binding);

struct LightListEntry {
    vec3 location = {0.0f, 0.0f, 0.0f};
    float padding;
    vec3 color = {0.0f, 0.0f, 0.0f};
    float distance = 0.0f;
    float padding2[8];
};

struct DrawListEntry {
    uint32_t flags = 0;

    Pose* pose = nullptr;

    glm::vec3 location = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

    uint32_t lights[4] = {0};
    
    uint32_t layer = 0;
    uint32_t lightmap = 0;
    uint32_t vao = 0;
    uint32_t eboLen = 0;
    uint32_t eboOff = 0;
    uint32_t shader = 0;
    uint32_t texCount = 0;
    uint32_t textures[15] = {0};
    
    char padding [4];


    /// Assembles a key for sorting
    uint64_t CalcSortKey (const glm::vec3& cameraPosition) {
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

#endif // RENDER_OPENGL_RENDERER_H