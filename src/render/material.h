// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_RENDER_MATERIAL_H
#define TRAM_SDK_RENDER_MATERIAL_H

#include <render/render.h>

namespace tram::Render {

enum MaterialProperty {
    PROPERTY_METAL,
    PROPERTY_METAL_THIN,
    PROPERTY_SLIME,
    PROPERTY_TILE,
    PROPERTY_GRATE,
    PROPERTY_WOOD,
    PROPERTY_COMPUTER,
    PROPERTY_GLASS,
    PROPERTY_SNOW,
    PROPERTY_GRASS,
    PROPERTY_CONCRETE,
    PROPERTY_FLESH
};    

class Material : public Resource {
public:
    Material(name_t name) : Resource(name) {}
    Material(name_t name, materialtype_t type) : Resource(name), type(type) {}
    Material(name_t name, materialtype_t type, MaterialFilter filter) : Resource(name), type(type), filter(filter) {}
    Material(name_t name, materialtype_t type, MaterialFilter filter, MaterialProperty property) : Resource(name), type(type), filter(filter), property(property) {}
    
    inline texturehandle_t GetTexture() const { return texture; }
    inline uint32_t GetWidth() const { return width; }
    inline uint32_t GetHeight() const { return height; }
    inline materialtype_t GetType() const { return type; }
    inline MaterialProperty GetProperty() const { return property; }

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
    MaterialFilter filter = FILTER_NEAREST;
    MaterialProperty property = PROPERTY_METAL;
    
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t channels = 0;
    uint8_t* texture_data = nullptr;
    size_t approx_vram_usage = 0;
    
    friend class Sprite;
};

}

#endif // TRAM_SDK_RENDER_MATERIAL_H