// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/opengl/renderer.h>
#include <render/vertices.h>
#include <render/api.h>

#include <framework/settings.h>
#include <framework/stats.h>

#include <templates/octree.h>

#ifndef _WIN32
    #include <GL/gl.h>
	#include <GLES3/gl3.h>
#else
    #include <glad.h>
#endif

#include <glm/gtc/type_ptr.hpp>

namespace tram::Render::API {

Pool<GLDrawListEntry> draw_list ("render list", 1000, false);
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
    vec4 texture_transforms[15];
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

static Render::Pose* null_pose = nullptr;

bool render_debug = false;

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

    UploadUniformBuffer(matrix_uniform_buffer, sizeof(ShaderUniformMatrices), &matrices);
    UploadUniformBuffer(light_uniform_buffer, sizeof(GLLight)*50, light_list.begin().ptr);

    static uint32_t layer; layer = 0;

    static std::vector<std::pair<uint64_t, GLDrawListEntry*>> rvec;
    rvec.clear();

    for (auto& robj : draw_list) {
        const vec3 pos = robj.matrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
        const float dist = glm::distance(pos, LAYER[robj.layer].view_position);
        
        if (dist > robj.fade_far || dist < robj.fade_near) {
            continue;
        }

        if (robj.flags & FLAG_USE_AABB) {
            auto matrix = LAYER[robj.layer].projection_matrix * LAYER[robj.layer].view_matrix;
            
            vec4 plane_l = {matrix[0][3] - matrix[0][0], 
                            matrix[1][3] - matrix[1][0],
                            matrix[2][3] - matrix[2][0],
                            matrix[3][3] - matrix[3][0]};
            
            vec4 plane_r = {matrix[0][3] + matrix[0][0],
                            matrix[1][3] + matrix[1][0],
                            matrix[2][3] + matrix[2][0],
                            matrix[3][3] + matrix[3][0]};

            vec4 plane_t = {matrix[0][3] - matrix[0][1],
                            matrix[1][3] - matrix[1][1],
                            matrix[2][3] - matrix[2][1],
                            matrix[3][3] - matrix[3][1]};

            vec4 plane_b = {matrix[0][3] + matrix[0][1],
                            matrix[1][3] + matrix[1][1],
                            matrix[2][3] + matrix[2][1],
                            matrix[3][3] + matrix[3][1]};

            vec4 plane_f = {matrix[0][2],
                            matrix[1][2],
                            matrix[2][2],
                            matrix[3][2]};

            vec4 plane_n = {matrix[0][3] - matrix[0][2],
                            matrix[1][3] - matrix[1][2],
                            matrix[2][3] - matrix[2][2],
                            matrix[3][3] - matrix[3][2]};
            
            vec3 min = robj.aabb_min;
            vec3 max = robj.aabb_max;
            
            vec3 mid = robj.matrix * vec4(glm::mix(min, max, 0.5f), 1.0f);
            
            float len1 = glm::length(min);
            float len2 = glm::length(max);
            
            float len = len1 > len2 ? len1 : len2;
            
            if (glm::dot(plane_l, vec4(mid, 1.0f)) < -len) continue;
            if (glm::dot(plane_r, vec4(mid, 1.0f)) < -len) continue;
            if (glm::dot(plane_t, vec4(mid, 1.0f)) < -len) continue;
            if (glm::dot(plane_b, vec4(mid, 1.0f)) < -len) continue;
            if (glm::dot(plane_f, vec4(mid, 1.0f)) < -len) continue;
            if (glm::dot(plane_n, vec4(mid, 1.0f)) < -len) continue;
        }

        rvec.push_back(std::pair<uint64_t, GLDrawListEntry*>(robj.CalcSortKey(LAYER[robj.layer].view_position), &robj));
    }

    sort(rvec.begin(), rvec.end());

