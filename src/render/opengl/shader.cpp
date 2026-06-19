// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <cassert>
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
    const char* shader_version = "#version 300 es";
#else
    const char* shader_path = "shaders/opengl4/";
    const char* shader_version = "#version 400 core";
#endif

static const char* get_fallback_shader(vertexformat_t format, uint32_t shader_type);

static const char* shader_flag_to_string(uint32_t flag) {
    switch (flag) {
        case SHADER_SPECULAR:
            return "FLAG_SPECULAR";
        case SHADER_LIGHTMAP:
            return "FLAG_LIGHTMAP";
        case SHADER_ALPHA_TEST:
            return "FLAG_ALPHA_TEST";
    default:
        return nullptr;
    }
}

static uint32_t SHADER_FLAG_LEN_LIMIT = 32;

static bool contains_flag(const char* shader, uint32_t shader_len, const char* flag) {
    uint32_t flag_hash = 0;
    uint32_t flag_len = strlen(flag);
    uint32_t hash = 0;
    
    if (flag_len > shader_len) return false;
    
    for (const char* c = flag; *c != '\0'; c++) {
        flag_hash = *c + (flag_hash << 5) + flag_hash;
    }
    
    for (uint32_t i = 0; i < flag_len; i++) {
        hash = shader[i] + (hash << 5) + hash;
    }
    
    uint32_t p = 1;
    for (uint32_t i = 0; i < flag_len - 1; i++) {
        p = (p << 5) + p;
    }
    
    for (uint32_t i = flag_len; i < shader_len; i++) {
        if (hash == flag_hash) {
            if (strncmp(shader + i - flag_len, flag, flag_len) == 0) {
                return true;
            }
        }
        
        hash = hash - shader[i - flag_len] * p;
        hash = shader[i] + (hash << 5) + hash;
    }
    
    return false;
}

static uint32_t find_flags_in_shader(const char* shader, uint32_t shader_len) {
    uint32_t flags = 0;
    for (uint32_t i = 0; i < 32; i++) {
        uint32_t flag = 1 << i;
        if (!shader_flag_to_string(flag)) continue;
        if (!contains_flag(shader, shader_len, shader_flag_to_string(flag))) continue;
        
        flags |= flag;
    }
    return flags;
}

class Shader {
public:
    virtual const char* GetExtension() = 0;
    virtual uint32_t GetGLShaderCode() = 0;
    
    name_t GetName() {
        return name;
    }
    
    void SetInitialFlags(uint32_t flags) {
        initial_flags = flags;
    }
    
    const char* GetShaderTypeName() {
        switch (GetGLShaderCode()) {
            case GL_FRAGMENT_SHADER: return "Fragment";
            case GL_VERTEX_SHADER: return "Vertex";
            default: return "Unknown type";
        }
    }
    
    // lazy loads shader code as needed and caches it
    void LoadShaderFromDisk() {
        if (shader_code) return;
        
        char path[PATH_LIMIT];
        strcpy(path, shader_path);
        strcat(path, name);
        strcat(path, GetExtension());
        
        FileReader* file = FileReader::GetReader(path);
        
        if (file->GetStatus() != FileStatus::READY) {
            Log(Severity::ERROR, System::RENDER, "Can't find {} shader source file {}!", GetShaderTypeName(), UID(path));
            
            // load in shader fallback
            shader_len = strlen(get_fallback_shader(format, GetGLShaderCode()));
            shader_code = (char*)malloc(shader_len + 1);
            strncpy(shader_code, get_fallback_shader(format, GetGLShaderCode()), shader_len + 1);
        } else {
            shader_len = file->GetSize();
            shader_code = (char*)malloc(shader_len + 1);
            strncpy(shader_code, file->GetContents(), shader_len + 1);
        }
        
        flags_in_shader = find_flags_in_shader(shader_code, shader_len);
        
        // conservative allocation
        shader_proc = (char*)malloc(shader_len + SHADER_FLAG_LEN_LIMIT * 34 + 1);
        
        file->Yeet();
    }

