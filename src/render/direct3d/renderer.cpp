// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/api.h>
#include <render/direct3d/direct3d.h>

#include <d3dx9.h>

#include <templates/octree.h>

//#include <glm/gtc/type_ptr.hpp>

namespace tram::Render::API {

static IDirect3DDevice9* device = nullptr;
    
Pool<D3DDrawListEntry> draw_list("render list", 500, false);
Pool<D3DLight> light_list("light list", 200, false);
Octree<uint32_t> light_tree;
std::vector<uint32_t> light_tree_ids (200);
/*
struct ShaderUniformMatrices {
    glm::mat4 projection;       /// Projection matrix.
    glm::mat4 view;             /// View matrix.
    glm::vec3 view_pos;
};

struct ShaderUniformModelMatrices {
    glm::mat4 model;        /// Model -> world space matrix. Rotates and translates vertices from how they are defined in the model to where they will appear in the world.
    glm::uvec4 modelLights; /// Indices for lights in the light list. The shader will use these 4 indices to determine with which lights the model should be lit up.
    glm::vec4 sunDirection; /// Normalized vector. Sunlight direction.
    glm::vec4 sunColor;     /// Sunlight color.
    glm::vec4 ambientColor; /// Shadow color.
    float time;
    float sunWeight;
    float screenWidth;
    float screenHeight;
    glm::vec4 colors[15];
    glm::vec4 specular[15];
    float padding[30];
};*/

struct LayerParameters {
    mat4 projection_matrix = mat4(1.0f);
    mat4 view_matrix = mat4(1.0f);
    vec3 view_position = {1.0f, 1.0f, 1.0f};
    
    vec3 sun_direction = {0.0f, 1.0f, 0.0f};
    vec3 sun_color = {1.0f, 1.0f, 1.0f};
    vec3 ambient_color = {0.3f, 0.3f, 0.3f};
};
/*
ShaderUniformMatrices matrices;
ShaderUniformModelMatrices modelMatrices;
*/
static LayerParameters layers[7];
/*
class ShaderBuffer {};

const uint32_t matrix_uniform_binding = 0;
const uint32_t model_matrix_uniform_binding = 1;
const uint32_t light_uniform_binding = 2;
const uint32_t bone_uniform_binding = 3;

uint32_t matrix_uniform_buffer;
uint32_t model_matrix_uniform_buffer;
uint32_t light_uniform_buffer;
uint32_t bone_uniform_buffer;

*/
vec3 screen_clear_color = {0.2f, 0.3f, 0.3f};
bool clear_screen = true;

/*

static float SCREEN_WIDTH = 800.0f;
static float SCREEN_HEIGHT = 600.0f;*/


void SetLightingParameters (vec3 sun_direction, vec3 sun_color, vec3 ambient_color, uint32_t layer) {
    layers[layer].sun_direction = sun_direction;
    layers[layer].sun_color = sun_color;
    layers[layer].ambient_color = ambient_color;
}

void SetScreenSize(float width, float height) {
    /*SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    
    glViewport(0, 0, width, height);*/
    
    //matrices.projection = glm::perspective(glm::radians(60.0f), width / height, 0.1f, 1000.0f);
}

void SetScreenClear (vec3 clear_color, bool clear) {
    clear_screen = clear;
    screen_clear_color = clear_color;
}

static UINT FVFToStride(DWORD fvf);

void RenderFrame() {
    if (clear_screen) {
        int r = screen_clear_color.r * 255.0f;
        int g = screen_clear_color.g * 255.0f;
        int b = screen_clear_color.b * 255.0f;
        device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(r, g, b, 0), 1.0f, 0);
    } else {
        device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
    }
    
    //device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

    const vec3 a = layers[0].ambient_color;
    const vec3 c = layers[0].sun_color;
    const vec3 d = -layers[0].sun_direction;

