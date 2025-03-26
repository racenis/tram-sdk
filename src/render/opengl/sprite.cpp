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
        
        // TODO: make rotation do something
        // TODO: make origin do something
        
        Render::SpriteVertex top_left {
            .co =           sprite.position,
            .voffset =      {sprite.dimensions.x * -0.5f, sprite.dimensions.y * 0.5f},
            .texco =        {sprite.texture_offset.x, sprite.texture_offset.y + sprite.texture_size.y},
            .color =        {1.0f, 1.0f, 1.0f},
            .verticality =  1.0f,
            .texture =      sprite.texture  
        };
        
        Render::SpriteVertex top_right {
            .co =           sprite.position,
            .voffset =      {sprite.dimensions.x * 0.5f, sprite.dimensions.y * 0.5f},
            .texco =        {sprite.texture_offset.x + sprite.texture_size.x, sprite.texture_offset.y + sprite.texture_size.y},
            .color =        {1.0f, 1.0f, 1.0f},
            .verticality =  1.0f,
            .texture =      sprite.texture
        };
        
        Render::SpriteVertex bottom_left {
            .co =           sprite.position,
            .voffset =      {sprite.dimensions.x * -0.5f, sprite.dimensions.y * -0.5f},
            .texco =        {sprite.texture_offset.x, sprite.texture_offset.y},
            .color =        {1.0f, 1.0f, 1.0f},
            .verticality =  1.0f,
            .texture =      sprite.texture
        };
        
        Render::SpriteVertex bottom_right {
            .co =           sprite.position,
            .voffset =      {sprite.dimensions.x * 0.5f, sprite.dimensions.y * -0.5f},
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
    
    UpdateVertexArray(array.vertex_array, sizeof(SpriteVertex) * vertices.size(), &vertices[0]);
}

}