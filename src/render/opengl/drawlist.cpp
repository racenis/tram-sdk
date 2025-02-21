// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/opengl/drawlist.h>

#include <render/opengl/light.h>
#include <render/opengl/shader.h>

#include <config.h>

using namespace tram;

template<> tram::Pool<tram::Render::API::GLDrawListEntry> tram::PoolProxy<tram::Render::API::GLDrawListEntry>::pool("GLDrawListEntry pool", 1000);

namespace tram::Render::API {
    
drawlistentry_t InsertDrawListEntry() {
    return {.generic = PoolProxy<GLDrawListEntry>::New()};
}

void RemoveDrawListEntry(drawlistentry_t entry) {
    PoolProxy<GLDrawListEntry>::Delete(entry.gl);
}

uint32_t GetFlags(drawlistentry_t entry) {
    return entry.gl->flags;
}

void SetFlags(drawlistentry_t entry, uint32_t flags) {
    entry.gl->flags = flags;
}

void SetLayer(drawlistentry_t entry, uint32_t layer) {
    entry.gl->layer = layer;
}

void SetPose(drawlistentry_t entry, Pose* pose) {
    entry.gl->pose = pose;
}

void SetLightmap(drawlistentry_t entry, texturehandle_t lightmap) {
    entry.gl->lightmap = lightmap.gl_texture_handle;
}

void SetDrawListAABB(drawlistentry_t entry, vec3 min, vec3 max) {
    entry.gl->aabb_min = min;
    entry.gl->aabb_max = max;
}

void SetDrawListColors(drawlistentry_t entry, size_t count, vec4* colors) {
    for (size_t i = 0; i < count; i++) {
        entry.gl->colors[i] = colors[i];
    }
}

void SetDrawListTextureOffsets(drawlistentry_t entry, size_t count, vec4* offset) {
    for (size_t i = 0; i < count; i++) {
        entry.gl->texture_transforms[i] = offset[i];
    }
}

void SetLights(drawlistentry_t entry, light_t* lights) {
    entry.gl->lights[0] = lights[0].gl ? PoolProxy<GLLight>::GetPool().index(lights[0].gl) : 0;
    entry.gl->lights[1] = lights[1].gl ? PoolProxy<GLLight>::GetPool().index(lights[1].gl) : 0;
    entry.gl->lights[2] = lights[2].gl ? PoolProxy<GLLight>::GetPool().index(lights[2].gl) : 0;
    entry.gl->lights[3] = lights[3].gl ? PoolProxy<GLLight>::GetPool().index(lights[3].gl) : 0;
}

void SetMatrix(drawlistentry_t entry, const mat4& matrix) {
    entry.gl->matrix = matrix;
}

void SetFadeDistance(drawlistentry_t entry, float near, float far) {
    entry.gl->fade_near = near;
    entry.gl->fade_far = far;
}

void SetDrawListVertexArray(drawlistentry_t entry, vertexarray_t vertex_array_handle) {
    entry.gl->vao = vertex_array_handle.gl_vertex_array;
}

void SetDrawListIndexArray(drawlistentry_t entry, indexarray_t index_array_handle) {
    // the index array is already bound to the vao
}

void SetDrawListSpriteArray(drawlistentry_t entry, spritearray_t sprite_array_handle) {
    SetDrawListVertexArray(entry, sprite_array_handle.vertex_array);
}

void SetDrawListIndexRange(drawlistentry_t entry, uint32_t index_offset, uint32_t index_length) {
    entry.gl->eboOff = index_offset;
    entry.gl->eboLen = index_length;
}

void SetDrawListShader(drawlistentry_t entry, vertexformat_t vertex_format, materialtype_t material_type) {
    entry.gl->shader = FindShader(vertex_format, material_type, SHADER_NONE);
}

void SetDrawListMaterials(drawlistentry_t entry, size_t material_count, material_t* materials) {
    for (size_t i = 0; i < material_count; i++) {
        entry.gl->materials[i] = materials[i].gl;
    }
    entry.gl->texCount = material_count;
}

}