    D3DLIGHT9 light;
    ::ZeroMemory(&light, sizeof(light));
    light.Type      = D3DLIGHT_DIRECTIONAL;
    light.Ambient   = D3DXCOLOR(a.r, a.g, a.b, 1.0f);
    light.Diffuse   = D3DXCOLOR(c.r, c.g, c.b, 1.0f);
    light.Specular  = D3DXCOLOR(c.r, c.g, c.b, 1.0f);
    light.Direction = D3DXVECTOR3(d.x, d.y, d.z);
    device->SetLight(4, &light);
    device->LightEnable(4, true);


    device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&layers[0].view_matrix);
    device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&layers[0].projection_matrix);
    
    device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

    device->BeginScene();

    for (auto& entry : draw_list) {
        if (!entry.vertex_buffer) continue;
        if (!entry.index_buffer) continue;
        
        if (entry.flags & FLAG_INTERIOR_LIGHTING) {
            device->LightEnable(4, false);
        } else {
            device->LightEnable(4, true);
        }
        
        if (entry.lightmap) {
            device->SetRenderState(D3DRS_LIGHTING, FALSE); 
        } else {
            device->SetRenderState(D3DRS_LIGHTING, TRUE); 
        }
        
        for (int i = 0; i < 4; i++) {
            if (entry.lights[i]) {
                const auto& params = light_list[entry.lights[i]];
                
                D3DLIGHT9 light;
                ::ZeroMemory(&light, sizeof(light));

                light.Type      = D3DLIGHT_POINT;
                light.Ambient   = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
                light.Diffuse   = D3DXCOLOR(params.color.r, params.color.g, params.color.b, 1.0f);
                light.Specular  = D3DXCOLOR(params.color.r, params.color.g, params.color.b, 1.0f);
                light.Position  = {params.location.x, params.location.y, params.location.z};
                light.Range        = params.distance;
                light.Falloff      = 1.0f;
                light.Attenuation0 = 1.0f;
                light.Attenuation1 = 0.09f;
                light.Attenuation2 = 0.032f;
                
                device->SetLight(i, &light);
                device->LightEnable(i, true);
            } else {
                device->LightEnable(i, false);
            }
        }
        
        device->SetStreamSource(0, entry.vertex_buffer, 0, FVFToStride(entry.fvf));
		device->SetIndices(entry.index_buffer);
		device->SetFVF(entry.fvf);
        
        D3DMATERIAL9 mtrl;
        mtrl.Ambient  = D3DXCOLOR(entry.color.r, entry.color.g, entry.color.b, 1.0f);
        mtrl.Diffuse  = D3DXCOLOR(entry.color.r, entry.color.g, entry.color.b, 1.0f);
        mtrl.Specular = D3DXCOLOR(entry.specular_weight * entry.color.r, entry.specular_weight * entry.color.g, entry.specular_weight * entry.color.b, 1.0f);
        mtrl.Emissive = D3DXCOLOR(D3DCOLOR_XRGB(0, 0, 0));
        mtrl.Power    = entry.specular_exponent;
        
        device->SetMaterial(&mtrl);
        
        device->SetTexture(0, entry.texture);
        device->SetTexture(1, entry.lightmap);
        
        //device->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
        device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
        
        device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&entry.matrix);
        device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, entry.vertex_count, entry.index_offset * 3, entry.index_length);
    }

		device->EndScene();
    
    
    
    
    
    
    
    
    
    
    /*if (clear_screen) {
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

    matrices.projection = LAYER[0].projection_matrix;
    matrices.view = LAYER[0].view_matrix;
    matrices.view_pos = LAYER[0].view_position;
    //matrices.view = glm::inverse(glm::translate(glm::mat4(1.0f), LAYER[0].camera_position) * glm::toMat4(LAYER[0].camera_rotation));
    //matrices.view_pos = LAYER[0].camera_position;

    //if (THIRD_PERSON) matrices.view = glm::translate(matrices.view, LAYER[0].camera_rotation * glm::vec3(0.0f, 0.0f, -5.0f));

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

        if (robj->flags & FLAG_INTERIOR_LIGHTING) {
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


    }*/
}

