// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/opengl/renderer.h>

#include <render/opengl/light.h>
#include <render/opengl/shader.h>
#include <render/opengl/drawlist.h>

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

/* what's up and what's down?
 * 
 * the original model for rendering, i.e. creating a big list and then sorting
 * it and then going through the list and rendering stuff is not working that
 * well anymore.
 * 
 * which means that we should probaly change it to something better.
 * TODO: investigate
 * 
 * Layer is a class, when you SetLayer you add a pointer to Layer??
 * each Layer would have a bucket for .. ? opaques and transparents!!
 */

namespace tram::Render::API {

//Pool<GLDrawListEntry> draw_list ("render list", 1000);

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
    vec4 l00;
    vec4 l1m1; vec4 l10; vec4 l11;
    vec4 l2m2; vec4 l2m1; vec4 l20; vec4 l21; vec4 l22;
};

struct Layer {
    std::vector<std::pair<uint64_t, GLDrawListEntry*>> forward_bucket;
    std::vector<std::pair<uint64_t, GLDrawListEntry*>> transparency_bucket;
    
    mat4 projection_matrix = mat4(1.0f);
    mat4 view_matrix = mat4(1.0f);
    vec3 view_position = {1.0f, 1.0f, 1.0f};
    
    vec3 sun_direction = {0.0f, 1.0f, 0.0f};
    vec3 sun_color = {1.0f, 1.0f, 1.0f};
    vec3 ambient_color = {0.3f, 0.3f, 0.3f};
};

ShaderUniformMatrices matrices;
ShaderUniformModelMatrices modelMatrices;

static Layer layers[7];

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

static Settings::Property<bool> render_debug = {false, "renderer-debug", Settings::NONE};

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
    layers[layer].sun_direction = sun_direction;
    layers[layer].sun_color = sun_color;
    layers[layer].ambient_color = ambient_color;
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


static void SetupLayer(layer_t layer) {
    modelMatrices.sunDirection =    vec4(layers[layer].sun_direction, 1.0f);
    modelMatrices.sunColor =        vec4(layers[layer].sun_color, 1.0f);
    modelMatrices.ambientColor =    vec4(layers[layer].ambient_color, 1.0f);

    matrices.projection = layers[layer].projection_matrix;
    matrices.view = layers[layer].view_matrix;
    matrices.view_pos = layers[layer].view_position;

    UploadUniformBuffer(matrix_uniform_buffer, sizeof(ShaderUniformMatrices), &matrices);
}

