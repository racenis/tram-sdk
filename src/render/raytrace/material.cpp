// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/raytrace/material.h>

#include <config.h>

namespace tram::Render::API {

static Pool<RTTexture> texture_list("texture list", RESOURCE_LIMIT_MATERIAL);
static Pool<RTMaterial> material_list("material list", RESOURCE_LIMIT_MATERIAL);

material_t MakeMaterial() {
    return material_t{.rt = material_list.AddNew()};
}

void YeetMaterial(material_t material) {
    material_list.Remove(material.rt);
}

void SetMaterialTexture(material_t material, texturehandle_t texture) {
    material.rt->texture = texture.rt_texture;
}

void SetMaterialNormalMap(material_t material, texturehandle_t texture) {
    material.rt->normal_map = texture.rt_texture;
}

void SetMaterialColor(material_t material, vec4 color) {
    material.rt->color = color;
}

void SetMaterialSpecularWeight(material_t material, float weight) {
    material.rt->specular_weight = weight;
}

void SetMaterialSpecularExponent(material_t material, float exponent) {
    material.rt->specular_exponent = exponent;
}

void SetMaterialSpecularTransparency(material_t material, float transparency) {
    material.rt->specular_transparency = transparency;
}

void SetMaterialReflectivity(material_t material, float reflectivity) {
    material.rt->reflectivity = reflectivity;
}

texturehandle_t CreateTexture(ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data) {
    RTTexture* texture = texture_list.AddNew();
    
    texture->width = width;
    texture->height = height;
    
    switch (color_mode) {
        case COLORMODE_R:       texture->channels = 1;  break;
        case COLORMODE_RG:      texture->channels = 2;  break;
        case COLORMODE_RGB:     texture->channels = 3;  break;
        case COLORMODE_RGBA:    texture->channels = 4;  break;
    }
    
    switch (texture_filter) {
        case TEXTUREFILTER_NEAREST:             texture->mode = RT_NEAREST; break;
        case TEXTUREFILTER_LINEAR:              texture->mode = RT_BLENDED; break;
        case TEXTUREFILTER_LINEAR_MIPMAPPED:    texture->mode = RT_BLENDED; break;
    }
    
    int texture_size = texture->width * texture->height * sizeof(vec4);
    texture->pixels = (vec4*)malloc(texture_size);
    uint8_t* pix = (uint8_t*)data;
    //memcpy(texture->pixels, data, texture_size);
    for (int i = 0; i < texture->width * texture->height; i++) {
        vec4 color = {0.0f, 0.0f, 0.0f, 255.0f};
        
        int offset = i * texture->channels;
        
        switch (color_mode) {
            case COLORMODE_R:       color.x = pix[offset + 0];      break;
            case COLORMODE_RG:      color.x = pix[offset + 0]; color.y = pix[offset + 1];    break;
            case COLORMODE_RGB:     color.x = pix[offset + 0]; color.y = pix[offset + 1]; color.z = pix[offset + 2];  break;
            case COLORMODE_RGBA:    color.x = pix[offset + 0]; color.y = pix[offset + 1]; color.z = pix[offset + 2]; color.w = pix[offset + 3];  break;
        }
        
        
        texture->pixels[i] = color / 255.0f;
    }
    
    return texturehandle_t {.rt_texture = texture};
}

}