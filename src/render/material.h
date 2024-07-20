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

enum TextureType {
    TEXTURE_NONE,
    TEXTURE_SAME,
    TEXTURE_SOURCE
};

class Material : public Resource {
public:
    Material(name_t name) : Resource(name) {}
    
    inline texturehandle_t GetTexture() const { return texture; }
    inline uint32_t GetWidth() const { return width; }
    inline uint32_t GetHeight() const { return height; }
    inline materialtype_t GetType() const { return type; }
    inline MaterialProperty GetProperty() const { return property; }
    inline vec3 GetColor() const { return color; }
    inline float GetSpecularWeight() const { return specular_weight; }
    inline float GetSpecularExponent() const { return specular_exponent; }
    inline float GetSpecularTransparency() const { return specular_transparency; }

    void SetMaterialType(materialtype_t type) { this->type = type; }
    void SetMaterialFilter(MaterialFilter filter) { this->filter = filter; }
    void SetMaterialProperty(MaterialProperty property) { this->property = property; }
    void SetColor(color_t color) { this->color = color; }
    void SetSpecular(float weight, float exponent,  float transparency) { specular_weight = weight; specular_exponent = exponent; specular_transparency = transparency; }
    void SetTextureType(TextureType texture_type) { this->texture_type = texture_type; }
    void SetSource(Material* source) { this->source = source; }
    
    void MakePattern(vec3 color1, vec3 color2);
    
    void LoadFromDisk();
    void LoadFromMemory();
    
    void Unload() {}
    
    static Material* Find(name_t name);
    static Material* Make(name_t name, materialtype_t type);
    
    static void LoadMaterialInfo(const char* filename);
    
protected:
    texturehandle_t texture = {.generic = 0};
    materialtype_t type = MATERIAL_TEXTURE;
    MaterialFilter filter = FILTER_NEAREST;
    MaterialProperty property = PROPERTY_METAL;
    TextureType texture_type = TEXTURE_SAME;
    
    Material* source = nullptr;
    
    vec3 color = {1.0f, 1.0f, 1.0f};
    float specular_weight = 0.0f;
    float specular_exponent = 0.0f;
    float specular_transparency = 0.0f;
    
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t channels = 0;
    uint8_t* texture_data = nullptr;
    size_t approx_vram_usage = 0;
    
    friend class Sprite;
};

}

#endif // TRAM_SDK_RENDER_MATERIAL_H