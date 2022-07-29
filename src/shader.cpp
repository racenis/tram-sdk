// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// SHADER.CPP -- shader loading stuff.

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glad.h>

#include <core.h>
#include <render.h>

namespace Core::Render {

    class Shader {
    public:
        const char* const path;
        bool is_compiled = false;
        bool is_no_error = false;
        uint32_t compiled_program = 0;

        Shader (const char* const path) : path(path) {}
        virtual void Compile () = 0;
    };

    class VertexShader : public Shader {
    public:
        Model::VertexFormat vertex_format;
        
        VertexShader (const char* const path, Model::VertexFormat format) : Shader(path), vertex_format(format) {}
        
        void Compile () {
            std::ifstream file;
            std::stringstream filestream;
            std::string shadercode;
            const char* shadercode_c;
            int32_t status;
            char compilemsg[419];
            // none of these variable names make any sense

            assert(path);

            file.open(path);
            if(!file.is_open()){
                std::cout << "Can't find shader: " << path << std::endl;
                compiled_program = 0;
                is_compiled = false;
                return;
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
                std::cout << path << " Shader compile error: " << std::endl << compilemsg << std::endl;
                is_compiled = 0;
                return;
            }

            is_compiled = true;
        }
    };

    class FragmentShader : public Shader {
    public:
        Material::Type material_type;
        
        FragmentShader (const char* const path, Material::Type material_type) : Shader(path), material_type(material_type) {}
        
        void Compile () {
            std::ifstream file;
            std::stringstream filestream;
            std::string shadercode;
            const char* shadercode_c;
            int32_t status;
            char compilemsg[419];
            // none of these variable names make any sense

            assert(path);

            file.open(path);
            if(!file.is_open()){
                std::cout << "Can't find shader: " << path << std::endl;
                compiled_program = 0;
                is_compiled = false;
                return;
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
                std::cout << path << " Shader compile error: " << std::endl << compilemsg << std::endl;
                is_compiled = 0;
                return;
            }

            is_compiled = true;
        }
    };

    
    std::vector<ShaderProgram> ShaderProgram::shaderPrograms;
    
    
    void ShaderProgram::Link () {
        int32_t status;
        char compilemsg[421];

        assert(vertexShader);
        assert(fragmentShader);
        
        if (!vertexShader->is_compiled) vertexShader->Compile();
        if (!fragmentShader->is_compiled) fragmentShader->Compile();

        compiled_shader = glCreateProgram();
        glAttachShader(compiled_shader, vertexShader->compiled_program);
        glAttachShader(compiled_shader, fragmentShader->compiled_program);
        glLinkProgram(compiled_shader);

        glGetProgramiv(compiled_shader, GL_LINK_STATUS, &status);
        if (!status){
            glGetShaderInfoLog(compiled_shader, 421, NULL, compilemsg);
            std::cout << "Shader link error: " << std::endl << compilemsg << std::endl;
            is_compiled = false;
            return;
        }

        is_compiled = true;
    }
    
    void ShaderProgram::BindTextures () {
        assert(is_compiled);
        
        glUseProgram(compiled_shader);
        for (unsigned int i = 0; i < 16; i++){
            std::string ff = "sampler[" + std::to_string(i) + "]";
            glUniform1i(glGetUniformLocation(compiled_shader, ff.c_str()), i);
        }
    }
    
    void ShaderProgram::BindUniformBlock (bool (*filter)(ShaderProgram&), const char* const name, uint32_t binding) {
        for (auto& it : shaderPrograms) {
            if (filter(it)) {
                assert(it.is_compiled);
                glUniformBlockBinding(it.compiled_shader, glGetUniformBlockIndex(it.compiled_shader, name), binding);
            }
        }
            
    }

    void ShaderProgram::Add (const ShaderProgram& shader) {
        shaderPrograms.push_back(shader);
    }
    
    ShaderProgram* ShaderProgram::Find (Model::VertexFormat vertexType, Material::Type materialType) {
        auto best_fit = &*shaderPrograms.begin();
        
        for (auto& it : shaderPrograms) {
            //std::cout << "format: " << it.vertexShader->vertex_format << " type: " << it.fragmentShader->material_type << std::endl;
            if (it.vertexShader->vertex_format != vertexType) continue;
            if (it.fragmentShader->material_type == materialType) best_fit = &it;
        }
        
        return best_fit;
    }
    
    void ShaderProgram::CompileAll () {
        for (auto& it : shaderPrograms) {
            it.Link();
            it.BindTextures();
        }
    }


    /// Compiles shaders.
    void CompileShaders(){
        auto normal_static_vertex =          new VertexShader("shaders/normal_static.vert", Model::STATIC_VERTEX);
        auto normal_static_fragment =        new FragmentShader("shaders/normal_static.frag", Material::TEXTURE);
        auto normal_static_fragment_alpha =  new FragmentShader("shaders/normal_static_alpha.frag", Material::TEXTURE_ALPHA);
        auto normal_dynamic_vertex =         new VertexShader("shaders/normal_dynamic.vert", Model::DYNAMIC_VERTEX);
        auto normal_dynamic_fragment =       new FragmentShader("shaders/normal_dynamic.frag", Material::TEXTURE);
        auto normal_water_fragment =         new FragmentShader("shaders/normal_water.frag", Material::TEXTURE_WATER);
        auto line_vertex =                   new VertexShader("shaders/line.vert", Model::LINE_VERTEX);
        auto line_fragment =                 new FragmentShader("shaders/line.frag", Material::FLAT_COLOR);
        auto text_vertex =                   new VertexShader("shaders/text.vert", Model::TEXT_VERTEX);
        auto text_fragment =                 new FragmentShader("shaders/text.frag", Material::TEXTURE_MSDF);
        
        ShaderProgram::Add(ShaderProgram(normal_static_vertex, normal_static_fragment));
        ShaderProgram::Add(ShaderProgram(normal_static_vertex, normal_static_fragment_alpha));
        ShaderProgram::Add(ShaderProgram(normal_dynamic_vertex, normal_dynamic_fragment));
        ShaderProgram::Add(ShaderProgram(normal_static_vertex, normal_water_fragment));
        ShaderProgram::Add(ShaderProgram(line_vertex, line_fragment));
        ShaderProgram::Add(ShaderProgram(text_vertex, text_fragment));
        
        ShaderProgram::CompileAll();


        // this is where you'd check if all of the shaders are loaded and do fallbacks and stuff
        // also a good place to abort if the shaders can't be loaded

        return;
    }

    /// Selects a shader based on the models vertex format and material type.
    uint32_t FindShader(Model::VertexFormat format, Material::Type type){
        return ShaderProgram::Find(format, type)->compiled_shader;
    }
}