drawlistentry_t InsertDrawListEntry() {
    return drawlistentry_t{.d3d = draw_list.AddNew()};
}

void RemoveDrawListEntry(drawlistentry_t entry) {
    draw_list.Remove(entry.d3d);
}

uint32_t GetFlags(drawlistentry_t entry) {
    return entry.d3d->flags;
}

void SetFlags(drawlistentry_t entry, uint32_t flags) {
    entry.d3d->flags = flags;
}

void SetLayer(drawlistentry_t entry, uint32_t layer) {
    entry.d3d->layer = layer;
}

void SetPose(drawlistentry_t entry, Pose* pose) {
    entry.d3d->pose = pose;
}

void SetLightmap(drawlistentry_t entry, texturehandle_t lightmap) {
    entry.d3d->lightmap = lightmap.d3d_texture_handle;
}

void SetDrawListColors(drawlistentry_t entry, size_t count, vec4* colors) {
    entry.d3d->color = *colors;
}

void SetDrawListSpecularities(drawlistentry_t entry, size_t count, float* weights, float* exponents, float* transparencies) {
    entry.d3d->specular_weight = *weights;
    entry.d3d->specular_exponent = *exponents;
    entry.d3d->specular_transparency = *transparencies;
}

void SetLights(drawlistentry_t entry, uint32_t* lights) {
    for (size_t i = 0; i < 4; i++) {
        entry.d3d->lights[i] = lights[i];
    }
}

void SetMatrix(drawlistentry_t entry, const mat4& matrix) {
    vec4 origin = matrix * vec4(0.0f, 0.0f, 0.0f, 1.0f);
    
    entry.d3d->matrix = matrix;
    
    light_tree.FindNearest(entry.d3d->lights, origin.x, origin.y, origin.z);
}

void SetDrawListVertexArray(drawlistentry_t entry, vertexarray_t vertex_array_handle) {
    entry.d3d->vertex_buffer = vertex_array_handle.d3d_vertex_buffer;
    entry.d3d->fvf = vertex_array_handle.d3d_fvf;
    entry.d3d->vertex_count = vertex_array_handle.d3d_vertex_count;
}

void SetDrawListIndexArray(drawlistentry_t entry, indexarray_t index_array_handle) {
    entry.d3d->index_buffer = index_array_handle.d3d_index_buffer;
}

void SetDrawListIndexRange(drawlistentry_t entry, uint32_t index_offset, uint32_t index_length) {
    entry.d3d->index_offset = index_offset;
    entry.d3d->index_length = index_length;
}

void SetDrawListShader(drawlistentry_t entry, vertexformat_t vertex_format, materialtype_t material_type) {
    //entry.gl->shader = FindShader(vertex_format, material_type);
}

void SetDrawListTextures(drawlistentry_t entry, size_t texture_count, texturehandle_t* textures) {
    entry.d3d->texture = textures->d3d_texture_handle;
}

light_t MakeLight() {
    D3DLight* light = light_list.AddNew();
    uint32_t light_id = light - light_list.GetFirst();
    uint32_t leaf_id = light_tree.AddLeaf(light_id, 0.0f, 0.0f, 0.0f);
    
    light_tree_ids [light_id] = leaf_id;
        
    return light_t{ .d3d = light };
}

void DeleteLight(light_t light) {
    D3DLight* light_ptr = light.d3d;
    uint32_t light_id = light_ptr - light_list.GetFirst();
    uint32_t leaf_id = light_tree_ids [light_id];

    light_list.Remove(light_ptr);
    light_tree.RemoveLeaf(leaf_id);
}

