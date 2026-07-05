// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/raytrace/material.h>
#include <render/api.h>

#include <config.h>

namespace tram::Render::API {

static Pool<RTTexture> texture_list("texture list", RESOURCE_LIMIT_MATERIAL);
static Pool<RTMaterial> material_list("material list", RESOURCE_LIMIT_MATERIAL);

material_t MakeMaterial() {
    return material_t{.rt = material_list.make()};
}

void YeetMaterial(material_t material) {
    material_list.yeet(material.rt);
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
    texturehandle_t texture = {.rt_texture = texture_list.make()};
    SetTextureImage(texture, color_mode, width, height, data);
    SetTextureFilter(texture, texture_filter);
    return texture;
}

void SetTextureFilter(texturehandle_t texture, TextureFilter texture_filter) {
    switch (texture_filter) {
        case TEXTUREFILTER_NEAREST:             texture.rt_texture->mode = RT_NEAREST;  break;
        case TEXTUREFILTER_LINEAR:              texture.rt_texture->mode = RT_BLENDED;  break;
        case TEXTUREFILTER_LINEAR_MIPMAPPED:    texture.rt_texture->mode = RT_BLENDED;  break;
    }
}

void SetTextureImage(texturehandle_t texture, ColorMode color_mode, uint32_t width, uint32_t height, void* data) {
    texture.rt_texture->width = width;
    texture.rt_texture->height = height;
    
    switch (color_mode) {
        case COLORMODE_R:       texture.rt_texture->channels = 1;   break;
        case COLORMODE_RG:      texture.rt_texture->channels = 2;   break;
        case COLORMODE_RGB:     texture.rt_texture->channels = 3;   break;
        case COLORMODE_RGBA:    texture.rt_texture->channels = 4;   break;
    }
    
    if (texture.rt_texture->pixels) free(texture.rt_texture->pixels);
    int texture_size = width * height * sizeof(vec4);
    texture.rt_texture->pixels = (vec4*)malloc(texture_size);
    uint8_t* pix = (uint8_t*)data;
    for (unsigned i = 0; i < width * height; i++) {
        vec4 color = {0.0f, 0.0f, 0.0f, 255.0f};
        
        int offset = i * texture.rt_texture->channels;
        
        switch (color_mode) {
            case COLORMODE_R:       color.x = pix[offset + 0];      break;
            case COLORMODE_RG:      color.x = pix[offset + 0]; color.y = pix[offset + 1];    break;
            case COLORMODE_RGB:     color.x = pix[offset + 0]; color.y = pix[offset + 1]; color.z = pix[offset + 2];  break;
            case COLORMODE_RGBA:    color.x = pix[offset + 0]; color.y = pix[offset + 1]; color.z = pix[offset + 2]; color.w = pix[offset + 3];  break;
        }
        
        
        texture.rt_texture->pixels[i] = color / 255.0f;
    }
}

void YeetTexture(texturehandle_t texture) {
    free(texture.rt_texture->pixels);
    texture_list.yeet(texture.rt_texture);
}

texturearray_t CreateTexture(ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, uint32_t layers, void* data) {
    return texturearray_t {.rt_texture = CreateTexture(color_mode, texture_filter, width, height, data).rt_texture};
}

void SetTextureFilter(texturearray_t texture, TextureFilter texture_filter) {
    SetTextureFilter(texturehandle_t {.rt_texture = texture.rt_texture}, texture_filter);
}

void SetTextureImage(texturearray_t texture, ColorMode color_mode, uint32_t width, uint32_t height, uint32_t layers, void* data) {
    SetTextureImage(texturehandle_t {.rt_texture = texture.rt_texture}, color_mode, width, height, data);
}

void YeetTexture(texturearray_t texture) {
    YeetTexture(texturehandle_t {.rt_texture = texture.rt_texture});
}

}