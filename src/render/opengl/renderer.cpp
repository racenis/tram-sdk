// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/opengl/renderer.h>
#include <render/api.h>

#include <templates/octree.h>

#ifndef __WIN32
    #include <GL/gl.h>
	#include <GLES3/gl3.h>
#else
    #include <glad.h>
#endif

#include <glm/gtc/type_ptr.hpp>

namespace tram::Render::API {

Pool<GLDrawListEntry> draw_list ("render list", 500, false);
Pool<GLLight> light_list ("light list", 200, false);
Octree<uint32_t> light_tree("light tree", 200);
std::vector<uint32_t> light_tree_ids (200);

struct ShaderUniformMatrices {
    mat4 projection;       /// Projection matrix.
    mat4 view;             /// View matrix.
    vec3 view_pos;
    float padding;
};

struct ShaderUniformModelMatrices {
    mat4 model;        /// Model -> world space matrix. Rotates and translates vertices from how they are defined in the model to where they will appear in the world.
    uvec4 modelLights; /// Indices for lights in the light list. The shader will use these 4 indices to determine with which lights the model should be lit up.
    vec4 sunDirection; /// Normalized vector. Sunlight direction.
    vec4 sunColor;     /// Sunlight color.
    vec4 ambientColor; /// Shadow color.
    float time;
    float sunWeight;
    float screenWidth;
    float screenHeight;
    vec4 colors[15];
    vec4 specular[15];
};

struct LayerParameters {
    //vec3 camera_position = {0.0f, 0.0f, 0.0f};
    //quat camera_rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    
    mat4 projection_matrix = mat4(1.0f);
    mat4 view_matrix = mat4(1.0f);
    vec3 view_position = {1.0f, 1.0f, 1.0f};
    
