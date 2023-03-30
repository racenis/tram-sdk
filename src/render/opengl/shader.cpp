// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

#ifdef __EMSCRIPTEN__
    #include <GL/gl.h>
	#include <GLES3/gl3.h>
#else
    #include <glad.h>
#endif

#include <render/opengl/renderer.h>

namespace tram::Render::OpenGL {
    std::vector<std::tuple<vertexformat_t, materialtype_t, uint32_t>> compiled_shaders;
    
    void StoreShaderForLater (vertexformat_t vertex_format, materialtype_t material_type, uint32_t shader) {
        compiled_shaders.push_back(std::tuple(vertex_format, material_type, shader));
    }
    
    uint32_t LoadVertexShader (const char* path) {
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

        return compiled_program;   
    }
    
    uint32_t LoadFragmentShader (const char* path) {
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

        return compiled_program;
    }
    
    uint32_t LinkShader (uint32_t vertex_shader, uint32_t fragment_shader) {
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
    
    void BindTextures (uint32_t shader) {        
        glUseProgram(shader);
        for (unsigned int i = 0; i < 16; i++){
            std::string ff = "sampler[" + std::to_string(i) + "]";
            glUniform1i(glGetUniformLocation(shader, ff.c_str()), i);
        }
    }

    void BindUniformBlock (const char* name, uint32_t binding) {
        for (auto& sh : compiled_shaders) {
            auto block_index = glGetUniformBlockIndex(std::get<2>(sh), name);
            if (block_index == GL_INVALID_INDEX) continue;
            glUniformBlockBinding(std::get<2>(sh), block_index, binding);
        }
    }
    
    uint32_t FindShader(vertexformat_t format, materialtype_t type){
        auto best_fit = std::get<2>(compiled_shaders[0]);
        
        for (auto& sh : compiled_shaders) {
            if (std::get<0>(sh) != format) continue;
            if (std::get<1>(sh) == type) best_fit = std::get<2>(sh);
        }
        
        return best_fit;
    }

    void CompileShaders(){
		std::cout << "loading shaders" << std::endl;
#ifdef __EMSCRIPTEN__
        auto normal_static_vertex =          LoadVertexShader("shaders/gles3/normal_static.vert");
        auto normal_static_fragment =        LoadFragmentShader("shaders/gles3/normal_static.frag");
        auto normal_static_fragment_alpha =  LoadFragmentShader("shaders/gles3/normal_static_alpha.frag");
        auto normal_dynamic_vertex =         LoadVertexShader("shaders/gles3/normal_dynamic.vert");
        auto normal_dynamic_fragment =       LoadFragmentShader("shaders/gles3/normal_dynamic.frag");
        auto normal_dynamic_fragment_alpha = LoadFragmentShader("shaders/gles3/normal_dynamic_alpha.frag");
        auto normal_water_fragment =         LoadFragmentShader("shaders/gles3/normal_water.frag");
        auto sprite_vertex =                 LoadVertexShader("shaders/gles3/normal_sprite.vert");
        auto sprite_fragment =               LoadFragmentShader("shaders/gles3/normal_sprite.frag");
        auto line_vertex =                   LoadVertexShader("shaders/gles3/line.vert");
        auto line_fragment =                 LoadFragmentShader("shaders/gles3/line.frag");
        auto text_vertex =                   LoadVertexShader("shaders/gles3/text.vert");
        auto text_fragment =                 LoadFragmentShader("shaders/gles3/text.frag");
        auto glyph_vertex =                  LoadVertexShader("shaders/gles3/glyph.vert");
        auto glyph_fragment =                LoadFragmentShader("shaders/gles3/glyph.frag");
#else
        auto normal_static_vertex =          LoadVertexShader("shaders/opengl3/normal_static.vert");
        auto normal_static_fragment =        LoadFragmentShader("shaders/opengl3/normal_static.frag");
        auto normal_static_fragment_alpha =  LoadFragmentShader("shaders/opengl3/normal_static_alpha.frag");
        auto normal_dynamic_vertex =         LoadVertexShader("shaders/opengl3/normal_dynamic.vert");
        auto normal_dynamic_fragment =       LoadFragmentShader("shaders/opengl3/normal_dynamic.frag");
        auto normal_dynamic_fragment_alpha = LoadFragmentShader("shaders/opengl3/normal_dynamic_alpha.frag");
        auto normal_water_fragment =         LoadFragmentShader("shaders/opengl3/normal_water.frag");
        auto sprite_vertex =                 LoadVertexShader("shaders/opengl3/normal_sprite.vert");
        auto sprite_fragment =               LoadFragmentShader("shaders/opengl3/normal_sprite.frag");
        auto line_vertex =                   LoadVertexShader("shaders/opengl3/line.vert");
        auto line_fragment =                 LoadFragmentShader("shaders/opengl3/line.frag");
        auto text_vertex =                   LoadVertexShader("shaders/opengl3/text.vert");
        auto text_fragment =                 LoadFragmentShader("shaders/opengl3/text.frag");
        auto glyph_vertex =                  LoadVertexShader("shaders/opengl3/glyph.vert");
        auto glyph_fragment =                LoadFragmentShader("shaders/opengl3/glyph.frag");
#endif

        StoreShaderForLater(VERTEX_STATIC,   MATERIAL_TEXTURE,          LinkShader(normal_static_vertex, normal_static_fragment));
        StoreShaderForLater(VERTEX_STATIC,   MATERIAL_TEXTURE_ALPHA,    LinkShader(normal_static_vertex, normal_static_fragment_alpha));
        StoreShaderForLater(VERTEX_DYNAMIC,  MATERIAL_TEXTURE,          LinkShader(normal_dynamic_vertex, normal_dynamic_fragment));
        StoreShaderForLater(VERTEX_DYNAMIC,  MATERIAL_TEXTURE_ALPHA,    LinkShader(normal_dynamic_vertex, normal_dynamic_fragment_alpha));
        StoreShaderForLater(VERTEX_STATIC,   MATERIAL_WATER,            LinkShader(normal_static_vertex, normal_water_fragment));
        StoreShaderForLater(VERTEX_SPRITE,   MATERIAL_TEXTURE_ALPHA,    LinkShader(sprite_vertex, sprite_fragment));
        StoreShaderForLater(VERTEX_LINE,     MATERIAL_FLAT_COLOR,       LinkShader(line_vertex, line_fragment));
        StoreShaderForLater(VERTEX_SPRITE,   MATERIAL_MSDF,             LinkShader(text_vertex, text_fragment));
        StoreShaderForLater(VERTEX_SPRITE,   MATERIAL_GLYPH,            LinkShader(glyph_vertex, glyph_fragment));
        
        for (auto& sh : compiled_shaders) BindTextures(std::get<2>(sh));
    }
}
