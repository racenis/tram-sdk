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

#include <core.h>
#include <render.h>

#include <fstream>

using namespace Core;
using namespace Core::Render;

Material* Material::error_material = nullptr;
std::unordered_map<uint64_t, Material> Material::List;

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

        List.emplace(UID(name), Material(UID(name), mattype));
    }
    
    file.close();
}

Material* Material::Find(name_t name){
    std::unordered_map<uint64_t, Material>::iterator ff = List.find(name);
    if(ff == List.end()){
        // something goes fucky-wucky and this thing doesn't work if you don't LoadFromDisk()
        // TODO: fix that
        List.emplace(name, Render::Material(name, TEXTURE_LIGHTMAP));
        ff = List.find(name);
    }

    Material* material = &ff->second;
    return material;
}

void Material::LoadFromDisk(){
    assert(status == UNLOADED);

    int loadwidth, loadheight, loadchannels;
    unsigned char* loadtexture = nullptr;
    char path[100];
    int channels;

    if(type == TEXTURE_LIGHTMAP){
        // TO DO: write that thing with the 2 images and merging them
        strcpy(path, "data/textures/lightmap/");
        channels = 3;
    } else if(type == TEXTURE_ALPHA){
        strcpy(path, "data/textures/");
        channels = 4;
    } else if(type == TEXTURE_MSDF){
        strcpy(path, "data/textures/ui/");
        channels = 4;
    } else {
        strcpy(path, "data/textures/");
        channels = 3;
    }

    strcat(path, ReverseUID(name));
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
        std::cout << "Texture " << ReverseUID(name) << " couldn't be loaded!" << std::endl;

        // copy the error texture
        texture = error_material->texture;
        status = READY;
        load_fail = true;
    }

}

void Material::LoadFromMemory(){
    assert(status == LOADED);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // this one can be set to GL_LINEAR for lower quality/faster rendering
    // maybe for lightmaps too
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    assert(textureData);

    if(type == TEXTURE_ALPHA || type == TEXTURE_MSDF){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
    }
    glGenerateMipmap(GL_TEXTURE_2D);

    delete[] textureData;
    textureData = nullptr;

    status = READY;
    return;
}



