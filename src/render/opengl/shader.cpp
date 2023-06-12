// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#ifdef __EMSCRIPTEN__
    #include <GL/gl.h>
	#include <GLES3/gl3.h>
#else
    #include <glad.h>
#endif

#include <framework/logging.h>

#include <platform/file.h>

#include <render/opengl/renderer.h>

namespace tram::Render::OpenGL {

struct VertexShader {
    name_t name;
    uint32_t compiled_shader;
};

struct FragmentShader {
    name_t name;
    uint32_t compiled_shader;
};    

struct LinkedShader {
    vertexformat_t vertex_format;
    materialtype_t material_type;
    
    uint32_t linked_shader;
    
    name_t vertex_shader;
    name_t fragment_shader;
};

const uint32_t MAX_MATERIAL_TYPES = 10;
const uint32_t MAX_VERTEX_FORMATS = 10;

const uint32_t MAX_VERTEX_SHADERS = 20;
const uint32_t MAX_FRAGMENT_SHADERS = 20;
const uint32_t MAX_LINKED_SHADERS = 40;

static VertexShader vertex_shaders[MAX_VERTEX_SHADERS];
static FragmentShader fragment_shaders[MAX_FRAGMENT_SHADERS];

static size_t last_vertex_shader = 0;
static size_t last_fragment_shader = 0;
static size_t last_linked_shader = 0;

static LinkedShader linked_shaders[MAX_LINKED_SHADERS];

static uint32_t linked_shader_lookup_table[MAX_VERTEX_FORMATS][MAX_MATERIAL_TYPES] = {0};

#ifdef __EMSCRIPTEN__
    const char* shader_path = "shaders/gles3/";
#else
    const char* shader_path = "shaders/opengl3/";
#endif

uint32_t LoadVertexShader(name_t name) {
    for (size_t i = 0; i < last_vertex_shader; i++) {
        if (vertex_shaders[i].name == name) {
            return vertex_shaders[i].compiled_shader;
        }
    }
    
    assert(last_vertex_shader < MAX_VERTEX_SHADERS);
    
    char path[256];
    strcpy(path, shader_path);
    strcat(path, name);
    strcat(path, ".vert");
    
    FileReader file(path);
    
    if (!file.is_open()) {
        Log(SEVERITY_ERROR, System::SYSTEM_RENDER, "Can't find vertex shader source file {}!", UID(path));
    }

    uint32_t compiled_program = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(compiled_program, 1, &file.contents, NULL);
    glCompileShader(compiled_program);

    int32_t compile_status;
    glGetShaderiv(compiled_program, GL_COMPILE_STATUS, &compile_status);
    
    if (!compile_status) {
        char compile_error[420];
        glGetShaderInfoLog(compiled_program, 420, NULL, compile_error);
        
        Log(SEVERITY_ERROR, System::SYSTEM_RENDER, "Vertex shader {} compile error:\n{}", UID(name), UID(compile_error));
        
        abort();
    }
    
    vertex_shaders[last_vertex_shader].name = name;
    vertex_shaders[last_vertex_shader].compiled_shader = compiled_program;

    last_vertex_shader++;

    return compiled_program;   
}

uint32_t LoadFragmentShader(name_t name) {
    for (size_t i = 0; i < last_fragment_shader; i++) {
        if (fragment_shaders[i].name == name) {
            return fragment_shaders[i].compiled_shader;
        }
    }
    
    assert(last_fragment_shader < MAX_FRAGMENT_SHADERS);
    
    char path[256];
    strcpy(path, shader_path);
    strcat(path, name);
    strcat(path, ".frag");
    
    FileReader file(path);
    
    if (!file.is_open()) {
        Log(SEVERITY_ERROR, System::SYSTEM_RENDER, "Can't find fragment shader source file {}!", UID(path));
    }

    uint32_t compiled_program = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(compiled_program, 1, &file.contents, NULL);
    glCompileShader(compiled_program);

    int32_t compile_status;
    glGetShaderiv(compiled_program, GL_COMPILE_STATUS, &compile_status);
    
    if (!compile_status) {
        char compile_error[420];
        glGetShaderInfoLog(compiled_program, 420, NULL, compile_error);
        
        Log(SEVERITY_ERROR, System::SYSTEM_RENDER, "Fragment shader {} compile error:\n{}", UID(name), UID(compile_error));
        
        abort();
    }

    fragment_shaders[last_fragment_shader].name = path;
    fragment_shaders[last_fragment_shader].compiled_shader = compiled_program;

    last_fragment_shader++;

    return compiled_program;
}

uint32_t LinkShader(uint32_t vertex_shader, uint32_t fragment_shader) {
    assert(vertex_shader);
    assert(fragment_shader);
    
    uint32_t linked_shader = glCreateProgram();
    glAttachShader(linked_shader, vertex_shader);
    glAttachShader(linked_shader, fragment_shader);
    glLinkProgram(linked_shader);

    int32_t link_status;
    glGetProgramiv(linked_shader, GL_LINK_STATUS, &link_status);
    
    if (!link_status) {
        char link_error[420];
        glGetShaderInfoLog(linked_shader, 420, NULL, link_error);
        
        Log(SEVERITY_ERROR, System::SYSTEM_RENDER, "Shader link error:\n{}", UID(link_error));
        
        abort();
    }
    
    glUseProgram(linked_shader);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[0]"), 0);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[1]"), 1);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[2]"), 2);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[3]"), 3);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[4]"), 4);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[5]"), 5);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[6]"), 6);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[7]"), 7);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[8]"), 8);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[9]"), 9);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[10]"), 10);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[11]"), 11);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[12]"), 12);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[13]"), 13);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[14]"), 14);
    glUniform1i(glGetUniformLocation(linked_shader, "sampler[15]"), 15);
    glUseProgram(0);

    return linked_shader;
}

