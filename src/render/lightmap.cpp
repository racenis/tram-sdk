// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#define STBI_ONLY_PNG
#include <stb_image.h>

#include <framework/stats.h>
#include <framework/logging.h>

#include <render/lightmap.h>
#include <render/api.h>

#include <platform/file.h>

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

/// Finds a Lightmap.
/// Finds a Lightmap by its associated name. If a lightmap with that name does
/// not exist, it will be created.
/// @return Always returns a pointer to a Lightmap.
Lightmap* Lightmap::Find(name_t name) {
    Lightmap* lightmap = lightmap_name_list.Find(name);
    
    if (!lightmap) {
        lightmap = PoolProxy<Lightmap>::GetPool().allocate();
        new(lightmap) Lightmap(name);
        lightmap_name_list.Insert(name, lightmap);
    }
    
    return lightmap;
}

/// Finds a Lightmap.
/// Finds a Lightmap by its associated index. If a lightmap with that index does
/// not exist, it will be created.
/// @return Always returns a pointer to a Lightmap.
Lightmap* Lightmap::Find(id_t index) {
    Lightmap* lightmap = lightmap_index_list.Find(index);
    
    if (!lightmap) {
        lightmap = PoolProxy<Lightmap>::GetPool().allocate();
        new(lightmap) Lightmap(index);
        lightmap_index_list.Insert(index, lightmap);
    }
    
    return lightmap;
}

static uint32_t layer_count_for_type(LightmapType type) {
    switch (type) {
        case LIGHTMAP_SINGLE:   return 1;
        case LIGHTMAP_CHANNELS: return 4;
        default:                return 1;
    }
}

void Lightmap::LoadFromDisk() {
    assert(status == UNLOADED);
    
    // ideally we would handle the `fullbright` lightmap using a shader flag,
    // but for now we'll just treat it as a special lightmap that gets generated
    // instead of being loaded.
    // this is kinda stinky and we *WILL* fix it.. later.. sometime..
    if (name == "fullbright") {
        type = LIGHTMAP_CHANNELS;
        width = 32;
        height = 32;
        size_t size = width * height * 3 * layer_count_for_type(type);
        texture_data = new uint8_t[size];
        memset(texture_data, 255, size);
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
        snprintf(path, PATH_LIMIT, "data/lightmaps/%i.png", index);
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
                snprintf(path, PATH_LIMIT, "data/lightmaps/%s.%i.png", (const char*)name, layer);
            } else {
                snprintf(path, PATH_LIMIT, "data/lightmaps/%i.%i.png", index, layer);
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

void Lightmap::LoadFromMemory() {
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

void Lightmap::Unload() {
    assert(status == READY);
    
    // TODO: add a check that this is being called from render thread
    
    if (texture.generic) API::YeetTexture(texture);
    
    texture.generic = nullptr;

    status = UNLOADED;
}

}



