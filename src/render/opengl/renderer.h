// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_RENDER_OPENGL_RENDERER_H
#define TRAM_SDK_RENDER_OPENGL_RENDERER_H

#include <render/render.h>
#include <render/renderer.h>

namespace tram::Render::API {
    
void CompileShaders();

uint32_t FindShader(vertexformat_t format, materialtype_t type);
void RegisterShader(vertexformat_t format, materialtype_t type, const char* vertex_shader, const char* fragment_shader);

void BindUniformBlock (const char* name, uint32_t binding);



struct GLLight {
    vec3 location = {0.0f, 0.0f, 0.0f};
    float padding;
    vec3 color = {0.0f, 0.0f, 0.0f};
    float distance = 0.0f;
    vec3 direction = {0.0f, 0.0f, -1.0f};
    float exponent = 0.0f;
    float padding2[4];
};

struct GLDrawListEntry {
    uint32_t flags = 0;

    Pose* pose = nullptr;
    
    mat4 matrix = mat4(1.0f);

    uint32_t lights[4] = {0};
    
    uint32_t layer = 0;
    uint32_t lightmap = 0;
    uint32_t vao = 0;
    uint32_t eboLen = 0;
    uint32_t eboOff = 0;
    uint32_t shader = 0;
    uint32_t texCount = 0;
    uint32_t textures[15] = {0};
    vec4 colors[15];
    float specular_weights[15];
    float specular_powers[15];
    
    char padding [4];

    GLDrawListEntry() {
        for (int i = 0; i < 15; i++) {
            colors[i] = {1.0f, 1.0f, 1.0f, 1.0f};
            specular_weights[i] = 0.0f;
            specular_powers[i] = 1.0f;
        }
    }

    /// Assembles a key for sorting
    uint64_t CalcSortKey (const glm::vec3& cameraPosition) {
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

#endif // TRAM_SDK_RENDER_OPENGL_RENDERER_H