// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

// turning off an annoying warning in the library code
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

#pragma GCC diagnostic pop

#include <framework/core.h>
#include <framework/stats.h>

#include <framework/file.h>

#include <render/material.h>
#include <render/api.h>
#include <render/error.h>

#include <templates/hashmap.h>

#include <fstream>

using namespace tram;

template <> Pool<Render::Material> PoolProxy<Render::Material>::pool("material pool", 500);

namespace tram::Render {

static Hashmap<Material*> material_list ("material name list", 500);

/// Loads a Material definition file.
void Material::LoadMaterialInfo(const char* filename){
    using namespace tram::Render;
    
    char path [100] = "data/";
    strcat (path, filename);
    strcat (path, ".list");

    File file (path, MODE_READ);
    
    if (!file.is_open()) {
        std::cout << "Can't open texture info file '" << path << "'" << std::endl;
        return;
    }
    
    name_t file_type = file.read_name();
    
    if (file_type != UID("MATv1")) {
        std::cout << "Invalid material file type " << path << std::endl;
        return;
    }

    while(file.is_continue()){
        materialtype_t mat_type;

        name_t mat_name = file.read_name();
        name_t mat_type_name = file.read_name();

        if(mat_type_name == UID("flat")){
            mat_type = MATERIAL_TEXTURE;
        } else if(mat_type_name == UID("alpha")){
            mat_type = MATERIAL_TEXTURE_ALPHA;
        } else if(mat_type_name == UID("lightmap")){
            mat_type = MATERIAL_LIGHTMAP;
        } else if(mat_type_name == UID("msdf")){
            mat_type = MATERIAL_MSDF;
        } else if(mat_type_name == UID("glyph")){
            mat_type = MATERIAL_GLYPH;
        } else if(mat_type_name == UID("water")){
            mat_type = MATERIAL_WATER;
        } else {
            std::cout << "Error material list material: " << mat_name << std::endl;
            continue;
        }

        material_list.Insert(mat_name, PoolProxy<Material>::New(mat_name, mat_type));
    }
}

/// Creates a material.
/// If a Material already exists with that name, then the existing Material is returned.
/// @return Always returns a pointer to a Material.
Material* Material::Make (name_t name, materialtype_t type) {
    Material* material = material_list.Find(name);
    
    if (!material) {
        material = PoolProxy<Material>::New(name, type);
        material_list.Insert(UID(name), material);
    }
    
    return material;
}

/// Finds a Material.
/// Finds a Material by its associated name. If Material with that names does not
/// exist, then it is created (with the TEXTURE_LIGHTMAP type). In addition, for
/// a Material to be found, it must have been created with the LoadMaterialInfo(),
/// Make() or Find() methods.
/// @return Always returns a pointer to a Material.
Material* Material::Find(name_t name){
    // APPARENTLY:
    // "something goes fucky-wucky and this thing doesn't work if you don't LoadFromDisk()"
    // TODO: check if it still happens
    Material* material = material_list.Find(name);
    
    if (!material) {
        material = PoolProxy<Material>::New(name, MATERIAL_LIGHTMAP);
        material_list.Insert(UID(name), material);
    }
    
    return material;
}

/// Makes Material a pattern.
/// This method does the same thing as LoadFromDisk(), but instead of loading
/// the texture from disk, it will generate a 64x64 pixel checkerboard
/// pattern, like the one used for errored materials.
void Material::MakePattern(vec3 color1, vec3 color2) {
    assert(status == UNLOADED);
    
    width = 64;
    height = 64;
    channels = 3;

    texture_data = MakeNewErrorTexture (color1, color2);

    status = LOADED;
}

void Material::LoadFromDisk(){
    assert(status == UNLOADED);

    int loadwidth, loadheight, loadchannels;
    unsigned char* loadtexture = nullptr;
    char path[100];
    int channels;

    if(type == MATERIAL_LIGHTMAP){
        strcpy(path, "data/textures/lightmap/");
        channels = 3;
    } else if(type == MATERIAL_TEXTURE_ALPHA){
        strcpy(path, "data/textures/");
        channels = 4;
    } else if(type == MATERIAL_MSDF || type == MATERIAL_GLYPH){
        strcpy(path, "data/textures/ui/");
        channels = 4;
    } else {
        strcpy(path, "data/textures/");
        channels = 3;
    }

    strcat(path, name);
    strcat(path, ".png");

    stbi_set_flip_vertically_on_load(true);
    loadtexture = stbi_load(path, &loadwidth, &loadheight, &loadchannels, channels);

    if (loadtexture) {
        width = loadwidth;
        height = loadheight;
        texture_data = new uint8_t[width * height * channels];

        for (size_t i = 0; i < width * height * channels; i++) {
            texture_data[i] = loadtexture[i]; // what the fuck
        }

        status = LOADED;

        stbi_image_free(loadtexture);

    } else {
        std::cout << "Texture " << name << " (" << path << ") couldn't be loaded!" << std::endl;

        MakePattern ({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f});
        
        load_fail = true;
    }

}

void Material::LoadFromMemory(){
    assert(status == LOADED);

    if (type == MATERIAL_TEXTURE_ALPHA || type == MATERIAL_MSDF || type == MATERIAL_GLYPH) {
        texture = CreateTexture(COLORMODE_RGBA, TEXTUREFILTER_LINEAR, width, height, texture_data);
    } else {
        texture = CreateTexture(COLORMODE_RGB, TEXTUREFILTER_LINEAR, width, height, texture_data);
    }

    float approx_memory = width * height * channels;  // image size
    approx_memory = approx_memory * 1.3f;             // plus mipmaps
    
    approx_vram_usage = (size_t) approx_memory;
    
    Stats::Add(Stats::RESOURCE_VRAM, approx_vram_usage);

    delete[] texture_data;
    texture_data = nullptr;

    status = READY;
    return;
}

}



