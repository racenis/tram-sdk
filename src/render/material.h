// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_MATERIAL_H
#define RENDER_MATERIAL_H

#include <render/render.h>

namespace tram::Render {
    
class Material : public Resource {
public:
    Material(name_t name, materialtype_t type) : Resource(name), type(type) {}
    
    inline texturehandle_t GetTexture() const { return texture; }
    inline uint32_t GetWidth() const { return width; }
    inline uint32_t GetHeight() const { return height; }
    inline materialtype_t GetType() const { return type; }

    void MakePattern (vec3 color1, vec3 color2);
    
    void LoadFromDisk();
    void LoadFromMemory();
    
    void Unload() {}
    
    static Material* Find (name_t name);
    static Material* Make (name_t name, materialtype_t type);
    
    static void LoadMaterialInfo (const char* filename);
    
protected:
    texturehandle_t texture = 0;
    materialtype_t type = MATERIAL_TEXTURE;
    
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t channels = 0;
    uint8_t* textureData = nullptr;
    size_t approx_vram_usage = 0;
    
    friend class Sprite;
};

}

#endif // RENDER_MATERIAL_H