static void Draw(GLDrawListEntry* robj) {
    if (render_debug && !(robj->flags & FLAG_NO_DEBUG)) {
        char debug_text[250];
        
        uint32_t tex_hash = 0;
        for (uint32_t tex = 0; tex < robj->texCount; tex++) {
            // TODO: figure out why we get nullptrs here
            // maybe because of lines???
            if (!robj->materials[tex]) continue;
            tex_hash ^= robj->materials[tex]->gl_texture;
        }
        
        sprintf(debug_text, "Layer: %i\nVAO: %i, [%i:%i]\nTexture: %i (%i)\nLightmap: %i\nEnvironment: %i\nLights: %i %i %i %i\nPose: %i\nSize: %.2f\nFade: %.2f -> %.2f",
            robj->layer, robj->vao, robj->eboOff, robj->eboLen, robj->texCount, tex_hash, robj->lightmap, robj->environmentmap,
            robj->lights[0], robj->lights[1], robj->lights[2], robj->lights[3], robj->pose ? (int)PoolProxy<Pose>::GetPool().index(robj->pose) : 0, glm::distance(robj->aabb_min, robj->aabb_max),
            robj->fade_near, robj->fade_far);
        
        vec3 pos = robj->matrix * glm::vec4(glm::mix(robj->aabb_min, robj->aabb_max, 0.5f), 1.0f);
        
        AddText(pos, debug_text);
    }

    glUseProgram(robj->shader);

    if (robj->pose) {
        UploadUniformBuffer(bone_uniform_buffer, sizeof(Pose), glm::value_ptr(robj->pose->pose[0]));
    } else {
        UploadUniformBuffer(bone_uniform_buffer, sizeof(Pose), glm::value_ptr(null_pose->pose[0]));
    }

    for (int i = 0; i < 15; i++) {
        modelMatrices.colors[i] = robj->colors[i];
        modelMatrices.texture_transforms[i] = robj->texture_transforms[i];
        
        if (!robj->materials[i]) continue;
        
        modelMatrices.specular[i].x = robj->materials[i]->specular_weight;
        modelMatrices.specular[i].y = robj->materials[i]->specular_exponent;
        modelMatrices.specular[i].z = robj->materials[i]->specular_transparency;
        modelMatrices.specular[i].w = robj->materials[i]->reflectivity;
    }
    
    modelMatrices.l00 = vec4(robj->harmonic.l00, 0.0f);
    modelMatrices.l1m1 = vec4(robj->harmonic.l1m1, 0.0f);
    modelMatrices.l10 = vec4(robj->harmonic.l10, 0.0f);
    modelMatrices.l11 = vec4(robj->harmonic.l11, 0.0f);
    modelMatrices.l2m2 = vec4(robj->harmonic.l2m2, 0.0f);
    modelMatrices.l2m1 = vec4(robj->harmonic.l2m1, 0.0f);
    modelMatrices.l20 = vec4(robj->harmonic.l20, 0.0f);
    modelMatrices.l21 = vec4(robj->harmonic.l21, 0.0f);
    modelMatrices.l22 = vec4(robj->harmonic.l22, 0.0f);

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
        if (!robj->materials[j]) continue;
        glActiveTexture(GL_TEXTURE0 + j);
        glBindTexture(GL_TEXTURE_2D, robj->materials[j]->gl_texture);
    }

    if (robj->lightmap) {
        glActiveTexture(GL_TEXTURE15);
        glBindTexture(GL_TEXTURE_2D, robj->lightmap);
    }
    
    if (robj->environmentmap) {
        glActiveTexture(GL_TEXTURE15);
        glBindTexture(GL_TEXTURE_2D, robj->environmentmap);
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

void RenderFrame() {
    if (clear_screen) {
        glClearColor(screen_clear_color.x, screen_clear_color.y, screen_clear_color.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    
    // setting up data that is shared between layers
    modelMatrices.time = GetTickTime();
    modelMatrices.sunDirection =    vec4(layers[0].sun_direction, 1.0f);
    modelMatrices.sunColor =        vec4(layers[0].sun_color, 1.0f);
    modelMatrices.ambientColor =    vec4(layers[0].ambient_color, 1.0f);
    modelMatrices.screenWidth =     SCREEN_WIDTH;
    modelMatrices.screenHeight =    SCREEN_HEIGHT;

    matrices.projection = layers[0].projection_matrix;
    matrices.view = layers[0].view_matrix;
    matrices.view_pos = layers[0].view_position;


    GLLight* first_light = PoolProxy<GLLight>::GetPool().begin().ptr;
    GLLight* last_light = PoolProxy<GLLight>::GetPool().end().ptr;
    
    UploadUniformBuffer(light_uniform_buffer, sizeof(GLLight) * (last_light - first_light), first_light);


    // clear out layer buckets
    for (auto& layer : layers) {
        layer.forward_bucket.clear();
        layer.transparency_bucket.clear();
    }
    
    // filter drawlistentries and bucket them
    for (auto& robj : PoolProxy<GLDrawListEntry>::GetPool()) {
        const vec3 pos = robj.matrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
        const float dist = glm::distance(pos, layers[robj.layer].view_position);
        
        // distance culling
        if (dist > robj.fade_far || dist < robj.fade_near) {
            continue;
        }

        // frustum culling
        if (robj.flags & FLAG_USE_AABB) {
            auto matrix = layers[robj.layer].projection_matrix * layers[robj.layer].view_matrix;

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
            
            // stupid hack
            // TODO: fix
            vec3 origin = matrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
            
            if (glm::distance(origin, mid) > len) {
                if (glm::dot(plane_l, vec4(mid, 1.0f)) < -len) continue;
                if (glm::dot(plane_r, vec4(mid, 1.0f)) < -len) continue;
                if (glm::dot(plane_t, vec4(mid, 1.0f)) < -len) continue;
                if (glm::dot(plane_b, vec4(mid, 1.0f)) < -len) continue;
                if (glm::dot(plane_f, vec4(mid, 1.0f)) < -len) continue;
                if (glm::dot(plane_n, vec4(mid, 1.0f)) < -len) continue;
            }
        }

        const uint64_t sort_key = robj.CalcSortKey(layers[robj.layer].view_position);

        if (robj.flags & FLAG_TRANSPARENT) {
            layers[robj.layer].transparency_bucket.push_back({sort_key, &robj});
        } else {
            layers[robj.layer].forward_bucket.push_back({sort_key, &robj});
        }
    }

    // sort the buckets and add up the drawcalls
    Stats::Remove(Stats::RESOURCE_DRAWCALL, Stats::GetStat(Stats::RESOURCE_DRAWCALL));
    
    for (auto& layer : layers) {
        std::sort(layer.forward_bucket.begin(), layer.forward_bucket.end());
        std::sort(layer.transparency_bucket.begin(), layer.transparency_bucket.end());
        
        Stats::Add(Stats::RESOURCE_DRAWCALL, layer.forward_bucket.size());
        Stats::Add(Stats::RESOURCE_DRAWCALL, layer.transparency_bucket.size());
    }


    // the depth buffer was already cleared when we cleared the color buffer
    SetupLayer(LAYER_DEFAULT);
    for (auto [_, robj] : layers[LAYER_DEFAULT].forward_bucket) {
        Draw(robj);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    for (auto [_, robj] : layers[LAYER_DEFAULT].transparency_bucket) {
        Draw(robj);
    }
    glDisable(GL_BLEND); 
    
    // the overlay layer is drawn on top of the default layer
    glClear(GL_DEPTH_BUFFER_BIT);
    SetupLayer(LAYER_OVERLAY);
    
    for (auto [_, robj] : layers[LAYER_OVERLAY].forward_bucket) {
        Draw(robj);
    }
    
    // finally we draw the GUI widgets and debug text on top of everything
    glClear(GL_DEPTH_BUFFER_BIT);
    SetupLayer(LAYER_GUI);
    
    for (auto [_, robj] : layers[LAYER_GUI].forward_bucket) {
        Draw(robj);
    }
    
    
    if (render_debug) {
        for (auto& light : PoolProxy<GLLight>::GetPool()) {
            char debug_text[250];
            
            sprintf(debug_text, "Index: %i\nDistance: %.2f\nColor: %.2f %.2f %.2f\nDirection: %.2f %.2f %.2f\nExponent: %.2f",
                (int)PoolProxy<GLLight>::GetPool().index(&light),
                light.distance,
                light.color.r, light.color.g, light.color.b,
                light.direction.x, light.direction.y, light.direction.z,
                light.exponent);
            
            AddText(light.location, debug_text);
        }
    }
}







void SetViewMatrix(const mat4& matrix, layer_t layer) {
    layers[layer].view_matrix = matrix;
    layers[layer].view_position = glm::inverse(matrix) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void SetProjectionMatrix(const mat4& matrix, layer_t layer) {
    layers[layer].projection_matrix = matrix;
}

void GetScreen(char* buffer, int w, int h) {
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer);
}

bool IsInteractiveMode() {
    return true;
}

void SetInteractiveMode(bool is_interactive) {
    // opengl doesn't have a non-interactive mode
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

    //Settings::Register(render_debug, "renderer-debug", Settings::SERIALIZE_NONE);

    // initialize the default pose
    null_pose = PoolProxy<Render::Pose>::New();
    for (size_t i = 0; i < BONE_COUNT; i++) {
        null_pose->pose[i] = mat4(1.0f);
    }
    
    // initialize the default light
    //new (light_list.begin().ptr) LightListEntry;
    //light_list.AddNew();
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
