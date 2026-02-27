// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/opengl/material.h>

#include <config.h>

using namespace tram;

template<> tram::Pool<tram::Render::API::GLMaterial> tram::PoolProxy<tram::Render::API::GLMaterial>::pool("GLMaterial pool", RESOURCE_LIMIT_MATERIAL);

namespace tram::Render::API {
    
material_t MakeMaterial() {
    return {PoolProxy<GLMaterial>::New()};
}

void YeetMaterial(material_t material) {
    PoolProxy<GLMaterial>::Delete(material.gl);
}

void SetMaterialTexture(material_t material, texturehandle_t texture) {
    material.gl->gl_texture = texture.gl_texture_handle;
}

void SetMaterialNormalMap(material_t material, texturehandle_t texture) {
    material.gl->gl_normal_map = texture.gl_texture_handle;
}

void SetMaterialColor(material_t material, vec4 color) {
    material.gl->color = color;
}

void SetMaterialSpecularWeight(material_t material, float weight) {
    material.gl->specular_weight = weight;
}

void SetMaterialSpecularExponent(material_t material, float exponent) {
    material.gl->specular_exponent = exponent;
}

void SetMaterialSpecularTransparency(material_t material, float transparency) {
    material.gl->specular_transparency = transparency;
}

void SetMaterialReflectivity(material_t material, float reflectivity) {
    material.gl->reflectivity = reflectivity;
}

}