// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#define STBI_ONLY_PNG
#include <stb_image.h>

#include <framework/stats.h>
#include <framework/logging.h>

#include <render/environment.h>
#include <render/api.h>

#include <platform/file.h>

#include <templates/hashmap.h>

#include <config.h>

/**
 * @class tram::Render::Environment render/environment.h <render/environment.h>
 * 
 * Environment map resource.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/render/environment.html
 */

using namespace tram;

template <> Pool<Render::Environment> PoolProxy<Render::Environment>::pool("Environment map pool", RESOURCE_LIMIT_ENVIRONMENTMAP);

namespace tram::Render {

static Hashmap<Environment*> environment_name_list("Environment map name list", RESOURCE_LIMIT_ENVIRONMENTMAP);
static Hashmap<std::vector<Environment*>*> environment_index_list("Environment map index list", RESOURCE_LIMIT_ENVIRONMENTMAP);

/// Finds an Environment map.
/// Finds an Environment map by its associated name. If a environment map with
/// that name does not exist, it will be created.
/// @return Always returns a pointer to a Environment map.
Environment* Environment::Find(name_t name) {
    Environment* environment = environment_name_list.Find(name);
    
    if (!environment) {
        environment = PoolProxy<Environment>::GetPool().allocate();
        new(environment) Environment(name);
        environment_name_list.Insert(name, environment);
    }
    
    return environment;
}

/// Finds an Environment map.
/// Finds an Environment map by its associated index and graph. If a environment
/// map with that index does not exist, it will be created.
/// @return Always returns a pointer to a Environment map.
Environment* Environment::Find(name_t graph, uint32_t index) {
    std::vector<Environment*>* graphmaps = environment_index_list.Find(graph);
    if (!graphmaps) {
        graphmaps = new std::vector<Environment*>;
        environment_index_list.Insert(graph, graphmaps);
    }
    
    if (index >= graphmaps->size()) {
        graphmaps->resize(index + 1, nullptr);
    }
    
    Environment* environment = (*graphmaps)[index];
    
    if (!environment) {
        environment = PoolProxy<Environment>::GetPool().allocate();
        new(environment) Environment(graph, index);
        (*graphmaps)[index] = environment;
    }
    
    return environment;
}

static uint32_t layer_count_for_type(EnvironmentType type) {
    switch (type) {
        case ENVIRONMENT_SPHERE:    return 2;
        default:                    return 1;
    }
}

void Environment::LoadFromDisk() {
    assert(status == UNLOADED);
    
    // ideally we would handle the `fulldark` environment map using a shader
    // flag, but for now we'll just treat it as a special environment map that
    // gets generated instead of being loaded.
    // this is kinda stinky and we *WILL* fix it.. later.. sometime..
    if (name == "fulldark") {
        type = ENVIRONMENT_SPHERE;
        width = 32;
        height = 32;
        size_t size = width * height * 3 * layer_count_for_type(type);
        texture_data = new uint8_t[size];
        memset(texture_data, 0, size);
        status = LOADED;
        return;
    }
    
    uint32_t layer_count = layer_count_for_type(type);
    
    int loadwidth, loadheight, loadchannels;
    unsigned char* loadtexture = nullptr;
    char path[PATH_LIMIT];
  
    for (uint32_t layer = 0; layer < layer_count; layer++) {
        if (name) {
            snprintf(path, PATH_LIMIT, "data/environments/%s.%i.png", (const char*)name, layer);
        } else {
            snprintf(path, PATH_LIMIT, "data/environments/%s.%i.%i.png", (const char*)graph, index, layer);
        }
        
        FileReader* file = FileReader::GetReader(path);
        if (file->GetStatus() == FileStatus::READY) {
            stbi_set_flip_vertically_on_load(true);
            loadtexture = stbi_load_from_memory((const unsigned char*)file->GetContents(), file->GetSize(), &loadwidth, &loadheight, &loadchannels, 3);

            if (texture_data && (loadwidth != (int)width || loadheight != (int)height)) {
                std::cout << "Environment map" << (name ? std::string(name) : std::string(graph) + std::to_string(index)) << " layer " << layer << " dimensions of " << loadwidth << " by " << loadheight << " don't match first frame dimensions of " << width << " by " << height << std::endl;
                stbi_image_free(loadtexture);
                loadtexture = nullptr;
            }
        }
        file->Yeet();
        
        if (!texture_data) {
            if (loadtexture) {
                width = loadwidth;
                height = loadheight;
            } else {
                width = 64;
                height = 64;
            }
            
            texture_data = new uint8_t[width * height * 3 * layer_count];
        }
        
        if (loadtexture) {
            memcpy(texture_data + width * height * 3 * layer, loadtexture, width * height * 3);
            stbi_image_free(loadtexture);
            loadtexture = nullptr;
        } else {
            std::cout << "Environment map " << (name ? std::string(name) : std::string(graph) + std::to_string(index)) << " (" << path << ") couldn't be loaded!" << std::endl;
            
            load_fail = true;
            
            // generate error pattern
            for (uint32_t x = 0; x < width; x++)
            for (uint32_t y = 0; y < height; y++) {
                uint32_t offset = (x + y * width + width * height * layer) * 3;
                bool w = x & 16;
                bool h = y & 16;
                uint8_t color = w ^ h ? 255 : 0;
                texture_data[offset + 0] = color;
                texture_data[offset + 1] = color;
                texture_data[offset + 2] = color;
            }
        }
    }
    
    status = LOADED;
}

void Environment::LoadFromMemory() {
    assert(status == LOADED);

    // TODO: add a check that this is being called from render thread

    texture = API::CreateTexture(COLORMODE_RGB, TEXTUREFILTER_LINEAR, width, height, layer_count_for_type(type), texture_data);
    
    float approx_memory = 3.0f * width * height * layer_count_for_type(type);  // image size
    approx_memory = approx_memory * 1.3f;             // plus mipmaps
    
    approx_vram_usage = (size_t) approx_memory;
    
    Stats::Add(Stats::RESOURCE_VRAM, approx_vram_usage);

    delete[] texture_data;
    texture_data = nullptr;
    
    status = READY;
    return;
}

void Environment::Unload() {
    assert(status == READY);
    
    // TODO: add a check that this is being called from render thread
    
    if (texture.generic) API::YeetTexture(texture);
    
    texture.generic = nullptr;

    status = UNLOADED;
}

}



