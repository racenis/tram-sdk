// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#define STBI_ONLY_PNG
#include <stb_image.h>

#include <framework/stats.h>
#include <framework/logging.h>
#include <framework/settings.h>

#include <render/lightmap.h>
#include <render/api.h>

#include <platform/file.h>
#include <platform/api.h>

#include <templates/hashmap.h>

#include <config.h>

/**
 * @class tram::Render::Lightmap render/lightmap.h <render/lightmap.h>
 * 
 * Lightmap resource.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/render/lightmap.html
 */

using namespace tram;

template <> Pool<Render::Lightmap> PoolProxy<Render::Lightmap>::pool("Lightmap pool", RESOURCE_LIMIT_LIGHTMAP);

namespace tram::Render {

static Hashmap<Lightmap*> lightmap_name_list("Lightmap name list", RESOURCE_LIMIT_LIGHTMAP);
static Hashmap<Lightmap*> lightmap_index_list("Lightmap index list", RESOURCE_LIMIT_LIGHTMAP);

static Settings::Property<bool> lightmap_nearest = {false, "lightmap-nearest", Settings::NONE};

/// Finds a Lightmap.
/// Finds a Lightmap by its associated name. If a lightmap with that name does
/// not exist, it will be created.
/// @return Always returns a pointer to a Lightmap.
Lightmap* Lightmap::Find(name_t name) {
    Lightmap* lightmap = lightmap_name_list.find(name);
    
    if (!lightmap) {
        lightmap = PoolProxy<Lightmap>::GetPool().allocate();
        new(lightmap) Lightmap(name);
        lightmap_name_list.insert(name, lightmap);
    }
    
    return lightmap;
}

/// Finds a Lightmap.
/// Finds a Lightmap by its associated index. If a lightmap with that index does
/// not exist, it will be created.
/// @return Always returns a pointer to a Lightmap.
Lightmap* Lightmap::Find(id_t index) {
    Lightmap* lightmap = lightmap_index_list.find(index);
    
    if (!lightmap) {
        lightmap = PoolProxy<Lightmap>::GetPool().allocate();
        new(lightmap) Lightmap(index);
        lightmap_index_list.insert(index, lightmap);
    }
    
    return lightmap;
}

static TextureFilter GetFilterFromMaterialFilter(MaterialFilter filter) {
    if (lightmap_nearest) {
        return TEXTUREFILTER_NEAREST;
    }
    
    switch (filter) {
        case FILTER_SURFACE:
        case FILTER_INTERFACE:
        case FILTER_LINEAR:
        default:
            return TEXTUREFILTER_LINEAR;
        case FILTER_NEAREST:
            return TEXTUREFILTER_NEAREST;
    }
}

void Lightmap::FlushToAPI() {
    if (status != READY) return;
    
    API::SetTextureFilter(texture, GetFilterFromMaterialFilter(filter));
}

void Lightmap::SetType(LightmapType type) {
    if (status != UNLOADED) {
        Log(Severity::WARNING, System::RENDER, "Loaded Lightmaps cannot change types! Ignoring Lightmap::SetType() call.");
        return;
    }
    
    this->type = type;
}

static uint32_t layer_count_for_type(LightmapType type) {
    switch (type) {
        case LIGHTMAP_SINGLE:   return 1;
        case LIGHTMAP_CHANNELS: return 4;
        default:                return 1;
    }
}

void Lightmap::LoadFromDisk() {
    if (status != UNLOADED) {
        Log(Severity::WARNING, System::RENDER, "Lightmap {} already loaded! Ignoring LightGraph::LoadFromDisk() call.", name);
        return;
    }
    
    // ideally we would handle the `fullbright` lightmap using a shader flag,
    // but for now we'll just treat it as a special lightmap that gets generated
    // instead of being loaded.
    // this is kinda stinky and we *WILL* fix it.. later.. sometime..
    if (name == "fullbright" || name == "fulldark") {
        type = LIGHTMAP_CHANNELS;
        width = 32;
        height = 32;
        size_t size = width * height * 3 * layer_count_for_type(type);
        texture_data = new uint8_t[size];
        memset(texture_data, name == "fullbright" ? 255 : 0, size);
        status = LOADED;
        return;
    }
    
    uint32_t layer_count = layer_count_for_type(type);
    
    int loadwidth, loadheight, loadchannels;
    unsigned char* loadtexture = nullptr;
    char path[PATH_LIMIT];
    
    if (name) {
        snprintf(path, PATH_LIMIT, "data/lightmaps/%s.png", (const char*)name);
    } else {
        snprintf(path, PATH_LIMIT, "data/lightmaps/%u.png", index);
    }


    FileReader* file = FileReader::GetReader(path);
    if (file->GetStatus() == FileStatus::READY) {
       stbi_set_flip_vertically_on_load(true);
       loadtexture = stbi_load_from_memory((const unsigned char*)file->GetContents(), file->GetSize(), &loadwidth, &loadheight, &loadchannels, 3);
    }
    file->Yeet();
    
    if (loadtexture) {
        width = loadwidth;
        height = loadheight;
    } else {
        width = 128;
        height = 128;
    }
    
    texture_data = new uint8_t[width * height * 3 * layer_count];
    
    for (uint32_t layer = 0; layer < layer_count; layer++) {
        if (layer >= 1) {
            if (name) {
                snprintf(path, PATH_LIMIT, "data/lightmaps/%s.%u.png", (const char*)name, layer);
            } else {
                snprintf(path, PATH_LIMIT, "data/lightmaps/%i.%u.png", index, layer);
            }
            
            file = FileReader::GetReader(path);
            if (file->GetStatus() == FileStatus::READY) {
                stbi_set_flip_vertically_on_load(true);
                loadtexture = stbi_load_from_memory((const unsigned char*)file->GetContents(), file->GetSize(), &loadwidth, &loadheight, &loadchannels, 3);

                if (loadwidth != (int)width || loadheight != (int)height) {
                    Log(Severity::WARNING, System::RENDER, "Lightmap {} layer {} dimensions of {} by {} don't match first frame dimensions of {} by {}",
                        (name ? std::string(name) : std::to_string(index)), layer, loadwidth, loadheight, width, height);
                    stbi_image_free(loadtexture);
                    loadtexture = nullptr;
                }
            }
            file->Yeet();
        }
        
        if (loadtexture) {
            memcpy(texture_data + width * height * 3 * layer, loadtexture, width * height * 3);
            stbi_image_free(loadtexture);
            loadtexture = nullptr;
        } else {
            Log(Severity::NOTE, System::RENDER,  "Lightmap {} ({}) couldn't be loaded!", (name ? std::string(name) : std::to_string(index)), path);

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

static size_t approx_memory(uint32_t width, uint32_t height, LightmapType type) {
    float approx_memory = 3.0f * width * height * layer_count_for_type(type);
    return approx_memory * 1.3f;
}

void Lightmap::LoadFromMemory() {
    if (status != LOADED) {
        Log(Severity::WARNING, System::RENDER, "Lightmap {} hasn't been loaded! Ignoring Lightmap::LoadFromMemory() call.", name);
        return;
    }
    
    if (!Platform::Window::IsRenderContextThread()) {
        Log(Severity::WARNING, System::RENDER, "Lightmap::LoadFromMemory() not being called from render thread! Ignoring.");
        return;
    }

    texture = API::CreateTexture(COLORMODE_RGB, lightmap_nearest ? TEXTUREFILTER_NEAREST : TEXTUREFILTER_LINEAR, width, height, layer_count_for_type(type), texture_data);
    
    Stats::Add(Stats::RESOURCE_VRAM, approx_memory(width, height, type));

    delete[] texture_data;
    texture_data = nullptr;
    
    status = READY;
    
    FlushToAPI();
}

void Lightmap::Unload() {
    if (status != READY) {
        Log(Severity::WARNING, System::RENDER, "Lightmap {} hasn't been loaded! Ignoring Lightmap::Unload() call.", name);
        return;
    }
    
    if (!Platform::Window::IsRenderContextThread()) {
        Log(Severity::WARNING, System::RENDER, "Lightmap::Unload() not being called from render thread! Ignoring.");
        return;
    }
    
    if (texture.generic) API::YeetTexture(texture);
    texture.generic = nullptr;

    Stats::Remove(Stats::RESOURCE_VRAM, approx_memory(width, height, type));

    status = UNLOADED;
}

}