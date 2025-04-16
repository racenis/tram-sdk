// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/raytrace/drawlist.h>

template<> tram::Pool<tram::Render::API::RTDrawListEntry> tram::PoolProxy<tram::Render::API::RTDrawListEntry>::pool("render list", 500);

namespace tram::Render::API {

drawlistentry_t InsertDrawListEntry() {
    return drawlistentry_t {.rt = PoolProxy<RTDrawListEntry>::New()};
}

void RemoveDrawListEntry(drawlistentry_t entry) {
    PoolProxy<RTDrawListEntry>::Delete(entry.rt);
}

uint32_t GetFlags(drawlistentry_t entry) {
    return entry.rt->flags;
}

void SetFlags(drawlistentry_t entry, uint32_t flags) {
    entry.rt->flags = flags;
}

void SetLayer(drawlistentry_t entry, uint32_t layer) {
    entry.rt->layer = layer;
}

void SetPose(drawlistentry_t entry, Pose* pose) {
    entry.rt->pose = pose;
}

void SetLightmap(drawlistentry_t entry, texturehandle_t lightmap) {
    entry.rt->lightmap = lightmap.rt_texture;
}

void SetEnvironmentMap(drawlistentry_t entry, texturehandle_t environmentmap) {
    entry.rt->environmentmap = environmentmap.rt_texture;
}

void SetSphericalHarmonic(drawlistentry_t entry, sphericalharmonic_t harmonic) {
    entry.rt->harmonic = harmonic;
}

void SetDrawListAABB(drawlistentry_t entry, vec3 min, vec3 max) {
    //entry.sw->aabb_min = min;
    //entry.sw->aabb_max = max;
}

void SetDrawListColors(drawlistentry_t entry, size_t count, vec4* color) {
    entry.rt->color = *color;
}

void SetDrawListTextureOffsets(drawlistentry_t entry, size_t count, vec4* offset) {
    /*for (size_t i = 0; i < count; i++) {
        entry.sw->texture_transforms[i] = offset[i];
    }*/
}


void SetDrawListSpecularities(drawlistentry_t entry, size_t count, float* weight, float* exponent, float* transparency) {
    entry.rt->specular_weight = *weight;
    entry.rt->specular_exponent = *exponent;
    entry.rt->specular_transparency = *transparency;
}

void SetLights(drawlistentry_t entry, light_t* lights) {
    for (size_t i = 0; i < 4; i++) {
        entry.rt->lights[i] = lights[i].rt;
    }
}

void SetMatrix(drawlistentry_t entry, const mat4& matrix) {
    entry.rt->matrix = matrix;
}

void SetFadeDistance(drawlistentry_t entry, float near, float far) {
    //entry.sw->fade_near = near;
    //entry.sw->fade_far = far;
}

void SetDrawListVertexArray(drawlistentry_t entry, vertexarray_t vertex_array_handle) {
    entry.rt->vertex_array = vertex_array_handle.rt_vertex_array;
}

void SetDrawListSpriteArray(drawlistentry_t entry, spritearray_t sprite_array_handle) {
    
}

void SetDrawListIndexArray(drawlistentry_t entry, indexarray_t index_array_handle) {
    entry.rt->index_array = index_array_handle.rt_index_array;
}

void SetDrawListIndexRange(drawlistentry_t entry, uint32_t index_offset, uint32_t index_length) {
    entry.rt->index_offset = index_offset;
    entry.rt->index_length = index_length;
}

bool SetDrawListShader(drawlistentry_t entry, vertexformat_t vertex_format, materialtype_t material_type) {
    // TODO: check if material_type has transparency and save that fact
    return true;
}

void SetDrawListMaterials(drawlistentry_t entry, size_t texture_count, material_t* material) {
    entry.rt->material = material->rt;
}
    
}