void BindTextures(uint32_t shader) {        

}

void BindUniformBlock(const char* name, uint32_t binding) {
    for (size_t i = 0; i < last_linked_shader; i++) {
        uint32_t shader_name = linked_shaders[i].linked_shader;
        
        auto block_index = glGetUniformBlockIndex(shader_name, name);
        
        if (block_index == GL_INVALID_INDEX) continue;
        
        glUniformBlockBinding(shader_name, block_index, binding);
    }
}

void RegisterShader(vertexformat_t format, materialtype_t type, const char* vertex_shader, const char* fragment_shader) {
    for (size_t i = 0; i < last_linked_shader; i++) {
        if (linked_shaders[i].vertex_shader == vertex_shader && linked_shaders[i].fragment_shader == fragment_shader) {
            linked_shader_lookup_table[format][type] = i;
            return;
        }
    }
    
    assert(last_linked_shader < MAX_LINKED_SHADERS);
    
    uint32_t vertex = LoadVertexShader(vertex_shader);
    uint32_t fragment = LoadFragmentShader(fragment_shader);
    
    uint32_t linked = LinkShader(vertex, fragment);
    
    linked_shaders[last_linked_shader].material_type = type;
    linked_shaders[last_linked_shader].vertex_format = format;
    
    linked_shaders[last_linked_shader].linked_shader = linked;
    
    linked_shaders[last_linked_shader].vertex_shader = vertex_shader;
    linked_shaders[last_linked_shader].fragment_shader = fragment_shader;
    
    linked_shader_lookup_table[format][type] = last_linked_shader;
    
    last_linked_shader++;
}

uint32_t FindShader(vertexformat_t format, materialtype_t type) {
    uint32_t shader_index = linked_shader_lookup_table[format][type];
    LinkedShader& shader = linked_shaders[shader_index];
    
    if (shader.material_type != type || shader.vertex_format != format) {
        Log(SEVERITY_ERROR, System::SYSTEM_RENDER, 
            "Can't find shader for combination of {} and {}!",
            GetVertexFormatName(format), GetMaterialTypeName(type)
        );
    }
    
    return shader.linked_shader;
}

void CompileShaders(){
    std::cout << "loading shaders" << std::endl;

    RegisterShader(VERTEX_STATIC,   MATERIAL_TEXTURE,          "normal_static",     "normal_static");
    RegisterShader(VERTEX_STATIC,   MATERIAL_TEXTURE_ALPHA,    "normal_static",     "normal_static_alpha");
    RegisterShader(VERTEX_DYNAMIC,  MATERIAL_TEXTURE,          "normal_dynamic",    "normal_dynamic");
    RegisterShader(VERTEX_DYNAMIC,  MATERIAL_TEXTURE_ALPHA,    "normal_dynamic",    "normal_dynamic_alpha");
    RegisterShader(VERTEX_STATIC,   MATERIAL_WATER,            "normal_static",     "normal_water");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_TEXTURE_ALPHA,    "normal_sprite",     "normal_sprite");
    RegisterShader(VERTEX_LINE,     MATERIAL_FLAT_COLOR,       "line",              "line");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_MSDF,             "text",              "text");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_GLYPH,            "glyph",             "glyph");
    
    for (size_t i = 0; i < last_linked_shader; i++) {
        //BindTextures(linked_shaders[i].linked_shader);
    }
}

}