    uint32_t GetShader(uint32_t flags) {
        LoadShaderFromDisk();

        // remove flags not in shader and set initial flags
        flags &= flags_in_shader;

        // check if shader already compiled with given flags
        for (auto [shader_flags, shader] : compiled_shaders) {
            if (shader_flags == flags) return shader;
        }
        
        Log(Severity::INFO, System::RENDER, "Compiling {} shader {} with {} flags", GetShaderTypeName(), name, flags);
        
        // otherwise compile the shader again
        strcpy(shader_proc, shader_version);
        strcat(shader_proc, "\n");
        uint32_t line = 1;
        for (uint32_t i = 0; i < 32; i++) {
            if (!(flags & (1 << i))) continue;
            strcat(shader_proc, "#define ");
            strcat(shader_proc, shader_flag_to_string(1 << i));
            strcat(shader_proc, "\n");
            line++;
        }
        
        char line_directive[16];
        snprintf(line_directive, 16, "#line %u\n", line);
        strcat(shader_proc, line_directive);
        strcat(shader_proc, shader_code);
        
        uint32_t compiled_program = glCreateShader(GetGLShaderCode());
        glShaderSource(compiled_program, 1, &shader_proc, NULL);
        glCompileShader(compiled_program);

        int32_t compile_status;
        glGetShaderiv(compiled_program, GL_COMPILE_STATUS, &compile_status);
        
        if (!compile_status) {
            char compile_error[420];
            glGetShaderInfoLog(compiled_program, 420, NULL, compile_error);
            
            Log(Severity::CRITICAL_ERROR, System::RENDER, "{} shader {} compile error:\n{}", GetShaderTypeName(), UID(name), UID(compile_error));
        }
        
        compiled_shaders.push_back({flags, compiled_program});
        return compiled_program;
    }
    
protected:
    Shader(name_t name, vertexformat_t format) : name(name), format(format) {}
    
    name_t name;
    std::vector<std::pair<shaderflags_t, uint32_t>> compiled_shaders;
    
    char* shader_code = nullptr;
    uint32_t shader_len = 0;
    uint32_t flags_in_shader = 0;
    
    uint32_t initial_flags = 0;
    
    vertexformat_t format = 0;
    
    char* shader_proc = nullptr;
};

class VertexShader : public Shader {
public:
    const char* GetExtension() override {
        return ".vert";
    }
    
    uint32_t GetGLShaderCode() override {
        return GL_VERTEX_SHADER;
    }
    
    static VertexShader* Find(name_t name, vertexformat_t format) {
        for (auto shader : all_shaders) {
            if (shader->GetName() == name) return shader;
        }
        
        auto new_shader = new VertexShader(name, format);
        all_shaders.push_back(new_shader);
        
        return new_shader;
    }
private:
    VertexShader(name_t name, vertexformat_t format) : Shader(name, format) {}
    static std::vector<VertexShader*> all_shaders;
};

class FragmentShader : public Shader {
public:
    const char* GetExtension() override {
        return ".frag";
    }
    
    uint32_t GetGLShaderCode() override {
        return GL_FRAGMENT_SHADER;
    }
    
    static FragmentShader* Find(name_t name, vertexformat_t format) {
        for (auto shader : all_shaders) {
            if (shader->GetName() == name) return shader;
        }
        
        auto new_shader = new FragmentShader(name, format);
        all_shaders.push_back(new_shader);
        
        return new_shader;
    }
private:
    FragmentShader(name_t name, vertexformat_t format) : Shader(name, format) {}
    static std::vector<FragmentShader*> all_shaders;
};

