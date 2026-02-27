// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

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
#include <framework/logging.h>

#include <render/material.h>
#include <render/api.h>
#include <render/error.h>

#include <platform/file.h>

#include <templates/hashmap.h>

#include <config.h>

#include <fstream>

/**
 * @class tram::Render::Material render/material.h <render/material.h>
 * 
 * Material resource.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/render/material.html
 */

using namespace tram;

template <> Pool<Render::Material> PoolProxy<Render::Material>::pool("Material pool", RESOURCE_LIMIT_MATERIAL);

namespace tram::Render {

static Hashmap<Material*> material_list("Material name list", RESOURCE_LIMIT_MATERIAL);

/// Loads a Material definition file.
void Material::LoadMaterialInfo(const char* filename) {
    using namespace tram::Render;
    
    char path [100] = "data/";
    strcat (path, filename);
    strcat (path, ".list");

    File file (path, File::READ);
    
    if (!file.is_open()) {
        std::cout << "Can't open texture info file '" << path << "'" << std::endl;
        return;
    }
    
    name_t file_type = file.read_name();
    
    if (file_type != "MATv7") {
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
        float mat_reflectivity = file.read_float32();
        name_t mat_tex_type_name = file.read_name();
        Material* mat_source = nullptr;

        if(mat_type_name == UID("flat")){
            mat_type = MATERIAL_TEXTURE;
        } else if(mat_type_name == UID("alpha")){
            mat_type = MATERIAL_TEXTURE_ALPHA;
        } else if(mat_type_name == UID("blend")){
            mat_type = MATERIAL_TEXTURE_BLEND;
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
        } else if (mat_tex_type_name == "normal") {
            mat_tex_type = TEXTURE_SAME_NORMAL;
        } else {
            mat_tex_type = TEXTURE_SOURCE;
            mat_source = Material::Find(mat_tex_type_name);
        }

        Material* material = material_list.Find(mat_name);
    
        if (!material) {
            material = PoolProxy<Material>::GetPool().allocate();
            new(material) Material(mat_name);
            material_list.Insert(UID(mat_name), material);
        }

        material->SetMaterialType(mat_type);
        material->SetMaterialFilter(mat_filter);
        material->SetMaterialProperty(mat_property);
        material->SetColor(mat_color);
        material->SetSpecular(mat_spec_weight, mat_spec_exponent, mat_spec_transparency);
        material->SetReflectivity(mat_reflectivity);
        material->SetTextureType(mat_tex_type);
        material->SetSource(mat_source);
    }
}

Material::Material(name_t name, materialtype_t type) : Resource(name), type(type) {
    if (type == MATERIAL_LIGHTMAP || type == MATERIAL_ENVIRONMENTMAP) {
        filter = FILTER_LINEAR;
    }
}

/// Creates a material.
/// If a Material already exists with that name, then the existing Material is returned.
/// @return Always returns a pointer to a Material.
Material* Material::Make(name_t name, materialtype_t type) {
    Material* material = material_list.Find(name);
    
    if (!material) {
        material = PoolProxy<Material>::GetPool().allocate();
        new(material) Material(name, type);
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
        material = PoolProxy<Material>::GetPool().allocate();
        new(material) Material(name);
        material_list.Insert(name, material);
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

    texture_data = MakeNewErrorTexture(color1, color2);

    status = LOADED;
}

void Material::SetSource(Material* source) {
    this->source = source;
    
    while (source) {
        if (this != source) {
            source = source->source;
            continue;
        }
        
        this->source = nullptr;
        this->texture_type = TEXTURE_NONE;
        
        Log(Severity::ERROR, System::RENDER, "Detected cycle when assigning source material to {}!", this->name);
        return;
    }
}

static ColorMode ChannelsToColorMode(int channels) {
    switch (channels) {
        case 0: assert(false);
        case 1: return COLORMODE_R;
        case 2: return COLORMODE_RG;
        case 3: return COLORMODE_RGB;
        case 4: return COLORMODE_RGBA;
        default: assert(false);
    }
    abort();
}

void Material::SetTextureImage(uint8_t* data, uint8_t channels, uint16_t width, uint16_t height) {
    if (status == READY) {
        this->width = width;
        this->height = height;
        this->channels = channels;
        
        API::SetTextureImage(texture, ChannelsToColorMode(channels), width, height, data);
        
        return;
    }
    
    if (status == LOADED) {
        delete[] this->texture_data;
    }
    
    size_t buffer_size = width * height * channels;
    
    this->width = width;
    this->height = height;
    this->channels = channels;
    this->texture_data = new uint8_t[buffer_size];
    
    mempcpy(this->texture_data, data, buffer_size);
    
    status = LOADED;
}

void Material::FlushToAPI() {
    if (status != LOADED) return;
    
    API::SetMaterialColor(material, vec4(color, 1.0f));
    API::SetMaterialSpecularWeight(material, specular_weight);
    API::SetMaterialSpecularExponent(material, specular_exponent);
    API::SetMaterialSpecularTransparency(material, specular_transparency);
    API::SetMaterialReflectivity(material, reflectivity);
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
        case MATERIAL_TEXTURE_BLEND:
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

    FileReader* file = FileReader::GetReader(path);
        
    if (file->GetStatus() == FileStatus::READY) {
       stbi_set_flip_vertically_on_load(true);
       loadtexture = stbi_load_from_memory((const unsigned char*)file->GetContents(), file->GetSize(), &loadwidth, &loadheight, &loadchannels, channels);
    }

    if (loadchannels < channels) {
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


    if (texture_type == TEXTURE_SAME_NORMAL) {
        strcpy(path, "data/textures/");
        strcat(path, name);
        strcat(path, ".normal.png");
        
        loadtexture = stbi_load(path, &loadwidth, &loadheight, &loadchannels, 3);
        
        if (loadchannels != 3) {
            std::cout << "Texture " << path << " should have 3 channels, but it has " << (int)loadchannels << "!" << std::endl;
        }
        
        if (loadtexture) {
            normal_map_width = loadwidth;
            normal_map_height = loadheight;
            normal_map_data = new uint8_t[normal_map_width * normal_map_height * channels];

            for (size_t i = 0; i < normal_map_width * normal_map_height * 3; i++) {
                normal_map_data[i] = loadtexture[i];
            }
            
            stbi_image_free(loadtexture);
        } else {            
            normal_map_width = 64;
            normal_map_height = 64;

            normal_map_data = MakeNewErrorTexture(glm::normalize(vec3(0.25f, 0.75f, 1.0f)), glm::normalize(vec3(0.75f, 0.25f, 1.0f)));

            std::cout << "Normal map " << name << " (" << path << ") couldn't be loaded!" << std::endl;
        }
    }

}

/// Pushes material textures to GPU.
/// Except if software rendering is used, in which case the texures will merely
/// be copied and converted.
void Material::LoadFromMemory() {
    assert(status == LOADED);

    material = API::MakeMaterial();
    FlushToAPI();

    if (texture_type == TEXTURE_SOURCE) {
        texture = source->texture;
        API::SetMaterialTexture(material, texture);
        status = READY;
        return;
    }
    
    texture = API::CreateTexture(ChannelsToColorMode(channels), filter == FILTER_NEAREST ? TEXTUREFILTER_NEAREST : TEXTUREFILTER_LINEAR, width, height, texture_data);
    
    if (texture_type == TEXTURE_SAME_NORMAL) {
        normal_map = API::CreateTexture(COLORMODE_RGB, filter == FILTER_NEAREST ? TEXTUREFILTER_NEAREST : TEXTUREFILTER_LINEAR, normal_map_width, normal_map_height, normal_map_data);
    }
    
    API::SetMaterialTexture(material, texture);
    
    if (texture_type == TEXTURE_SAME_NORMAL) {
        API::SetMaterialNormalMap(material, normal_map);
    }
    
    float approx_memory = width * height * channels;  // image size
    if (texture_type == TEXTURE_SAME_NORMAL) {
        approx_memory += normal_map_width * normal_map_height * 3;
    }
    approx_memory = approx_memory * 1.3f;             // plus mipmaps
    
    approx_vram_usage = (size_t) approx_memory;
    
    Stats::Add(Stats::RESOURCE_VRAM, approx_vram_usage);

    delete[] texture_data;
    texture_data = nullptr;
    
    if (normal_map_data) {
        delete[] normal_map_data;
        normal_map_data = nullptr;
    }
    
    status = READY;
    return;
}

void Material::Unload() {
    assert(status == LOADED);
    
    if (texture.generic) API::YeetTexture(texture);
    if (normal_map.generic) API::YeetTexture(normal_map);
    if (material.generic) API::YeetMaterial(material);
}

}



