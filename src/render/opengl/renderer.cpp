// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/opengl/renderer.h>
#include <render/opengl/api.h>

#include <templates/octree.h>

#ifdef __EMSCRIPTEN__
    #include <GL/gl.h>
	#include <GLES3/gl3.h>
#else
    #include <glad.h>
#endif

#include <glm/gtc/type_ptr.hpp>

namespace tram::Render::OpenGL {

Pool<DrawListEntry> draw_list ("render list", 500, false);
Pool<LightListEntry> light_list ("light list", 200, false);
Octree<uint32_t> light_tree;
std::vector<uint32_t> light_tree_ids (200);

struct ShaderUniformMatrices {
    glm::mat4 projection;       /// Projection matrix.
    glm::mat4 view;             /// View matrix.
};

struct ShaderUniformModelMatrices{
    glm::mat4 model;        /// Model -> world space matrix. Rotates and translates vertices from how they are defined in the model to where they will appear in the world.
    glm::uvec4 modelLights; /// Indices for lights in the light list. The shader will use these 4 indices to determine with which lights the model should be lit up.
    glm::vec4 sunDirection; /// Normalized vector. Sunlight direction.
    glm::vec4 sunColor;     /// Sunlight color.
    glm::vec4 ambientColor; /// Shadow color.
    float time;
    float sunWeight;
    float screenWidth;
    float screenHeight;
};

struct LayerParameters {
    vec3 camera_position = {0.0f, 0.0f, 0.0f};
    quat camera_rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    
    vec3 sun_direction = {0.0f, 1.0f, 0.0f};
    vec3 sun_color = {1.0f, 1.0f, 1.0f};
    vec3 ambient_color = {0.3f, 0.3f, 0.3f};
};

ShaderUniformMatrices matrices;
ShaderUniformModelMatrices modelMatrices;

static LayerParameters LAYER [7];

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

void SetCameraParameters (vec3 position, quat rotation, uint32_t layer) {
    LAYER[layer].camera_position = position;
    LAYER[layer].camera_rotation = rotation;
}

void SetScreenSize(float width, float height) {
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    
    glViewport(0, 0, width, height);
    
    matrices.projection = glm::perspective(glm::radians(90.0f), width / height, 0.1f, 1000.0f);
}

void SetScreenClear (vec3 clear_color, bool clear) {
    clear_screen = clear;
    screen_clear_color = clear_color;
}
    
void Init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    CompileShaders();

    light_uniform_buffer = MakeUniformBuffer("Lights", light_uniform_binding, sizeof(LightListEntry)*50);
    matrix_uniform_buffer = MakeUniformBuffer("Matrices", matrix_uniform_binding, sizeof(ShaderUniformMatrices));
    model_matrix_uniform_buffer = MakeUniformBuffer("ModelMatrices", model_matrix_uniform_binding, sizeof(ShaderUniformModelMatrices));
    bone_uniform_buffer = MakeUniformBuffer("Bones", bone_uniform_binding, sizeof(Pose));
    
    matrices.projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.0f);

    // initialize the default pose
    BLANK_POSE = PoolProxy<Render::Pose>::New();
    for (size_t i = 0; i < BONE_COUNT; i++) {
        BLANK_POSE->pose[i] = mat4(1.0f);
    }
    
    // initialize the default light
    //new (light_list.begin().ptr) LightListEntry;
    light_list.AddNew();
}


