// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/raytrace/sprite.h>

namespace tram::Render::API {

spritearray_t CreateSpriteArray() {
    return spritearray_t {.generic = nullptr};
}

void UpdateSpriteArray(spritearray_t array, size_t data_size, void* data) {
    
}

}