std::vector<VertexShader*> VertexShader::all_shaders;
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
        
        // add any additional flags that specific material types might need
        flags |= additional_flags;
        
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
            
            Log(Severity::ERROR, System::RENDER, "Shader link error between {} and {}:\n{}", vertex_shader->GetName(), fragment_shader->GetName(), UID(link_error));
            
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
        glUniform1i(glGetUniformLocation(linked_shader, "samplerArray"), 15);
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
    
    void SetInitialFlags(uint32_t flags) {
        vertex_shader->SetInitialFlags(flags);
        fragment_shader->SetInitialFlags(flags);
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
    
    static void Register(vertexformat_t format, materialtype_t type, uint32_t flags, const char* vertex_shader, const char* fragment_shader) {
        
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
        auto new_shader = new LinkedShader(format, type, flags, vertex_shader, fragment_shader);
        all_shaders.push_back(new_shader);;
    }
    
    static void RegisterUniformBlock(const char* name, uint32_t binding) {
        all_uniform_blocks.push_back({name, binding});
        
        for (auto shader : all_shaders) {
            shader->BindUniformBlock(name, binding);
        }
    }
private:
    LinkedShader(vertexformat_t format, materialtype_t type, uint32_t additional_flags, name_t vertex, name_t fragment) : format(format), type(type), additional_flags(additional_flags) {
        vertex_shader = VertexShader::Find(vertex, format);
        fragment_shader = FragmentShader::Find(fragment, format);
    }
    
    vertexformat_t format;
    materialtype_t type;
    
    VertexShader* vertex_shader = nullptr;
    FragmentShader* fragment_shader = nullptr;
    
    uint32_t additional_flags = 0;
    
    std::vector<std::pair<shaderflags_t, uint32_t>> linked_shaders;
    
    static std::vector<std::pair<const char*, uint32_t>> all_uniform_blocks;
    static std::vector<LinkedShader*> all_shaders;
};

std::vector<std::pair<const char*, uint32_t>> LinkedShader::all_uniform_blocks;
std::vector<LinkedShader*> LinkedShader::all_shaders;

void BindUniformBlock(const char* name, uint32_t binding) {
    LinkedShader::RegisterUniformBlock(name, binding);
}

void RegisterShader(vertexformat_t format, materialtype_t type, shaderflags_t flags, const char* vertex_shader, const char* fragment_shader) {
    LinkedShader::Register(format, type, flags, vertex_shader, fragment_shader);
}

uint32_t FindShader(vertexformat_t format, materialtype_t type, shaderflags_t mask) {
    return LinkedShader::FindShader(format, type, mask);
}

uint32_t GetAnyShader(shaderflags_t mask) {
    return LinkedShader::GetAnyShader(mask);
}

void CompileShaders() {
    RegisterShader(VERTEX_STATIC,   MATERIAL_TEXTURE,       SHADER_NONE,        "static",   "static");
    RegisterShader(VERTEX_STATIC,   MATERIAL_TEXTURE_ALPHA, SHADER_ALPHA_TEST,  "static",   "static");
    RegisterShader(VERTEX_STATIC,   MATERIAL_TEXTURE_BLEND, SHADER_NONE,        "static",   "static");
    RegisterShader(VERTEX_DYNAMIC,  MATERIAL_TEXTURE,       SHADER_NONE,        "dynamic",  "dynamic");
    RegisterShader(VERTEX_DYNAMIC,  MATERIAL_TEXTURE_ALPHA, SHADER_ALPHA_TEST,  "dynamic",  "dynamic");
    RegisterShader(VERTEX_DYNAMIC,  MATERIAL_TEXTURE_BLEND, SHADER_NONE,        "dynamic",  "dynamic");
    RegisterShader(VERTEX_STATIC,   MATERIAL_WATER,         SHADER_NONE,        "static",   "water");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_TEXTURE_ALPHA, SHADER_NONE,        "sprite",   "sprite");
    RegisterShader(VERTEX_LINE,     MATERIAL_FLAT_COLOR,    SHADER_NONE,        "line",     "line");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_MSDF,          SHADER_NONE,        "text",     "text");
    RegisterShader(VERTEX_SPRITE,   MATERIAL_GLYPH,         SHADER_NONE,        "glyph",    "glyph");
}