void RenderFrame() {
    if (clear_screen) {
        glClearColor(screen_clear_color.x, screen_clear_color.y, screen_clear_color.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    
    modelMatrices.time = GetTickTime();
    modelMatrices.sunDirection =    glm::vec4(LAYER[0].sun_direction, 1.0f);
    modelMatrices.sunColor =        glm::vec4(LAYER[0].sun_color, 1.0f);
    modelMatrices.ambientColor =    glm::vec4(LAYER[0].ambient_color, 1.0f);
    modelMatrices.screenWidth =     SCREEN_WIDTH;
    modelMatrices.screenHeight =    SCREEN_HEIGHT;
    
    matrices.view = glm::inverse(glm::translate(glm::mat4(1.0f), LAYER[0].camera_position) * glm::toMat4(LAYER[0].camera_rotation));

    if (THIRD_PERSON) matrices.view = glm::translate(matrices.view, LAYER[0].camera_rotation * glm::vec3(0.0f, 0.0f, -5.0f));

    UploadUniformBuffer(matrix_uniform_buffer, sizeof(ShaderUniformMatrices), &matrices);
    UploadUniformBuffer(light_uniform_buffer, sizeof(LightListEntry)*50, light_list.begin().ptr);

    static uint32_t layer; layer = 0;

    static std::vector<std::pair<uint64_t, DrawListEntry*>> rvec;

    rvec.clear();


    // TODO: change this to using iterators
    DrawListEntry* robj = draw_list.GetFirst();
    DrawListEntry* rlast = draw_list.GetLast();

    for(;robj < rlast; robj++){
        if(*((uint64_t*)robj) == 0) continue;

        // TODO: do view culling in here

        rvec.push_back(std::pair<uint64_t, DrawListEntry*>(robj->CalcSortKey(LAYER[0].camera_position), robj));
    }

    sort(rvec.begin(), rvec.end());

    for (std::pair<uint64_t, DrawListEntry*>& pp : rvec){
        DrawListEntry* robj = pp.second;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(robj->location[0], robj->location[1], robj->location[2]));

        model *= glm::toMat4(robj->rotation);

        glUseProgram(robj->shader);

        if(robj->pose != nullptr){
            UploadUniformBuffer(bone_uniform_buffer, sizeof(Pose), glm::value_ptr(robj->pose->pose[0]));
        }


        modelMatrices.modelLights.x = robj->lights[0];
        modelMatrices.modelLights.y = robj->lights[1];
        modelMatrices.modelLights.z = robj->lights[2];
        modelMatrices.modelLights.w = robj->lights[3];

        if (robj->flags & FLAG_INTERIOR_LIGHTING) {
            modelMatrices.sunWeight = 0.0f;
        } else {
            modelMatrices.sunWeight = 1.0f;
        }

        modelMatrices.model = model;
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
    return draw_list.AddNew();
}

void RemoveDrawListEntry(drawlistentry_t entry) {
    draw_list.Remove((DrawListEntry*) entry);
}

uint32_t GetFlags(drawlistentry_t entry) {
    return ((DrawListEntry*) entry)->flags;
}

void SetFlags(drawlistentry_t entry, uint32_t flags) {
    ((DrawListEntry*) entry)->flags = flags;
}

void SetPose(drawlistentry_t entry, Pose* pose) {
    ((DrawListEntry*) entry)->pose = pose;
}

void SetLightmap(drawlistentry_t entry, texturehandle_t lightmap) {
    ((DrawListEntry*) entry)->lightmap = lightmap;
}

void SetLights(drawlistentry_t entry, uint32_t* lights) {
    for (size_t i = 0; i < 4; i++) {
        ((DrawListEntry*) entry)->lights[i] = lights[i];
    }
}

void SetLocation(drawlistentry_t entry, glm::vec3& location) {
    DrawListEntry* entry_ptr = (DrawListEntry*) entry;
    entry_ptr->location = location;
    
    light_tree.FindNearest(entry_ptr->lights, location.x, location.y, location.z);
}

void SetRotation(drawlistentry_t entry, glm::quat& rotation) {
    ((DrawListEntry*) entry)->rotation = rotation;
}

void SetDrawListVertexArray(drawlistentry_t entry, vertexhandle_t vertex_array_handle) {
    ((DrawListEntry*) entry)->vao = vertex_array_handle;
}

void SetDrawListIndexRange(drawlistentry_t entry, uint32_t index_offset, uint32_t index_length) {
    ((DrawListEntry*) entry)->eboOff = index_offset;
    ((DrawListEntry*) entry)->eboLen = index_length;
}

void SetDrawListShader(drawlistentry_t entry, vertexformat_t vertex_format, materialtype_t material_type) {
    ((DrawListEntry*) entry)->shader = FindShader(vertex_format, material_type);
}

void SetDrawListTextures(drawlistentry_t entry, size_t texture_count, uint32_t* textures) {
    for (size_t i = 0; i < texture_count; i++) {
        ((DrawListEntry*) entry)->textures[i] = textures[i];
    }
    ((DrawListEntry*) entry)->texCount = texture_count;
}

light_t MakeLight() {
    LightListEntry* light = light_list.AddNew();
    uint32_t light_id = light - light_list.GetFirst();
    uint32_t leaf_id = light_tree.AddLeaf(light_id, 0.0f, 0.0f, 0.0f);
    
    light_tree_ids [light_id] = leaf_id;
        
    return light;
}

void DeleteLight(light_t light) {
    LightListEntry* light_ptr = (LightListEntry*) light;
    uint32_t light_id = light_ptr - light_list.GetFirst();
    uint32_t leaf_id = light_tree_ids [light_id];
    
    std::cout << "yeeted light with id " << light_id << std::endl;
    
    light_list.Remove(light_ptr);
    light_tree.RemoveLeaf(leaf_id);
}

void SetLightParameters (light_t light, vec3 location, vec3 color, float distance) {
    LightListEntry* light_ptr = (LightListEntry*) light;
    uint32_t light_id = light_ptr - light_list.GetFirst();
    uint32_t leaf_id = light_tree_ids [light_id];
    
    light_ptr->location = location;
    light_ptr->color = color;
    light_ptr->distance = distance;
    
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
    
    // TODO: add a similar switch() to the texture_filter, so that you can change them
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    assert(data);

    glTexImage2D(GL_TEXTURE_2D, 0, opengl_tex_format, width, height, 0, opengl_tex_format, GL_UNSIGNED_BYTE, data);
        
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return texture;
}

void CreateIndexedVertexArray(VertexDefinition vertex_format, vertexhandle_t& vertex_buffer_handle, vertexhandle_t& index_buffer_handle,  vertexhandle_t& vertex_array_handle, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data) {
    glGenBuffers(1, &vertex_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
    glBufferData(GL_ARRAY_BUFFER, vertex_size, vertex_data, GL_STATIC_DRAW);

    glGenBuffers(1, &index_buffer_handle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_handle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, index_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vertex_array_handle);

    glBindVertexArray(vertex_array_handle);

    for (size_t i = 0; i < vertex_format.attribute_count; i++) {
        uint32_t opengl_type = vertex_format.attributes[i].type == VertexAttribute::FLOAT32 ? GL_FLOAT : GL_UNSIGNED_INT;
        
        if (opengl_type == GL_FLOAT) {
            glVertexAttribPointer(i, vertex_format.attributes[i].size, opengl_type, GL_FALSE, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        } else {
            glVertexAttribIPointer(i, vertex_format.attributes[i].size, opengl_type, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        }
        
        glEnableVertexAttribArray(i);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_handle);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CreateVertexArray(VertexDefinition vertex_format, vertexhandle_t& vertex_buffer_handle,  vertexhandle_t& vertex_array_handle) {
    glGenBuffers(1, &vertex_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &vertex_array_handle);

    glBindVertexArray(vertex_array_handle);

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

void UpdateVertexArray(vertexhandle_t vertex_buffer_handle, size_t data_size, void* data) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
}

// why is this implemented in here
void tram::Render::Project(const glm::vec3& point, glm::vec3& result) {
    result = glm::project(point, OpenGL::matrices.view, OpenGL::matrices.projection, glm::vec4 (0.0f, 0.0f, OpenGL::SCREEN_WIDTH, OpenGL::SCREEN_HEIGHT));
    result.y = OpenGL::SCREEN_HEIGHT - result.y;
}

tram::vec3 tram::Render::ProjectInverse(tram::vec3 point) {
    point.y =  OpenGL::SCREEN_HEIGHT - point.y;
    vec3 result = glm::unProject(point, OpenGL::matrices.view, OpenGL::matrices.projection, glm::vec4 (0.0f, 0.0f, OpenGL::SCREEN_WIDTH, OpenGL::SCREEN_HEIGHT));
    //result.y = OpenGL::SCREEN_HEIGHT - result.y;
    
    return result;
}