    vec3 sun_direction = {0.0f, 1.0f, 0.0f};
    vec3 sun_color = {1.0f, 1.0f, 1.0f};
    vec3 ambient_color = {0.3f, 0.3f, 0.3f};
};

ShaderUniformMatrices matrices;
ShaderUniformModelMatrices modelMatrices;

static LayerParameters LAYER[7];

class ShaderBuffer {};

const uint32_t matrix_uniform_binding = 0;
const uint32_t model_matrix_uniform_binding = 1;
const uint32_t light_uniform_binding = 2;
const uint32_t bone_uniform_binding = 3;

uint32_t matrix_uniform_buffer;
uint32_t model_matrix_uniform_buffer;
uint32_t light_uniform_buffer;
uint32_t bone_uniform_buffer;

vec3 screen_clear_color = {0.2f, 0.3f, 0.3f};
bool clear_screen = true;

static float SCREEN_WIDTH = 800.0f;
static float SCREEN_HEIGHT = 600.0f;


uint32_t MakeUniformBuffer (const char* name, uint32_t binding, uint32_t initial_size) {
    uint32_t handle;
    
    glGenBuffers(1, &handle);
    glBindBuffer(GL_UNIFORM_BUFFER, handle);
    glBufferData(GL_UNIFORM_BUFFER, initial_size, NULL, GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER, binding, handle);

    BindUniformBlock (name, binding);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return handle;
}

void UploadUniformBuffer (uint32_t handle, uint32_t data_size, void* data) {
    glBindBuffer(GL_UNIFORM_BUFFER, handle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, data_size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SetLightingParameters (vec3 sun_direction, vec3 sun_color, vec3 ambient_color, uint32_t layer) {
    LAYER[layer].sun_direction = sun_direction;
    LAYER[layer].sun_color = sun_color;
    LAYER[layer].ambient_color = ambient_color;
}

void SetViewParameters (vec3 position, quat rotation, uint32_t layer) {
    //LAYER[layer].camera_position = position;
    //LAYER[layer].camera_rotation = rotation;
}

void SetScreenSize(float width, float height) {
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    
    glViewport(0, 0, width, height);
    
    //matrices.projection = glm::perspective(glm::radians(60.0f), width / height, 0.1f, 1000.0f);
}

void SetScreenClear (vec3 clear_color, bool clear) {
    clear_screen = clear;
    screen_clear_color = clear_color;
}

void RenderFrame() {
    if (clear_screen) {
        glClearColor(screen_clear_color.x, screen_clear_color.y, screen_clear_color.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    
    modelMatrices.time = GetTickTime();
    modelMatrices.sunDirection =    vec4(LAYER[0].sun_direction, 1.0f);
    modelMatrices.sunColor =        vec4(LAYER[0].sun_color, 1.0f);
    modelMatrices.ambientColor =    vec4(LAYER[0].ambient_color, 1.0f);
    modelMatrices.screenWidth =     SCREEN_WIDTH;
    modelMatrices.screenHeight =    SCREEN_HEIGHT;

    matrices.projection = LAYER[0].projection_matrix;
    matrices.view = LAYER[0].view_matrix;
    matrices.view_pos = LAYER[0].view_position;
    //matrices.view = glm::inverse(glm::translate(mat4(1.0f), LAYER[0].camera_position) * glm::toMat4(LAYER[0].camera_rotation));
    //matrices.view_pos = LAYER[0].camera_position;

    //if (THIRD_PERSON) matrices.view = glm::translate(matrices.view, LAYER[0].camera_rotation * vec3(0.0f, 0.0f, -5.0f));

    UploadUniformBuffer(matrix_uniform_buffer, sizeof(ShaderUniformMatrices), &matrices);
    UploadUniformBuffer(light_uniform_buffer, sizeof(GLLight)*50, light_list.begin().ptr);

    static uint32_t layer; layer = 0;

    static std::vector<std::pair<uint64_t, GLDrawListEntry*>> rvec;

    rvec.clear();


    // TODO: change this to using iterators
    GLDrawListEntry* robj = draw_list.GetFirst();
    GLDrawListEntry* rlast = draw_list.GetLast();

    for(;robj < rlast; robj++){
        if(*((uint64_t*)robj) == 0) continue;

        // TODO: do view culling in here

        rvec.push_back(std::pair<uint64_t, GLDrawListEntry*>(robj->CalcSortKey(LAYER[0].view_position), robj));
    }

    sort(rvec.begin(), rvec.end());

    for (std::pair<uint64_t, GLDrawListEntry*>& pp : rvec){
        GLDrawListEntry* robj = pp.second;

        /*mat4 model = mat4(1.0f);
        model = glm::translate(model, vec3(robj->location[0], robj->location[1], robj->location[2]));
        model *= glm::toMat4(robj->rotation);*/

        glUseProgram(robj->shader);

        if(robj->pose != nullptr){
            UploadUniformBuffer(bone_uniform_buffer, sizeof(Pose), glm::value_ptr(robj->pose->pose[0]));
        }

        for (int i = 0; i < 15; i++) {
            modelMatrices.colors[i] = robj->colors[i];
            modelMatrices.specular[i].x = robj->specular_weights[i];
            modelMatrices.specular[i].y = robj->specular_exponents[i];
            modelMatrices.specular[i].z = robj->specular_transparencies[i];
        }

        //AddLine(vec3(robj->matrix * vec4(0.0f, 0.0f, 0.0f, 1.0f)), light_list.GetFirst()[robj->lights[0]].location, light_list.GetFirst()[robj->lights[0]].color);
        //AddLine(vec3(robj->matrix * vec4(0.0f, 0.0f, 0.0f, 1.0f)), light_list.GetFirst()[robj->lights[1]].location, light_list.GetFirst()[robj->lights[1]].color);
        //AddLine(vec3(robj->matrix * vec4(0.0f, 0.0f, 0.0f, 1.0f)), light_list.GetFirst()[robj->lights[2]].location, light_list.GetFirst()[robj->lights[2]].color);
        //AddLine(vec3(robj->matrix * vec4(0.0f, 0.0f, 0.0f, 1.0f)), light_list.GetFirst()[robj->lights[3]].location, light_list.GetFirst()[robj->lights[3]].color);

        modelMatrices.modelLights.x = robj->lights[0];
        modelMatrices.modelLights.y = robj->lights[1];
        modelMatrices.modelLights.z = robj->lights[2];
        modelMatrices.modelLights.w = robj->lights[3];

        if (robj->flags & FLAG_NO_DIRECTIONAL) {
            modelMatrices.sunWeight = 0.0f;
        } else {
            modelMatrices.sunWeight = 1.0f;
        }

        //modelMatrices.model = model;
        modelMatrices.model = robj->matrix;
        UploadUniformBuffer(model_matrix_uniform_buffer, sizeof(ShaderUniformModelMatrices), &modelMatrices);



        for (unsigned int j = 0; j < robj->texCount; j++){
            glActiveTexture(GL_TEXTURE0 + j);
            glBindTexture(GL_TEXTURE_2D, robj->textures[j]);
        }

        if(robj->lightmap != 0){
            glActiveTexture(GL_TEXTURE15);
            glBindTexture(GL_TEXTURE_2D, robj->lightmap);
        }
        
        
        if (layer != robj->layer) {
            // *whatever opengl call clears the depth buffer*
            layer = robj->layer;
        }

        if (robj->flags & FLAG_NO_DEPTH_TEST) glDisable(GL_DEPTH_TEST);
        if (robj->flags & FLAG_DRAW_INDEXED) {
            glBindVertexArray(robj->vao);
            glDrawElements(robj->flags & FLAG_DRAW_LINES ? GL_LINES : GL_TRIANGLES, robj->eboLen * 3, GL_UNSIGNED_INT, (void*)(robj->eboOff * 3 * sizeof(uint32_t)));
        } else {
            glBindVertexArray(robj->vao);
            glDrawArrays(robj->flags & FLAG_DRAW_LINES ? GL_LINES : GL_TRIANGLES, 0, robj->eboLen);
        }
        if (robj->flags & FLAG_NO_DEPTH_TEST) glEnable(GL_DEPTH_TEST);


    }
}

drawlistentry_t InsertDrawListEntry() {
    return {.generic = draw_list.AddNew()};
}

void RemoveDrawListEntry(drawlistentry_t entry) {
    draw_list.Remove(entry.gl);
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

void SetDrawListColors(drawlistentry_t entry, size_t count, vec4* colors) {
    for (size_t i = 0; i < count; i++) {
        entry.gl->colors[i] = colors[i];
    }
}

void SetDrawListSpecularities(drawlistentry_t entry, size_t count, float* weights, float* exponents, float* transparencies) {
    for (size_t i = 0; i < count; i++) {
        entry.gl->specular_weights[i] = weights[i];
        entry.gl->specular_exponents[i] = exponents[i];
        entry.gl->specular_transparencies[i] = transparencies[i];
    }
}

void SetLights(drawlistentry_t entry, light_t* lights) {
    /*for (size_t i = 0; i < 4; i++) {
        entry.gl->lights[i] = light_list.index(lights[i].gl);
    }*/
}

void SetMatrix(drawlistentry_t entry, const mat4& matrix) {
    vec4 origin = matrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    
    entry.gl->matrix = matrix;
    
    light_tree.FindNearest(entry.gl->lights, origin.x, origin.y, origin.z);
}

void SetDrawListVertexArray(drawlistentry_t entry, vertexarray_t vertex_array_handle) {
    entry.gl->vao = vertex_array_handle.gl_vertex_array;
}

void SetDrawListIndexArray(drawlistentry_t entry, indexarray_t index_array_handle) {
    // the index array is already bound to the vao
}

void SetDrawListIndexRange(drawlistentry_t entry, uint32_t index_offset, uint32_t index_length) {
    entry.gl->eboOff = index_offset;
    entry.gl->eboLen = index_length;
}

void SetDrawListShader(drawlistentry_t entry, vertexformat_t vertex_format, materialtype_t material_type) {
    entry.gl->shader = FindShader(vertex_format, material_type);
}

void SetDrawListTextures(drawlistentry_t entry, size_t texture_count, texturehandle_t* textures) {
    for (size_t i = 0; i < texture_count; i++) {
        entry.gl->textures[i] = textures[i].gl_texture_handle;
    }
    entry.gl->texCount = texture_count;
}

light_t MakeLight() {
    GLLight* light = light_list.AddNew();
    uint32_t light_id = light - light_list.GetFirst();
    uint32_t leaf_id = light_tree.AddLeaf(light_id, 0.0f, 0.0f, 0.0f);
    
    light_tree_ids [light_id] = leaf_id;
        
    return { .gl = light };
}

void DeleteLight(light_t light) {
    GLLight* light_ptr = light.gl;
    uint32_t light_id = light_ptr - light_list.GetFirst();
    uint32_t leaf_id = light_tree_ids [light_id];
    
    std::cout << "yeeted light with id " << light_id << std::endl;
    
    light_list.Remove(light_ptr);
    light_tree.RemoveLeaf(leaf_id);
}

void SetLightParameters(light_t light, vec3 location, vec3 color, float distance, vec3 direction, float exponent) {
    GLLight* light_ptr = light.gl;
    uint32_t light_id = light_ptr - light_list.GetFirst();
    uint32_t leaf_id = light_tree_ids [light_id];
    
    light_ptr->location = location;
    light_ptr->color = color;
    light_ptr->distance = distance;
    light_ptr->direction = direction;
    light_ptr->exponent = exponent;
    
    light_tree.RemoveLeaf(leaf_id);
    leaf_id = light_tree.AddLeaf(light_id, light_ptr->location.x, light_ptr->location.y, light_ptr->location.z);
    light_tree_ids [light_id] = leaf_id;
}


texturehandle_t CreateTexture(ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data) {
    uint32_t texture;
    uint32_t opengl_tex_format;
    
    switch (color_mode) {
        case COLORMODE_R:
            opengl_tex_format = GL_RED;
            break;
        case COLORMODE_RG:
            opengl_tex_format = GL_RG;
            break;
        case COLORMODE_RGB:
            opengl_tex_format = GL_RGB;
            break;
        case COLORMODE_RGBA:
            opengl_tex_format = GL_RGBA;
    }
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    switch (texture_filter) {
        case TEXTUREFILTER_NEAREST:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case TEXTUREFILTER_LINEAR:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case TEXTUREFILTER_LINEAR_MIPMAPPED:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
    }

    assert(data);

    glTexImage2D(GL_TEXTURE_2D, 0, opengl_tex_format, width, height, 0, opengl_tex_format, GL_UNSIGNED_BYTE, data);
        
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return {.gl_texture_handle = texture};
}

void CreateIndexedVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array, indexarray_t& index_array, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data) {
    glGenBuffers(1, &vertex_array.gl_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array.gl_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_size, vertex_data, GL_STATIC_DRAW);

    glGenBuffers(1, &index_array.gl_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_array.gl_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, index_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vertex_array.gl_vertex_array);

    glBindVertexArray(vertex_array.gl_vertex_array);

    for (size_t i = 0; i < vertex_format.attribute_count; i++) {
        uint32_t opengl_type = vertex_format.attributes[i].type == VertexAttribute::FLOAT32 ? GL_FLOAT : GL_UNSIGNED_INT;
        
        if (opengl_type == GL_FLOAT) {
            glVertexAttribPointer(i, vertex_format.attributes[i].size, opengl_type, GL_FALSE, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        } else {
            glVertexAttribIPointer(i, vertex_format.attributes[i].size, opengl_type, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        }
        
        glEnableVertexAttribArray(i);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_array.gl_index_buffer);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CreateVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array) {
    glGenBuffers(1, &vertex_array.gl_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array.gl_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &vertex_array.gl_vertex_array);

    glBindVertexArray(vertex_array.gl_vertex_array);

    for (size_t i = 0; i < vertex_format.attribute_count; i++) {
        uint32_t opengl_type = vertex_format.attributes[i].type == VertexAttribute::FLOAT32 ? GL_FLOAT : GL_UNSIGNED_INT;
        
        if (opengl_type == GL_FLOAT) {
            glVertexAttribPointer(i, vertex_format.attributes[i].size, opengl_type, GL_FALSE, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        } else {
            glVertexAttribIPointer(i, vertex_format.attributes[i].size, opengl_type, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        }
        
        glEnableVertexAttribArray(i);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void UpdateVertexArray(vertexarray_t& vertex_array, size_t data_size, void* data) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array.gl_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void SetViewMatrix(const mat4& matrix, layer_t layer) {
    LAYER[layer].view_matrix = matrix;
    LAYER[layer].view_position = glm::inverse(matrix) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void SetProjectionMatrix(const mat4& matrix, layer_t layer) {
    LAYER[layer].projection_matrix = matrix;
}

void GetScreen(char* buffer, int w, int h) {
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer);
}

#ifdef __WIN32
void APIENTRY RenderErrorCallback(uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int32_t length, const char* message, const void*) {
    // apparently these are spammy, or something
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    const char* source_str =    "UNKNOWN";
    const char* type_str =      "UNKNOWN";
    const char* severity_str =  "UNKNOWN";

    switch (source) {
        case GL_DEBUG_SOURCE_API_ARB:               source_str = "API";                 break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:     source_str = "WINDOW_SYSTEM";       break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:   source_str = "SHADER_COMPILER";     break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:       source_str = "THIRD_PARTY";         break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB:       source_str = "APPLICATION";         break;
        case GL_DEBUG_SOURCE_OTHER_ARB:             source_str = "OTHER";               break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR_ARB:               type_str = "ERROR";                 break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: type_str = "DEPRECATED_BEHAVIOR";   break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:  type_str = "UNDEFINED_BEHAVIOR";    break; 
        case GL_DEBUG_TYPE_PORTABILITY_ARB:         type_str = "PORTABILITY";           break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:         type_str = "PERFORMANCE";           break;
        case GL_DEBUG_TYPE_OTHER_ARB:               type_str = "OTHER";                 break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH_ARB:            severity_str = "HIGH";              break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB:          severity_str = "MEDIUM";            break;
        case GL_DEBUG_SEVERITY_LOW_ARB:             severity_str = "LOW";               break;
    }

    std::cout << "OpenGL Debug Message: " << source_str << " " << type_str << " " << severity_str << " " << id << std::endl;
    std::cout << message << std::endl;
}
#endif

void Init() {
    #ifdef __WIN32
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB); 
        glDebugMessageCallbackARB(RenderErrorCallback, nullptr);
        glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    #endif
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    CompileShaders();

    light_uniform_buffer = MakeUniformBuffer("Lights", light_uniform_binding, sizeof(GLLight)*50);
    matrix_uniform_buffer = MakeUniformBuffer("Matrices", matrix_uniform_binding, sizeof(ShaderUniformMatrices));
    model_matrix_uniform_buffer = MakeUniformBuffer("ModelMatrices", model_matrix_uniform_binding, sizeof(ShaderUniformModelMatrices));
    bone_uniform_buffer = MakeUniformBuffer("Bones", bone_uniform_binding, sizeof(Pose));
    
    //matrices.projection = glm::perspective(glm::radians(60.0f), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.0f);

    // initialize the default pose
    BLANK_POSE = PoolProxy<Render::Pose>::New();
    for (size_t i = 0; i < BONE_COUNT; i++) {
        BLANK_POSE->pose[i] = mat4(1.0f);
    }
    
    // initialize the default light
    //new (light_list.begin().ptr) LightListEntry;
    light_list.AddNew();
}

ContextType GetContext() {
#ifndef __EMSCRIPTEN__
    return CONTEXT_OPENGL;
#else
    return CONTEXT_WEBGL;
#endif
}

uint32_t GetMaxIndexRangeLength() {
    return 15;
}

void SetDevice(void*) {
    // OpenGL doesn't need a device!
}

}
