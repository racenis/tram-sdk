// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_OPENGL_SHADER_H
#define TRAM_SDK_RENDER_OPENGL_SHADER_H

#include <render/render.h>
#include <render/renderer.h>

namespace tram::Render::API {

typedef uint32_t shaderflags_t;

enum ShaderFlag : shaderflags_t {
    SHADER_NONE = 0,
    SHADER_SPECULAR = 1,
    SHADER_LIGHTMAP = 2,
};

void CompileShaders();

uint32_t FindShader(vertexformat_t format, materialtype_t type, shaderflags_t mask);
void RegisterShader(vertexformat_t format, materialtype_t type, const char* vertex_shader, const char* fragment_shader);

void BindUniformBlock (const char* name, uint32_t binding);

}

#endif // TRAM_SDK_RENDER_OPENGL_SHADER_H