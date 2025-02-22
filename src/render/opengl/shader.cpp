// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

#include <config.h>

#ifndef _WIN32
    #include <GL/gl.h>
	#include <GLES3/gl3.h>
#else
    #include <glad.h>
#endif

#include <framework/logging.h>

#include <platform/file.h>

#include <render/opengl/shader.h>
#include <render/opengl/renderer.h>

namespace tram::Render::API {

#ifndef _WIN32
    const char* shader_path = "shaders/gles3/";
#else
    const char* shader_path = "shaders/opengl4/";
#endif

class VertexShader {
public:
    name_t GetName() {
        return name;
    }

    uint32_t GetShader(uint32_t flags) {
        
        // check if shader already compiled with given flags
        for (auto [shader_flags, shader] : compiled_shaders) {
            if (shader_flags == flags) return shader;
        }
        
        // otherwise compile the shader again
        char path[256];
        strcpy(path, shader_path);
        strcat(path, name);
        strcat(path, ".vert");
        
        FileReader* file = FileReader::GetReader(path);
        
        if (file->GetStatus() != FileStatus::READY) {
            Log(Severity::ERROR, System::RENDER, "Can't find vertex shader source file {}!", UID(path));
        }

        Log(Severity::INFO, System::RENDER, "Compiling vertex shader {} with {} flags", name, flags);

        uint32_t compiled_program = glCreateShader(GL_VERTEX_SHADER);
        const char* contents = file->GetContents();
        glShaderSource(compiled_program, 1, &contents, NULL);
        glCompileShader(compiled_program);

        int32_t compile_status;
        glGetShaderiv(compiled_program, GL_COMPILE_STATUS, &compile_status);
        
        if (!compile_status) {
            char compile_error[420];
            glGetShaderInfoLog(compiled_program, 420, NULL, compile_error);
            
            Log(Severity::CRITICAL_ERROR, System::RENDER, "Vertex shader {} compile error:\n{}", UID(name), UID(compile_error));
        }

        file->Yeet();
        
        compiled_shaders.push_back({flags, compiled_program});
        return compiled_program;
    }
    
    static VertexShader* Find(name_t name) {
        for (auto shader : all_shaders) {
            if (shader->GetName() == name) return shader;
        }
        
        auto new_shader = new VertexShader(name);
        all_shaders.push_back(new_shader);
        
        return new_shader;
    }
private:
    VertexShader(name_t name) : name(name) {}
    
    name_t name;
    std::vector<std::pair<shaderflags_t, uint32_t>> compiled_shaders;
    
    static std::vector<VertexShader*> all_shaders;
};

std::vector<VertexShader*> VertexShader::all_shaders;

class FragmentShader {
public:
    name_t GetName() {
        return name;
    }

    uint32_t GetShader(uint32_t flags) {
        
        // check if shader already compiled with given flags
        for (auto [shader_flags, shader] : compiled_shaders) {
            if (shader_flags == flags) return shader;
        }
        
        // otherwise compile the shader again
        char path[256];
        strcpy(path, shader_path);
        strcat(path, name);
        strcat(path, ".frag");
        
        FileReader* file = FileReader::GetReader(path);
        
        if (file->GetStatus() != FileStatus::READY) {
            Log(Severity::ERROR, System::RENDER, "Can't find fragment shader source file {}!", UID(path));
            return 0;
        }
        
        Log(Severity::INFO, System::RENDER, "Compiling fragment shader {} with {} flags", name, flags);

        uint32_t compiled_program = glCreateShader(GL_FRAGMENT_SHADER);
        const char* contents = file->GetContents();
        glShaderSource(compiled_program, 1, &contents, NULL);
        glCompileShader(compiled_program);

        int32_t compile_status;
        glGetShaderiv(compiled_program, GL_COMPILE_STATUS, &compile_status);
        
        if (!compile_status) {
            char compile_error[420];
            glGetShaderInfoLog(compiled_program, 420, NULL, compile_error);
            
            Log(Severity::CRITICAL_ERROR, System::RENDER, "Fragment shader {} compile error:\n{}", UID(name), UID(compile_error));
        }

        file->Yeet();
            
        
        compiled_shaders.push_back({flags, compiled_program});
        return compiled_program;
    }
    
    static FragmentShader* Find(name_t name) {
        for (auto shader : all_shaders) {
            if (shader->GetName() == name) return shader;
        }
        
        auto new_shader = new FragmentShader(name);
        all_shaders.push_back(new_shader);
        
        return new_shader;
    }
private:
    FragmentShader(name_t name) : name(name) {}
    
    name_t name;
    std::vector<std::pair<shaderflags_t, uint32_t>> compiled_shaders;
    
    static std::vector<FragmentShader*> all_shaders;
};

std::vector<FragmentShader*> FragmentShader::all_shaders;

class LinkedShader {
public:
    vertexformat_t GetFormat() {
        return format;
    }
    
    materialtype_t GetType() {
        return type;
    }
    
    VertexShader* GetVertexShader() {
        return vertex_shader;
    }
    
    FragmentShader* GetFragmentShader() {
        return fragment_shader;
    }