    Stats::Remove(Stats::RESOURCE_DRAWCALL, Stats::GetStat(Stats::RESOURCE_DRAWCALL));
    Stats::Add(Stats::RESOURCE_DRAWCALL, rvec.size());

    for (auto [_, robj] : rvec) {
        
        if (render_debug && !(robj->flags & FLAG_NO_DEBUG)) {
            char debug_text[250];
            
            uint32_t tex_hash = 0;
            for (uint32_t tex = 0; tex < robj->texCount; tex++) {
                tex_hash ^= robj->textures[tex];
            }
            
            sprintf(debug_text, "Layer: %i\nVAO: %i, [%i:%i]\nTexture: %i (%i)\nLightmap: %i\nLights: %i %i %i %i\nPose: %i\nSize: %.2f\nFade: %.2f -> %.2f",
                robj->layer, robj->vao, robj->eboOff, robj->eboLen, robj->texCount, tex_hash, robj->lightmap,
                robj->lights[0], robj->lights[1], robj->lights[2], robj->lights[3], robj->pose ? (int)PoolProxy<Pose>::GetPool().index(robj->pose) : 0, glm::distance(robj->aabb_min, robj->aabb_max),
                robj->fade_near, robj->fade_far);
            
            vec3 pos = robj->matrix * glm::vec4(glm::mix(robj->aabb_min, robj->aabb_max, 0.5f), 1.0f);
            
            AddText(pos, debug_text);
        }
        
        if (layer != robj->layer) {
            layer = robj->layer;
            
            glClear(GL_DEPTH_BUFFER_BIT);
            
            modelMatrices.sunDirection =    vec4(LAYER[layer].sun_direction, 1.0f);
            modelMatrices.sunColor =        vec4(LAYER[layer].sun_color, 1.0f);
            modelMatrices.ambientColor =    vec4(LAYER[layer].ambient_color, 1.0f);

            matrices.projection = LAYER[layer].projection_matrix;
            matrices.view = LAYER[layer].view_matrix;
            matrices.view_pos = LAYER[layer].view_position;

            UploadUniformBuffer(matrix_uniform_buffer, sizeof(ShaderUniformMatrices), &matrices);
        }

        glUseProgram(robj->shader);

        if (robj->pose) {
            UploadUniformBuffer(bone_uniform_buffer, sizeof(Pose), glm::value_ptr(robj->pose->pose[0]));
        } else {
            UploadUniformBuffer(bone_uniform_buffer, sizeof(Pose), glm::value_ptr(null_pose->pose[0]));
        }

        for (int i = 0; i < 15; i++) {
            modelMatrices.colors[i] = robj->colors[i];
            modelMatrices.specular[i].x = robj->specular_weights[i];
            modelMatrices.specular[i].y = robj->specular_exponents[i];
            modelMatrices.specular[i].z = robj->specular_transparencies[i];
            modelMatrices.texture_transforms[i] = robj->texture_transforms[i];
            /*modelMatrices.texture_transforms[i].x = robj->texture_transforms[i][0][0];
            modelMatrices.texture_transforms[i].y = robj->texture_transforms[i][0][1];
            modelMatrices.texture_transforms[i].z = robj->texture_transforms[i][1][0];
            modelMatrices.texture_transforms[i].w = robj->texture_transforms[i][1][1];*/
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
    
    if (render_debug) {
        for (auto& light : light_list) {
            char debug_text[250];
            
            sprintf(debug_text, "Index: %i\nDistance: %.2f\nColor: %.2f %.2f %.2f\nDirection: %.2f %.2f %.2f\nExponent: %.2f",
                (int)light_list.index(&light),
                light.distance,
                light.color.r, light.color.g, light.color.b,
                light.direction.x, light.direction.y, light.direction.z,
                light.exponent);
            
            AddText(light.location, debug_text);
        }
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

void SetDrawListAABB(drawlistentry_t entry, vec3 min, vec3 max) {
    entry.gl->aabb_min = min;
    entry.gl->aabb_max = max;
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

void SetDrawListTextureOffsets(drawlistentry_t entry, size_t count, vec4* offset) {
    for (size_t i = 0; i < count; i++) {
        entry.gl->texture_transforms[i] = offset[i];
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
    
    // wait do we need this even?
    light_tree.FindNearest(entry.gl->lights, origin.x, origin.y, origin.z);
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

spritearray_t CreateSpriteArray() {
    vertexarray_t vertex_array;
    CreateVertexArray(GetVertexDefinition(VERTEX_SPRITE), vertex_array);
    
    return {vertex_array};
}


/*
struct SpritePoint {
    vec3 position;
    vec3 center;
    vec3 color;
    float rotation;
    vec2 dimensions;
    vec2 texture_offset;
    vec2 texture_size;
    uint32_t texture;
};
 
struct SpriteVertex {
    vec3 co;            //< Sprite position in object space.
    vec2 voffset;       //< Vertex offset in projection space.
    vec2 texco;         //< Vertex texture coordinates.
    vec3 color;         //< Vertex color.
    float verticality;  //< I don't remember.
    uint32_t texture;   //< Vertex texture material index.
};

 */

void UpdateSpriteArray(spritearray_t array, size_t data_size, void* data) {
    SpritePoint* sprites = (SpritePoint*) data;
    
    std::vector<Render::SpriteVertex> vertices;
    
    for (size_t i = 0; i < data_size; i++) {
        const SpritePoint& sprite = sprites[i];
        
        // TODO: make rotation do something
        // TODO: make origin do something
        
        Render::SpriteVertex top_left {
            .co =           sprite.position,
            .voffset =      {sprite.dimensions.x * -0.5f, sprite.dimensions.y * 0.5f},
            .texco =        {sprite.texture_offset.x, sprite.texture_offset.y + sprite.texture_size.y},
            .verticality =  1.0f,
            .texture =      sprite.texture  
        };
        
        Render::SpriteVertex top_right {
            .co =           sprite.position,
            .voffset =      {sprite.dimensions.x * 0.5f, sprite.dimensions.y * 0.5f},
            .texco =        {sprite.texture_offset.x + sprite.texture_size.x, sprite.texture_offset.y + sprite.texture_size.y},
            .verticality =  1.0f,
            .texture =      sprite.texture
        };
        
        Render::SpriteVertex bottom_left {
            .co =           sprite.position,
            .voffset =      {sprite.dimensions.x * -0.5f, sprite.dimensions.y * -0.5f},
            .texco =        {sprite.texture_offset.x, sprite.texture_offset.y},
            .verticality =  1.0f,
            .texture =      sprite.texture
        };
        
        Render::SpriteVertex bottom_right {
            .co =           sprite.position,
            .voffset =      {sprite.dimensions.x * 0.5f, sprite.dimensions.y * -0.5f},
            .texco =        {sprite.texture_offset.x + sprite.texture_size.x, sprite.texture_offset.y},
            .verticality =  1.0f,
            .texture =      sprite.texture
        };

        vertices.push_back(top_left);
        vertices.push_back(bottom_left);
        vertices.push_back(top_right);
        vertices.push_back(top_right);
        vertices.push_back(bottom_left);
        vertices.push_back(bottom_right);
    }
    
    UpdateVertexArray(array.vertex_array, sizeof(SpriteVertex) * vertices.size(), &vertices[0]);
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

#ifdef _WIN32
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
    #ifdef _WIN32
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

    Settings::Register(&render_debug, "renderer-debug");

    // initialize the default pose
    null_pose = PoolProxy<Render::Pose>::New();
    for (size_t i = 0; i < BONE_COUNT; i++) {
        null_pose->pose[i] = mat4(1.0f);
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

bool IsDebugMode() {
    return render_debug;
}

void SetDebugMode(bool mode) {
    render_debug = mode;
}

}
