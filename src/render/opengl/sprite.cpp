// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/api.h>
#include <render/vertices.h>

namespace tram::Render::API {
    
spritearray_t CreateSpriteArray() {
    vertexarray_t vertex_array;
    CreateVertexArray(GetVertexDefinition(VERTEX_SPRITE), vertex_array);
    
    return {vertex_array};
}

void UpdateSpriteArray(spritearray_t array, size_t data_size, void* data) {
    SpritePoint* sprites = (SpritePoint*) data;
    
    std::vector<Render::SpriteVertex> vertices;
    
    for (size_t i = 0; i < data_size; i++) {
        const SpritePoint& sprite = sprites[i];

        vec2 top_left_voffset = {0, sprite.dimensions.y};
        vec2 top_right_voffset = {sprite.dimensions.x, sprite.dimensions.y};
        vec2 bottom_left_voffset = {0, 0};
        vec2 bottom_right_voffset = {sprite.dimensions.x, 0};
        
        // I guess that we could probably offload this computation to the GPU.
        // but that would mean that we have to change the vertex definition.
        // whatevs
        // TODO: fix this
        mat2 matrix;
        ((float*)(&matrix))[0] = cosf(sprite.rotation);
        ((float*)(&matrix))[1] = -sinf(sprite.rotation);
        ((float*)(&matrix))[2] = sinf(sprite.rotation);
        ((float*)(&matrix))[3] = cosf(sprite.rotation);
        
        top_left_voffset = matrix * (top_left_voffset - sprite.midpoint);
        top_right_voffset = matrix * (top_right_voffset - sprite.midpoint);
        bottom_left_voffset = matrix * (bottom_left_voffset - sprite.midpoint);
        bottom_right_voffset = matrix * (bottom_right_voffset - sprite.midpoint);
        
        Render::SpriteVertex top_left {
            .co =           sprite.position,
            .voffset =      top_left_voffset,
            .texco =        {sprite.texture_offset.x, sprite.texture_offset.y + sprite.texture_size.y},
            .color =        {1.0f, 1.0f, 1.0f},
            .verticality =  1.0f,
            .texture =      sprite.texture  
        };
        
        Render::SpriteVertex top_right {
            .co =           sprite.position,
            .voffset =      top_right_voffset,
            .texco =        {sprite.texture_offset.x + sprite.texture_size.x, sprite.texture_offset.y + sprite.texture_size.y},
            .color =        {1.0f, 1.0f, 1.0f},
            .verticality =  1.0f,
            .texture =      sprite.texture
        };
        
        Render::SpriteVertex bottom_left {
            .co =           sprite.position,
            .voffset =      bottom_left_voffset,
            .texco =        {sprite.texture_offset.x, sprite.texture_offset.y},
            .color =        {1.0f, 1.0f, 1.0f},
            .verticality =  1.0f,
            .texture =      sprite.texture
        };
        
        Render::SpriteVertex bottom_right {
            .co =           sprite.position,
            .voffset =      bottom_right_voffset,
            .texco =        {sprite.texture_offset.x + sprite.texture_size.x, sprite.texture_offset.y},
            .color =        {1.0f, 1.0f, 1.0f},
            .verticality =  1.0f,
            .texture =      sprite.texture
        };

        vertices.push_back(top_left);
        vertices.push_back(bottom_left);
        vertices.push_back(top_right);
        vertices.push_back(top_right);
        vertices.push_back(bottom_left);
        vertices.push_back(bottom_right);
    }
    
    UpdateVertexArray(array.vertex_array, sizeof(SpriteVertex) * vertices.size(), vertices.data());
}

}