static const char* get_fallback_shader(vertexformat_t format, uint32_t shader_type) {
    if (shader_type == GL_VERTEX_SHADER) switch (format) {
        case VERTEX_STATIC:
            return  "layout (location = 0) in vec3 Position;"
                    "layout (location = 1) in vec3 Normal;"
                    "layout (location = 2) in vec2 VertUV;"
                    "layout (location = 3) in vec2 VertLightUV;"
                    "layout (location = 4) in uint TexIndex;"
                    ""
                    "layout (std140) uniform Matrices {"
                    "    mat4 projection;"
                    "    mat4 view;"
                    "};"
                    ""
                    "layout (std140) uniform ModelMatrices {"
                    "    mat4 model; uvec4 model_lights; vec3 sun_direction; vec3 sun_color;"
                    "    vec4 ambient_color; float time; float sun_weight; float screen_width;"
                    "    float screen_height; vec4 colors[15]; vec4 specular[15];"
                    "    vec4 texture_transforms[15];"
                    "};"
                    ""
                    "out vec2 vert_uv;"
                    "out vec2 vert_light_uv;"
                    "out vec3 vert_color;"
                    "out float vert_opacity;"
                    "flat out uint vert_tex_index;"
                    ""
                    "void main() {"
                    "    gl_Position = projection * view * model * vec4(Position, 1.0);"
                    ""
                    "    /* simple warnock lighting */"
                    "    vec3 n = normalize(vec3(projection * view * model * vec4(Normal, 0.0)));"
                    "    vert_color = vec3(ambient_color);"
                    "    vert_color += clamp(dot(n, vec3(0.0, 0.0, -1.0)), 0.0, 1.0);"
                    "    vert_color *= vec3(colors[TexIndex]);"
                    ""
                    "    vert_opacity = colors[TexIndex].w;"
                    "    vert_uv = VertUV + vec2(texture_transforms[TexIndex]);"
                    "    vert_tex_index = TexIndex;"
                    "    vert_light_uv = VertLightUV;"
                    "}";
        case VERTEX_DYNAMIC:
            return  "layout (location = 0) in vec3 Position;"
                    "layout (location = 1) in vec3 Normal;"
                    "layout (location = 2) in vec2 VertUV;"
                    "layout (location = 3) in ivec4 BoneIndex;"
                    "layout (location = 4) in vec4 BoneWeight;"
                    "layout (location = 5) in uint TexIndex;"
                    ""
                    "layout (std140) uniform Matrices {"
                    "    mat4 projection; mat4 view; vec3 view_pos;"
                    "};"
                    ""
                    "layout (std140) uniform ModelMatrices {"
                    "    mat4 model; uvec4 model_lights; vec3 sun_direction; vec3 sun_color;"
                    "    vec4 ambient_color; float time; float sun_weight; float screen_width;"
                    "    float screen_height; vec4 colors[15]; vec4 specular[15];"
                    "    vec4 texture_transforms[15]; vec3 l00; vec3 l1m1; vec3 l10; vec3 l11;"
                    "    vec3 l2m2; vec3 l2m1; vec3 l20; vec3 l21; vec3 l22;"
                    "};"
                    ""
                    "out vec3 vert_color;"
                    "out vec2 vert_uv;"
                    "out float vert_opacity;"
                    "flat out uint vert_tex_index;"
                    ""
                    "void main() {"
                    "    gl_Position = projection * view * model * vec4(Position, 1.0);"
                    ""
                    "    /* simple warnock illumination + ambient color term */"
                    "    vec3 n = normalize(vec3(projection * view * model * vec4(Normal, 0.0)));"
                    "    vert_color = vec3(ambient_color);"
                    "    vert_color += clamp(dot(vec3(0.0, 0.0, -1.0), n), 0.0, 1.0);"
                    "    vert_color *= vec3(colors[TexIndex]);"
                    ""
                    "    vert_opacity = colors[TexIndex].w;"
                    "    vert_uv = VertUV;"
                    "}";
        case VERTEX_SPRITE:
            return  "layout (location = 0) in vec3 Position;"
                    "layout (location = 1) in vec2 TexCoord;"
                    "layout (location = 2) in vec3 VertColor;"
                    "layout (location = 5) in uint Texture;"
                    ""
                    "out vec2 vert_uv;"
                    "out vec3 vert_color;"
                    "flat out uint vert_tex_index;"
                    ""
                    "uniform sampler2D sampler[15];"
                    "uniform sampler2DArray samplerArray;"
                    ""
                    "layout (std140) uniform ModelMatrices {"
                    "    mat4 model; uvec4 model_lights; vec3 sun_direction; vec3 sun_color;"
                    "    vec4 ambient_color; float time; float sun_weight; float screen_width;"
                    "    float screen_height; vec4 colors[15]; vec4 specular[15]; "
                    "    vec4 texture_transforms[15];"
                    "};"
                    ""
                    "void main() {"
                    "    float pos_x = (round(Position.x) / (screen_width / 2.0)) - 1.0;"
                    "    float pos_y = (round(Position.y) / (screen_height / -2.0)) + 1.0;"
                    "    float depth = -0.5 - (Position.z / 128.0);"
                    "    gl_Position = vec4(pos_x, pos_y, depth, 1.0);"
                    "    vec2 tex_size = textureSize(sampler[Texture], 0);"
                    "    vert_uv = vec2(TexCoord.x/tex_size.x, TexCoord.y/-tex_size.y);"
                    "    vert_color = VertColor;"
                    "    vert_tex_index = Texture;"
                    "}";
        case VERTEX_LINE:
        default:
            return  "layout (location = 0) in vec3 Position;"
                    "layout (location = 1) in vec3 VertColor;"
                    ""
                    "layout (std140) uniform Matrices {"
                    "    mat4 projection;"
                    "    mat4 view;"
                    "    vec3 view_pos;"
                    "};"
                    ""
                    "out vec3 vert_color;"
                    ""
                    "void main() {"
                    "    gl_Position = projection * view * vec4(Position, 1.0);"
                    "    vert_color = VertColor;"
                    "}";
    }
    
    if (shader_type == GL_FRAGMENT_SHADER) switch (format) {
        case VERTEX_STATIC:
            return  "out vec4 fragment;"
                    ""
                    "in vec2 vert_uv;"
                    "in vec2 vert_light_uv;"
                    "in vec3 vert_color;"
                    "in float vert_opacity;"
                    "flat in uint vert_tex_index;"
                    ""
                    "uniform sampler2D sampler[15];"
                    "uniform sampler2DArray samplerArray;"
                    ""
                    "void main() {"
                    "    fragment = texture(sampler[vert_tex_index], vert_uv) * texture(samplerArray, vec3(vert_light_uv, 0.0)) * vec4(vert_color, vert_opacity);"
                    "}";
        case VERTEX_DYNAMIC:
            return  "out vec4 fragment;"
                    ""
                    "in vec2 vert_uv;"
                    "in vec3 vert_color;"
                    "in float vert_opacity;"
                    "flat in uint vert_tex_index;"
                    ""
                    "uniform sampler2D sampler[15];"
                    "uniform sampler2DArray samplerArray;"
                    ""
                    "void main() {"
                    "    fragment = texture(sampler[vert_tex_index], vert_uv) * vec4(vert_color, vert_opacity);"
                    "}";
        case VERTEX_SPRITE:
            return  "out vec4 fragment;"
                    ""
                    "in vec2 vert_uv;"
                    "in vec3 vert_color;"
                    "flat in uint vert_tex_index;"
                    ""
                    "uniform sampler2D sampler[15];"
                    "uniform sampler2DArray samplerArray;"
                    ""
                    "void main() {"
                    "    vec4 sampled_color = texture(sampler[vert_tex_index], vert_uv);"
                    "    if (sampled_color.w < 1.0) discard;"
                    "    fragment = sampled_color * vec4(vert_color, 1.0);"
                    "}";

        case VERTEX_LINE:
        default:
            return  "out vec4 fragment;"
                    "in vec3 vert_color;"
                    "void main() {"
                    "    fragment = vec4(vert_color, 1.0);"
                    "}";
    }
    
    return nullptr;
}

}
