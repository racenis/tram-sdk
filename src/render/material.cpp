// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// MATERIAL.CPP
// Methods for the material resource.

// turning off an annoying warning in the library code
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

#pragma GCC diagnostic pop

#include <glad.h>
//#include <glad_gles3.h>

#include <framework/core.h>
#include <framework/stats.h>

#include <render/render.h>
#include <render/renderer.h>

#include <templates/hashmap.h>

#include <fstream>

using namespace Core;
using namespace Core::Render;

Material* Material::error_material = nullptr;
//std::unordered_map<uint64_t, Material*> Material::List;
Hashmap<Material*> MATERIAL_LIST("material name list", 500);
template <> Pool<Material> PoolProxy<Material>::pool("material pool", 500);

void Material::LoadMaterialInfo(const char* filename){
    using namespace Core::Render;

    std::ifstream file;
    file.open(filename);
    
    if (!file.is_open()) {
        std::cout << "Can't open texture info file '" << filename << "'" << std::endl;
        abort();
    }
    
    // TODO: make file parsing more efficient
    while(file){
        Material::Type mattype;
        std::string name;
        std::string type;

        file >> name;
        file >> type;
        if(type == "flat"){
            mattype = Material::TEXTURE;
        } else if(type == "alpha"){
            mattype = Material::TEXTURE_ALPHA;
        } else if(type == "lightmap"){
            mattype = Material::TEXTURE_LIGHTMAP;
        } else if(type == "msdf"){
            mattype = Material::TEXTURE_MSDF;
        } else if(type == "water"){
            mattype = Material::TEXTURE_WATER;
        } else {
            std::cout << "Error material list material: " << name << std::endl;
        }

        MATERIAL_LIST.Insert(UID(name), PoolProxy<Material>::New(UID(name), mattype));
    }
    
    file.close();
}

Material* Material::Find(name_t name){
    // APPARENTLY:
    // "something goes fucky-wucky and this thing doesn't work if you don't LoadFromDisk()"
    // TODO: check if it still happens
    Material* material = MATERIAL_LIST.Find(name);
    
    if (!material) {
        material = PoolProxy<Material>::New(name, TEXTURE_LIGHTMAP);
        MATERIAL_LIST.Insert(UID(name), material);
    }
    
    return material;
}

void Material::LoadFromDisk(){
    assert(status == UNLOADED);

    int loadwidth, loadheight, loadchannels;
    unsigned char* loadtexture = nullptr;
    char path[100];
    int channels;

    if(type == TEXTURE_LIGHTMAP){
        strcpy(path, "data/textures/lightmap/");
        channels = 3;
    } else if(type == TEXTURE_ALPHA){
        strcpy(path, "data/textures/");
        channels = 4;
    } else if(type == TEXTURE_MSDF || type == TEXTURE_GLYPH){
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

    if (loadtexture){
        //std::cout << "Loading: " << path << std::endl;
        width = loadwidth;
        height = loadheight;
        textureData = new uint8_t[width * height * channels];

        for (size_t i = 0; i < width * height * channels; i++){
            textureData[i] = loadtexture[i];
        }

        status = LOADED;

        stbi_image_free(loadtexture);

    } else {
        std::cout << "Texture " << name << " couldn't be loaded!" << std::endl;

        // copy the error texture
        texture = error_material->texture;
        status = READY;
        load_fail = true;
    }

}

void Material::LoadFromMemory(){
    using namespace Core::Render::OpenGL;
    assert(status == LOADED);

    if(type == TEXTURE_ALPHA || type == TEXTURE_MSDF || type == TEXTURE_GLYPH){
        texture = CreateTexture(COLORMODE_RGBA, TEXTUREFILTER_LINEAR, width, height, textureData);
    } else {
        texture = CreateTexture(COLORMODE_RGB, TEXTUREFILTER_LINEAR, width, height, textureData);
    }

    float approx_memory = width * height * channels;  // image size
    approx_memory = approx_memory * 1.3f;             // plus mipmaps
    approx_vram_usage = (size_t) approx_memory;
    //RESOURCE_VRAM_USAGE += approx_vram_usage;
    Stats::Add(Stats::RESOURCE_VRAM, approx_vram_usage);

    delete[] textureData;
    textureData = nullptr;

    status = READY;
    return;
}



