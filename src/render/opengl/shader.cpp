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

#include <render/opengl/renderer.h>

namespace tram::Render::OpenGL {

struct VertexShader {
    const char* name = nullptr;
    uint32_t compiled_shader;
};

struct FragmentShader {
    const char* name = nullptr;
    uint32_t compiled_shader;
};    

struct LinkedShader {
    vertexformat_t vertex_format;
    materialtype_t material_type;
    
    uint32_t linked_shader;
    
    const char* vertex_shader = nullptr;
    const char* fragment_shader = nullptr;
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

uint32_t LoadVertexShader(const char* path) {
    for (size_t i = 0; i < last_vertex_shader; i++) {
        if (strcmp(vertex_shaders[i].name, path) == 0) {
            return vertex_shaders[i].compiled_shader;
        }
    }
    
    assert(last_vertex_shader < MAX_VERTEX_SHADERS);
    
    // TODO: swap out this stringstream loader for platform loader.
    
    std::ifstream file;
    std::stringstream filestream;
    std::string shadercode;
    const char* shadercode_c;
    int32_t status;
    char compilemsg[419];
    uint32_t compiled_program;

    assert(path);

    file.open(path);
    if(!file.is_open()){
        std::cout << "Can't find shader: " << path << std::endl;
        abort();
    }

    filestream << file.rdbuf();
    shadercode = filestream.str();

    file.close();

    shadercode_c = shadercode.c_str();

    compiled_program = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(compiled_program, 1, &shadercode_c, NULL);
    glCompileShader(compiled_program);


    glGetShaderiv(compiled_program, GL_COMPILE_STATUS, &status);
    if (!status){
        glGetShaderInfoLog(compiled_program, 419, NULL, compilemsg);
        std::cout << "Vertex shader " << path << " compile error:" << std::endl << compilemsg << std::endl;
        abort();
    }
    
    vertex_shaders[last_vertex_shader].name = path;
    vertex_shaders[last_vertex_shader].compiled_shader = compiled_program;

    last_vertex_shader++;

    return compiled_program;   
}

uint32_t LoadFragmentShader(const char* path) {
    for (size_t i = 0; i < last_fragment_shader; i++) {
        if (strcmp(fragment_shaders[i].name, path) == 0) {
            return fragment_shaders[i].compiled_shader;
        }
    }
    
    assert(last_fragment_shader < MAX_FRAGMENT_SHADERS);
    
    std::ifstream file;
    std::stringstream filestream;
    std::string shadercode;
    const char* shadercode_c;
    int32_t status;
    char compilemsg[419];
    uint32_t compiled_program;

    assert(path);

    file.open(path);
    if(!file.is_open()){
        std::cout << "Can't find shader: " << path << std::endl;
        abort();
    }

    filestream << file.rdbuf();
    shadercode = filestream.str();

    file.close();

    shadercode_c = shadercode.c_str();

    compiled_program = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(compiled_program, 1, &shadercode_c, NULL);
    glCompileShader(compiled_program);


    glGetShaderiv(compiled_program, GL_COMPILE_STATUS, &status);
    if (!status){
        glGetShaderInfoLog(compiled_program, 419, NULL, compilemsg);
        std::cout << "Fragment shader " << path << " compile error:" << std::endl << compilemsg << std::endl;
        abort();
    }

    fragment_shaders[last_fragment_shader].name = path;
    fragment_shaders[last_fragment_shader].compiled_shader = compiled_program;

    last_fragment_shader++;

    return compiled_program;
}

uint32_t LinkShader(uint32_t vertex_shader, uint32_t fragment_shader) {
    int32_t status;
    char compilemsg[421];
    uint32_t compiled_shader;

    assert(vertex_shader);
    assert(fragment_shader);
    

    compiled_shader = glCreateProgram();
    glAttachShader(compiled_shader, vertex_shader);
    glAttachShader(compiled_shader, fragment_shader);
    glLinkProgram(compiled_shader);

    glGetProgramiv(compiled_shader, GL_LINK_STATUS, &status);
    if (!status){
        glGetShaderInfoLog(compiled_shader, 421, NULL, compilemsg);
        std::cout << "Shader link error: " << std::endl << compilemsg << std::endl;
        abort();
    }

    return compiled_shader;
}

void BindTextures(uint32_t shader) {        
    glUseProgram(shader);
    for (unsigned int i = 0; i < 16; i++){
        std::string ff = "sampler[" + std::to_string(i) + "]";
        glUniform1i(glGetUniformLocation(shader, ff.c_str()), i);
    }
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
        if (strcmp(linked_shaders[i].vertex_shader, vertex_shader) == 0 && strcmp(linked_shaders[i].fragment_shader, fragment_shader) == 0) {
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
#ifdef __EMSCRIPTEN__
    RegisterShader(VERTEX_STATIC,   MATERIAL_TEXTURE,          "shaders/gles3/normal_static.vert", "shaders/gles3/normal_static.frag");
    RegisterShader(VERTEX_STATIC,   MATERIAL_TEXTURE_ALPHA,    "shaders/gles3/normal_static.vert", "shaders/gles3/normal_static_alpha.frag");
    RegisterShader(VERTEX_DYNAMIC,  MATERIAL_TEXTURE,          "shaders/gles3/normal_dynamic.vert", "shaders/gles3/normal_dynamic.frag");
    RegisterShader(VERTEX_DYNAMIC,  MATERIAL_TEXTURE_ALPHA,    "shaders/gles3/normal_dynamic.vert", "shaders/gles3/normal_dynamic_alpha.frag");
    RegisterShader(VERTEX_STATIC,   MATERIAL_WATER,            "shaders/gles3/normal_static.vert", "shaders/gles3/normal_water.frag");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_TEXTURE_ALPHA,    "shaders/gles3/normal_sprite.vert", "shaders/gles3/normal_sprite.frag");
    RegisterShader(VERTEX_LINE,     MATERIAL_FLAT_COLOR,       "shaders/gles3/line.vert", "shaders/gles3/line.frag");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_MSDF,             "shaders/gles3/text.vert", "shaders/gles3/text.frag");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_GLYPH,            "shaders/gles3/glyph.vert", "shaders/gles3/glyph.frag");
#else
    RegisterShader(VERTEX_STATIC,   MATERIAL_TEXTURE,          "shaders/opengl3/normal_static.vert", "shaders/opengl3/normal_static.frag");
    RegisterShader(VERTEX_STATIC,   MATERIAL_TEXTURE_ALPHA,    "shaders/opengl3/normal_static.vert", "shaders/opengl3/normal_static_alpha.frag");
    RegisterShader(VERTEX_DYNAMIC,  MATERIAL_TEXTURE,          "shaders/opengl3/normal_dynamic.vert", "shaders/opengl3/normal_dynamic.frag");
    RegisterShader(VERTEX_DYNAMIC,  MATERIAL_TEXTURE_ALPHA,    "shaders/opengl3/normal_dynamic.vert", "shaders/opengl3/normal_dynamic_alpha.frag");
    RegisterShader(VERTEX_STATIC,   MATERIAL_WATER,            "shaders/opengl3/normal_static.vert", "shaders/opengl3/normal_water.frag");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_TEXTURE_ALPHA,    "shaders/opengl3/normal_sprite.vert", "shaders/opengl3/normal_sprite.frag");
    RegisterShader(VERTEX_LINE,     MATERIAL_FLAT_COLOR,       "shaders/opengl3/line.vert", "shaders/opengl3/line.frag");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_MSDF,             "shaders/opengl3/text.vert", "shaders/opengl3/text.frag");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_GLYPH,            "shaders/opengl3/glyph.vert", "shaders/opengl3/glyph.frag");
#endif


    
    for (size_t i = 0; i < last_linked_shader; i++) {
        BindTextures(linked_shaders[i].linked_shader);
    }
}

}
