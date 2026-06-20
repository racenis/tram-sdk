// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#define STBI_ONLY_PNG
#include <stb_image.h>

#include <framework/stats.h>
#include <framework/logging.h>

#include <render/environment.h>
#include <render/api.h>

#include <platform/file.h>
#include <platform/api.h>

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

void Environment::SetType(EnvironmentType type) {
    if (status != UNLOADED) {
        Log(Severity::WARNING, System::RENDER, "Loaded Environments cannot change types! Ignoring Environment::SetType() call.");
        return;
    }
    
    this->type = type;
}

static uint32_t layer_count_for_type(EnvironmentType type) {
    switch (type) {
        case ENVIRONMENT_SPHERE:    return 2;
        default:                    return 1;
    }
}

void Environment::LoadFromDisk() {
    if (status != UNLOADED) {
        Log(Severity::WARNING, System::RENDER, "Environment {} already loaded! Ignoring Environment::LoadFromDisk() call.", name);
        return;
    }
    
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
            snprintf(path, PATH_LIMIT, "data/environments/%s.%u.png", (const char*)name, layer);
        } else {
            snprintf(path, PATH_LIMIT, "data/environments/%s.%u.%u.png", (const char*)graph, index, layer);
        }
        
        FileReader* file = FileReader::GetReader(path);
        if (file->GetStatus() == FileStatus::READY) {
            stbi_set_flip_vertically_on_load(true);
            loadtexture = stbi_load_from_memory((const unsigned char*)file->GetContents(), file->GetSize(), &loadwidth, &loadheight, &loadchannels, 3);

            if (texture_data && (loadwidth != (int)width || loadheight != (int)height)) {
                Log(Severity::WARNING, System::RENDER, "Environment map {} layer {} dimensions of {} by {} don't match first frame dimensions of {} by {}",
                        (name ? std::string(name) : std::to_string(index)), layer, loadwidth, loadheight, width, height);
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
            Log(Severity::NOTE, System::RENDER, "File not found: {}", path);
            
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

static size_t approx_memory(uint32_t width, uint32_t height, EnvironmentType type) {
    float approx_memory = 3.0f * width * height * layer_count_for_type(type);
    return approx_memory * 1.3f;
}

void Environment::LoadFromMemory() {
    if (status != LOADED) {
        Log(Severity::WARNING, System::RENDER, "Environment {} hasn't been loaded! Ignoring Environment::LoadFromMemory() call.", name);
        return;
    }
    
    if (!Platform::Window::IsRenderContextThread()) {
        Log(Severity::WARNING, System::RENDER, "Environment::LoadFromMemory() not being called from render thread! Ignoring.");
        return;
    }

    texture = API::CreateTexture(COLORMODE_RGB, TEXTUREFILTER_LINEAR, width, height, layer_count_for_type(type), texture_data);
    
    Stats::Add(Stats::RESOURCE_VRAM, approx_memory(width, height, type));

    delete[] texture_data;
    texture_data = nullptr;
    
    status = READY;
    return;
}

void Environment::Unload() {
    if (status != READY) {
        Log(Severity::WARNING, System::RENDER, "Environment {} hasn't been loaded! Ignoring Environment::Unload() call.", name);
        return;
    }
    
    if (!Platform::Window::IsRenderContextThread()) {
        Log(Severity::WARNING, System::RENDER, "Environment::Unload() not being called from render thread! Ignoring.");
        return;
    }
    
    if (texture.generic) API::YeetTexture(texture);
    texture.generic = nullptr;

    Stats::Remove(Stats::RESOURCE_VRAM, approx_memory(width, height, type));

    status = UNLOADED;
}

}