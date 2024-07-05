// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_RENDER_DIRECT3D_DIRECT3D_H
#define TRAM_SDK_RENDER_DIRECT3D_DIRECT3D_H

#include <render/render.h>
#include <render/renderer.h>

namespace tram::Render::API {
    struct D3DDrawListEntry {
        uint32_t flags = 0;

        Pose* pose = nullptr;
        
        mat4 matrix = mat4(1.0f);

        uint32_t lights[4] = {0};
        
        IDirect3DVertexBuffer9* vertex_buffer = nullptr;
        IDirect3DIndexBuffer9* index_buffer = nullptr;
        
        uint32_t vertex_count = 0;
        uint32_t index_length = 0;
        uint32_t index_offset = 0;
        
        uint32_t fvf = 0;
        
        uint32_t layer = 0;
        IDirect3DTexture9* lightmap = nullptr;
        IDirect3DTexture9* texture = nullptr;

        vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
        
        float specular_weight = 0.0f;
        float specular_exponent = 1.0f;
        float specular_transparency = 1.0f;
    };
    
    struct D3DLight {
        vec3 location = {0.0f, 0.0f, 0.0f};
        vec3 color = {0.0f, 0.0f, 0.0f};
        float distance = 0.0f;
        vec3 direction = {0.0f, 0.0f, -1.0f};
        float exponent = 0.0f;
    };
}

#endif // TRAM_SDK_RENDER_DIRECT3D_DIRECT3D_H