    uint32_t GetShader(uint32_t flags) {
        
        // check if shader already linked with given flags
        for (auto [shader_flags, shader] : linked_shaders) {
            if (shader_flags == flags) return shader;
        }
        
        Log(Severity::INFO, System::RENDER, "Linking {} and {} shader with {} flags",
                                             GetVertexFormatName(format),
                                             GetMaterialTypeName(type),
                                             flags);
        
        // otherwise link the shader again
        uint32_t linked_shader = glCreateProgram();
        glAttachShader(linked_shader, vertex_shader->GetShader(flags));
        glAttachShader(linked_shader, fragment_shader->GetShader(flags));
        glLinkProgram(linked_shader);

        int32_t link_status;
        glGetProgramiv(linked_shader, GL_LINK_STATUS, &link_status);
        
        if (!link_status) {
            char link_error[420];
            glGetShaderInfoLog(linked_shader, 420, NULL, link_error);
            
            Log(Severity::ERROR, System::RENDER, "Shader link error:\n{}", UID(link_error));
            
            return 0;
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
        
        for (auto [name, binding] : all_uniform_blocks) {
            auto block_index = glGetUniformBlockIndex(linked_shader, name);
                
            if (block_index == GL_INVALID_INDEX) continue;
            
            glUniformBlockBinding(linked_shader, block_index, binding);
            
        }
        
        linked_shaders.push_back({flags, linked_shader});
        
        return linked_shader;
    }
    
    void BindUniformBlock(const char* name, uint32_t binding) {
        for (auto [flags, shader_name] : linked_shaders) {
            auto block_index = glGetUniformBlockIndex(shader_name, name);
            
            if (block_index == GL_INVALID_INDEX) continue;
            
            glUniformBlockBinding(shader_name, block_index, binding);
            
            
            GLint result;
            glGetActiveUniformBlockiv(shader_name, block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &result);
            
        }
    }
    
    static uint32_t FindShader(vertexformat_t format, materialtype_t type, shaderflags_t flags) {
        for (auto shader : all_shaders) {
            if (shader->GetFormat() == format && shader->GetType() == type) return shader->GetShader(flags);
        }
        
        Log(Severity::ERROR, System::RENDER, "Can't find shader pair for {} and {}!",
                                             GetVertexFormatName(format),
                                             GetMaterialTypeName(type));
                                                     
        return 0;
    }
    
    // we could hard-code a default shader to use when no other shaders are available
    // TODO: fix
    static uint32_t GetAnyShader(shaderflags_t flags) {
        return all_shaders.front()->GetShader(flags);
    }
    
    static void Register(vertexformat_t format, materialtype_t type, const char* vertex_shader, const char* fragment_shader) {
        
        // check if format/type pair already registered
        for (auto shader : all_shaders) {
            if (shader->GetFormat() == format && shader->GetType() == type) {
                Log(Severity::ERROR, System::RENDER, "Can't register shader pair for {} and {}!\nNew pair\t[ {}; {} ]\nExisting\t[ {}; {} ]",
                                                     GetVertexFormatName(format),
                                                     GetMaterialTypeName(type),
                                                     vertex_shader, 
                                                     fragment_shader,
                                                     shader->GetVertexShader()->GetName(),
                                                     shader->GetFragmentShader()->GetName());
                                                     
                return;
            }
        }
        
        // we could check if vertex_shader and fragment_shader files actually
        // exist on disk and not register the shader if it they aren't found
        // TODO: fix
        auto new_shader = new LinkedShader(format, type, vertex_shader, fragment_shader);
        all_shaders.push_back(new_shader);
    }
    
    static void RegisterUniformBlock(const char* name, uint32_t binding) {
        all_uniform_blocks.push_back({name, binding});
        
        for (auto shader : all_shaders) {
            shader->BindUniformBlock(name, binding);
        }
    }
private:
    LinkedShader(vertexformat_t format, materialtype_t type, name_t vertex, name_t fragment) : format(format), type(type) {
        vertex_shader = VertexShader::Find(vertex);
        fragment_shader = FragmentShader::Find(fragment);
    }
    
    vertexformat_t format;
    materialtype_t type;
    
    VertexShader* vertex_shader = nullptr;
    FragmentShader* fragment_shader = nullptr;
    
    std::vector<std::pair<shaderflags_t, uint32_t>> linked_shaders;
    
    static std::vector<std::pair<const char*, uint32_t>> all_uniform_blocks;
    static std::vector<LinkedShader*> all_shaders;
};

std::vector<std::pair<const char*, uint32_t>> LinkedShader::all_uniform_blocks;
std::vector<LinkedShader*> LinkedShader::all_shaders;

void BindUniformBlock(const char* name, uint32_t binding) {
    LinkedShader::RegisterUniformBlock(name, binding);
}

void RegisterShader(vertexformat_t format, materialtype_t type, const char* vertex_shader, const char* fragment_shader) {
    LinkedShader::Register(format, type, vertex_shader, fragment_shader);
}

uint32_t FindShader(vertexformat_t format, materialtype_t type, shaderflags_t mask) {
    return LinkedShader::FindShader(format, type, mask);
}

uint32_t GetAnyShader(shaderflags_t mask) {
    return LinkedShader::GetAnyShader(mask);
}

void CompileShaders() {
    std::cout << "Loading shaders... " << std::flush;

    RegisterShader(VERTEX_STATIC,   MATERIAL_TEXTURE,          "normal_static",     "normal_static");
    RegisterShader(VERTEX_STATIC,   MATERIAL_TEXTURE_ALPHA,    "normal_static",     "normal_static_alpha");
    RegisterShader(VERTEX_DYNAMIC,  MATERIAL_TEXTURE,          "normal_dynamic",    "normal_dynamic");
    RegisterShader(VERTEX_DYNAMIC,  MATERIAL_TEXTURE_ALPHA,    "normal_dynamic",    "normal_dynamic_alpha");
    RegisterShader(VERTEX_STATIC,   MATERIAL_WATER,            "normal_static",     "normal_water");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_TEXTURE_ALPHA,    "normal_sprite",     "normal_sprite");
    RegisterShader(VERTEX_LINE,     MATERIAL_FLAT_COLOR,       "line",              "line");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_MSDF,             "text",              "text");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_GLYPH,            "glyph",             "glyph");
    
    std::cout << "done!" << std::endl;
}

}
