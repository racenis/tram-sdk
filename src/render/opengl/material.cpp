// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/opengl/material.h>

#include <config.h>

using namespace tram;

template<> tram::Pool<tram::Render::API::GLMaterial> tram::PoolProxy<tram::Render::API::GLMaterial>::pool("GLMaterial pool", RESOURCE_LIMIT_MATERIAL);

namespace tram::Render::API {
    
material_t MakeMaterial() {
    return {PoolProxy<GLMaterial>::New()};
}

void DeleteMaterial(material_t material) {
    PoolProxy<GLMaterial>::Delete(material.gl);
}

void SetMaterialTexture(material_t material, texturehandle_t texture) {
    material.gl->gl_texture = texture.gl_texture_handle;
}

void SetMaterialColor(material_t material, vec4 color) {
    material.gl->color = color;
}

void SetMaterialSpecularityWeight(material_t material, float weight) {
    material.gl->specularity_weight = weight;
}

void SetMaterialSpecularityExponent(material_t material, float exponent) {
    material.gl->specularity_exponent = exponent;
}

void SetMaterialSpecularityTransparency(material_t material, float transparency) {
    material.gl->specularity_transparency = transparency;
}


}