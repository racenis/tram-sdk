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
    
    if (file_type != "MATv6") {
        std::cout << "Invalid material file type " << path << std::endl;
        abort();
    }

    while (file.is_continue()) {
        materialtype_t mat_type;
        MaterialFilter mat_filter;
        MaterialProperty mat_property;
        TextureType mat_tex_type;

        name_t mat_name = file.read_name();
        name_t mat_type_name = file.read_name();
        name_t mat_filter_name = file.read_name();
        name_t mat_property_name = file.read_name();
        vec3 mat_color = {file.read_float32(), file.read_float32(), file.read_float32()};
        float mat_spec_weight = file.read_float32();
        float mat_spec_exponent = file.read_float32();
        float mat_spec_transparency = file.read_float32();
        name_t mat_tex_type_name = file.read_name();
        Material* mat_source = nullptr;

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
            mat_type = FindMaterialType(mat_type_name);
            
            if (mat_type == (materialtype_t)-1) {
                std::cout << "Error material list material: " << mat_name << std::endl;
                continue;
            }
        }
        
        if (mat_filter_name == "linear") {
            mat_filter = FILTER_LINEAR;
        } else {
            mat_filter = FILTER_NEAREST;
        }
        
        if (mat_property_name == "metal") {
            mat_property = PROPERTY_METAL;
        } else if (mat_property_name == "metal-thin") {
            mat_property = PROPERTY_METAL_THIN;
        } else if (mat_property_name == "slime") {
            mat_property = PROPERTY_SLIME;
        } else if (mat_property_name == "tile") {
            mat_property = PROPERTY_TILE;
        } else if (mat_property_name == "grate") {
            mat_property = PROPERTY_GRATE;
        } else if (mat_property_name == "wood") {
            mat_property = PROPERTY_WOOD;
        } else if (mat_property_name == "computer") {
            mat_property = PROPERTY_COMPUTER;
        } else if (mat_property_name == "glass") {
            mat_property = PROPERTY_GLASS;
        } else if (mat_property_name == "snow") {
            mat_property = PROPERTY_SNOW;
        } else if (mat_property_name == "grass") {
            mat_property = PROPERTY_GRASS;
        } else if (mat_property_name == "concrete") {
            mat_property = PROPERTY_CONCRETE;
        } else if (mat_property_name == "flesh") {
            mat_property = PROPERTY_FLESH;
        } else {
            mat_property = PROPERTY_METAL;
        }

        if (mat_tex_type_name == "none") {
            mat_tex_type = TEXTURE_NONE;
        } else if (mat_tex_type_name == "same") {
            mat_tex_type = TEXTURE_SAME;
        } else {
            mat_tex_type = TEXTURE_SOURCE;
            mat_source = Material::Find(mat_tex_type_name);
        }

        Material* material = material_list.Find(mat_name);
    
        if (!material) {
            material = PoolProxy<Material>::New(mat_name);
            material_list.Insert(UID(mat_name), material);
        }

        material->SetMaterialType(mat_type);
        material->SetMaterialFilter(mat_filter);
        material->SetMaterialProperty(mat_property);
        material->SetColor(mat_color);
        material->SetSpecular(mat_spec_weight, mat_spec_exponent, mat_spec_transparency);
        material->SetTextureType(mat_tex_type);
        material->SetSource(mat_source);
    }
}

Material::Material(name_t name, materialtype_t type) : Resource(name), type(type) {
    if (type == MATERIAL_LIGHTMAP) {
        filter = FILTER_LINEAR;
    }
}

/// Creates a material.
/// If a Material already exists with that name, then the existing Material is returned.
/// @return Always returns a pointer to a Material.
Material* Material::Make(name_t name, materialtype_t type) {
    Material* material = material_list.Find(name);
    
    if (!material) {
        //material = PoolProxy<Material>::New(name, type, FILTER_NEAREST);
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
        //material = PoolProxy<Material>::New(name, MATERIAL_LIGHTMAP, FILTER_LINEAR);
        material = PoolProxy<Material>::New(name);
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

void Material::LoadFromDisk() {
    assert(status == UNLOADED);

    if (texture_type == TEXTURE_NONE) {
        MakePattern({1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f});
        status = LOADED;
        return;
    } else if (texture_type == TEXTURE_SOURCE) {
        source->AddReference();
        Async::LoadDependency(source);
        status = LOADED;
        return;
    }

    int loadwidth, loadheight, loadchannels;
    unsigned char* loadtexture = nullptr;
    char path[100] = "data/textures/";

    switch (type) {
        case MATERIAL_LIGHTMAP:
            //filter = FILTER_LINEAR;
            channels = 3;
            break;
        case MATERIAL_TEXTURE_ALPHA:
            channels = 4;
            break;
        case MATERIAL_MSDF:
            filter = FILTER_LINEAR;
            channels = 4;
            break;
        case MATERIAL_GLYPH:
            filter = FILTER_NEAREST;
            channels = 4;
            break;
        default:
            channels = 3;
    }

    strcat(path, name);
    strcat(path, ".png");

    stbi_set_flip_vertically_on_load(true);
    loadtexture = stbi_load(path, &loadwidth, &loadheight, &loadchannels, channels);

    if (loadchannels != channels) {
        std::cout << "Texture " << path << " should have " << (int)channels << " channels, but it has " << (int)loadchannels << "!" << std::endl;
    }

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

        MakePattern({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 1.0f});
        
        load_fail = true;
    }

}

void Material::LoadFromMemory(){
    assert(status == LOADED);

    if (texture_type == TEXTURE_SOURCE) {
        texture = source->texture;
        status = READY;
        return;
    }

    // TODO: switch this out from checking 'type' and instead use 'channels'
    if (type == MATERIAL_TEXTURE_ALPHA || type == MATERIAL_MSDF || type == MATERIAL_GLYPH) {
        texture = API::CreateTexture(COLORMODE_RGBA, filter == FILTER_NEAREST ? TEXTUREFILTER_NEAREST : TEXTUREFILTER_LINEAR, width, height, texture_data);
    } else {
        texture = API::CreateTexture(COLORMODE_RGB, filter == FILTER_NEAREST ? TEXTUREFILTER_NEAREST : TEXTUREFILTER_LINEAR, width, height, texture_data);
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