void SetLightParameters(light_t light, vec3 location, vec3 color, float distance, vec3 direction, float exponent) {
    D3DLight* light_ptr = light.d3d;
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
    IDirect3DTexture9* texture = nullptr;
    D3DFORMAT texture_format;
    
    switch (color_mode) {
        case COLORMODE_R:
            texture_format = D3DFMT_L8;
            break;
        case COLORMODE_RG:
            texture_format = D3DFMT_A8L8;
            break;
        case COLORMODE_RGB:
            // we should be using D3DFMT_R8G8B8, but it fails on my computer
            texture_format = D3DFMT_A8R8G8B8;
            break;
        case COLORMODE_RGBA:
            texture_format = D3DFMT_A8R8G8B8;
    }
    
    texture_format = D3DFMT_A8R8G8B8;
    
    HRESULT result = device->CreateTexture(width, height, 1, 0, texture_format, D3DPOOL_MANAGED, &texture, 0);
    if (!texture) {
        switch (result) {
            case D3DERR_INVALIDCALL:
                std::cout << "Texture creation failed! (D3DERR_INVALIDCALL)" << std::endl;
                break;
            case D3DERR_OUTOFVIDEOMEMORY:
                std::cout << "Texture creation failed! (D3DERR_OUTOFVIDEOMEMORY)" << std::endl;
                break;
            case E_OUTOFMEMORY:
                std::cout << "Texture creation failed! (E_OUTOFMEMORY)" << std::endl;
                break;
            default:
                std::cout << "Texture creation failed! Error code: " << result << std::endl;
            } 
        abort();
    }

    D3DLOCKED_RECT rect;
    texture->LockRect(0, &rect, 0, D3DLOCK_DISCARD);
    unsigned char* dst = (unsigned char*)rect.pBits;
    unsigned char* src = (unsigned char*)data;
    
    switch (color_mode) {
        case COLORMODE_R:
            memcpy(dst, src, width * height * 1);
            break;
        case COLORMODE_RG:
            memcpy(dst, src, width * height * 2);
            break;
        case COLORMODE_RGB:
            // conversion from RGB to ARGB
            for (uint32_t i = 0; i < width * height; i++) {
                dst[i * 4 + 0] = src[i * 3 + 2];
                dst[i * 4 + 1] = src[i * 3 + 1];
                dst[i * 4 + 2] = src[i * 3 + 0];
                dst[i * 4 + 3] =  255;
            }
            break;
        case COLORMODE_RGBA:
            // conversion from RGBA to ARGB
            for (uint32_t i = 0; i < width * height; i++) {
                dst[i * 4 + 0] = src[i * 4 + 2];
                dst[i * 4 + 1] = src[i * 4 + 1];
                dst[i * 4 + 2] = src[i * 4 + 0];
                dst[i * 4 + 3] = src[i * 4 + 3];
            }
    }
    
    texture->UnlockRect(0);
    
    return texturehandle_t {.d3d_texture_handle = texture};
}

struct FVFHelper {
    int position = -1;
    int normal = -1;
    int texture = -1;
    int lightmap = -1;
    int bone_index = -1;
    int bone_weight = -1;
};

struct StaticVertex {
    float pos_x, pos_y, pos_z;
    float nrm_x, nrm_y, nrm_z;
    float tex_x, tex_y;
    float lit_x, lit_y;
};
const DWORD STATIC_VERTEX_FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2;

struct DynamicVertex {
    float pos_x, pos_y, pos_z;
    float nrm_x, nrm_y, nrm_z;
    float tex_x, tex_y;
};
const DWORD DYNAMIC_VERTEX_FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

static UINT FVFToStride(DWORD fvf) {
    switch (fvf) {
        case STATIC_VERTEX_FVF:     return sizeof(StaticVertex);
        case DYNAMIC_VERTEX_FVF:    return sizeof(DynamicVertex);
        default:                    return sizeof(float) * 3;
    }
}

void CreateIndexedVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array, indexarray_t& index_array, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data) {
    FVFHelper helper;
    for (int i = 0; i < (int)vertex_format.attribute_count; i++) {
        switch (vertex_format.attributes[i].ffp_type) {
            case VertexAttribute::FFP_POSITION:     helper.position = i;    break;
            case VertexAttribute::FFP_NORMAL:       helper.normal = i;      break;
            case VertexAttribute::FFP_TEXTURE:      helper.texture = i;     break;
            case VertexAttribute::FFP_LIGHTMAP:     helper.lightmap = i;    break;
            case VertexAttribute::FFP_BONE_INDEX:   helper.bone_index = i;  break;
            case VertexAttribute::FFP_BONE_WEIGHT:  helper.bone_weight = i; break;
            case VertexAttribute::FFP_IGNORE:                               break;
        }
    }
    
    DWORD fvf = 0;
    if (helper.position != -1 && helper.normal != -1 && helper.texture != -1) {
        fvf = DYNAMIC_VERTEX_FVF;
        vertex_array.d3d_fvf = DYNAMIC_VERTEX_FVF;
    }
    if (helper.position != -1 && helper.normal != -1 && helper.texture != -1 && helper.lightmap != -1) {
        fvf = STATIC_VERTEX_FVF;
        vertex_array.d3d_fvf = STATIC_VERTEX_FVF;
    }
    
    if (fvf == 0) {
        std::cout << "FVF could not be determined!" << std::endl;
        return;
    }
    
    const size_t vertex_count = vertex_size / vertex_format.attributes->stride;
    vertex_array.d3d_vertex_count = vertex_count;
    
    if (fvf == STATIC_VERTEX_FVF) {
        device->CreateVertexBuffer(
            vertex_count * sizeof(StaticVertex), 
            D3DUSAGE_WRITEONLY,
            fvf,
            D3DPOOL_MANAGED,
            &vertex_array.d3d_vertex_buffer,
            0);

        device->CreateIndexBuffer(
            index_size,
            D3DUSAGE_WRITEONLY,
            D3DFMT_INDEX32,
            D3DPOOL_MANAGED,
            &index_array.d3d_index_buffer,
            0);
        
        
        StaticVertex* parsed_data;
        vertex_array.d3d_vertex_buffer->Lock(0, 0, (void**)&parsed_data, 0);

        VertexAttribute pos_attrib = vertex_format.attributes[helper.position];
        for (size_t i = 0; i < vertex_count; i++) {
            float* param = (float*)((char*)vertex_data + pos_attrib.offset + pos_attrib.stride * i);
            parsed_data[i].pos_x = *param++;
            parsed_data[i].pos_y = *param++;
            parsed_data[i].pos_z = *param;
        }
        
        VertexAttribute nrm_attrib = vertex_format.attributes[helper.normal];
        for (size_t i = 0; i < vertex_count; i++) {
            float* param = (float*)((char*)vertex_data + nrm_attrib.offset + nrm_attrib.stride * i);
            parsed_data[i].nrm_x = *param++;
            parsed_data[i].nrm_y = *param++;
            parsed_data[i].nrm_z = *param;
        }
        
        VertexAttribute tex_attrib = vertex_format.attributes[helper.texture];
        for (size_t i = 0; i < vertex_count; i++) {
            float* param = (float*)((char*)vertex_data + tex_attrib.offset + tex_attrib.stride * i);
            parsed_data[i].tex_x = *param++;
            parsed_data[i].tex_y = *param;
        }
        
        VertexAttribute lit_attrib = vertex_format.attributes[helper.lightmap];
        for (size_t i = 0; i < vertex_count; i++) {
            float* param = (float*)((char*)vertex_data + lit_attrib.offset + lit_attrib.stride * i);
            parsed_data[i].lit_x = *param++;
            parsed_data[i].lit_y = *param;
        }
        
        vertex_array.d3d_vertex_buffer->Unlock();
        
        
        uint32_t* indices;
        index_array.d3d_index_buffer->Lock(0, 0, (void**)&indices, 0);
        memcpy(indices, index_data, index_size);
        index_array.d3d_index_buffer->Unlock();
    }
    
    if (fvf == DYNAMIC_VERTEX_FVF) {
        device->CreateVertexBuffer(
            vertex_count * sizeof(DynamicVertex), 
            D3DUSAGE_WRITEONLY,
            fvf,
            D3DPOOL_MANAGED,
            &vertex_array.d3d_vertex_buffer,
            0);

        device->CreateIndexBuffer(
            index_size,
            D3DUSAGE_WRITEONLY,
            D3DFMT_INDEX32,
            D3DPOOL_MANAGED,
            &index_array.d3d_index_buffer,
            0);
        
        DynamicVertex* parsed_data;
        vertex_array.d3d_vertex_buffer->Lock(0, 0, (void**)&parsed_data, 0);

        VertexAttribute pos_attrib = vertex_format.attributes[helper.position];
        for (size_t i = 0; i < vertex_count; i++) {
            float* param = (float*)((char*)vertex_data + pos_attrib.offset + pos_attrib.stride * i);
            parsed_data[i].pos_x = *param++;
            parsed_data[i].pos_y = *param++;
            parsed_data[i].pos_z = *param;
        }
        
        VertexAttribute nrm_attrib = vertex_format.attributes[helper.normal];
        for (size_t i = 0; i < vertex_count; i++) {
            float* param = (float*)((char*)vertex_data + nrm_attrib.offset + nrm_attrib.stride * i);
            parsed_data[i].nrm_x = *param++;
            parsed_data[i].nrm_y = *param++;
            parsed_data[i].nrm_z = *param;
        }
        
        VertexAttribute tex_attrib = vertex_format.attributes[helper.texture];
        for (size_t i = 0; i < vertex_count; i++) {
            float* param = (float*)((char*)vertex_data + tex_attrib.offset + tex_attrib.stride * i);
            parsed_data[i].tex_x = *param++;
            parsed_data[i].tex_y = *param;
        }
        
        vertex_array.d3d_vertex_buffer->Unlock();
        
        
        uint32_t* indices;
        index_array.d3d_index_buffer->Lock(0, 0, (void**)&indices, 0);
        memcpy(indices, index_data, index_size);
        index_array.d3d_index_buffer->Unlock();
    }
}

void CreateVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array) {
    vertex_array.d3d_vertex_buffer = nullptr;
    /*glGenBuffers(1, &vertex_array.gl_vertex_buffer);
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
    glBindBuffer(GL_ARRAY_BUFFER, 0);*/
}

void UpdateVertexArray(vertexarray_t vertex_array, size_t data_size, void* data) {
    /*glBindBuffer(GL_ARRAY_BUFFER, vertex_array.gl_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);*/
}

void SetViewMatrix(const mat4& matrix, layer_t layer) {
    layers[layer].view_matrix = matrix;
    layers[layer].view_position = glm::inverse(matrix) * vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void SetProjectionMatrix(const mat4& matrix, layer_t layer) {
    layers[layer].projection_matrix = matrix;
}

void GetScreen(char* buffer, int w, int h) {
    //glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer);
}

void Init() {
    device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
    
    device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    
    device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
    device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
    
    device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
    device->SetRenderState(D3DRS_SPECULARENABLE, true);
    
    // initialize the default pose
    BLANK_POSE = PoolProxy<Render::Pose>::New();
    for (size_t i = 0; i < BONE_COUNT; i++) {
        BLANK_POSE->pose[i] = mat4(1.0f);
    }
    
    // initialize the default light
    light_list.AddNew();
}

ContextType GetContext() {
    return CONTEXT_DIRECT3D;
}

uint32_t GetMaxIndexRangeLength() {
    return 1;
}

void SetDevice(void* new_device) {
    device = (IDirect3DDevice9*)new_device;
}

}
