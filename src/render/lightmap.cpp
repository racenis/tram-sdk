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
    
    uint32_t layer_count = layer_count_for_type(type);
    
    
    int loadwidth, loadheight, loadchannels;
    unsigned char* loadtexture = nullptr;
    char path[100];
    
    if (name) {
        snprintf(path, 100, "data/lightmaps/%s.png", (const char*)name);
    } else {
        snprintf(path, 100, "data/lightmaps/%i.png", index);
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
                snprintf(path, 100, "data/lightmaps/%s.%i.png", (const char*)name, layer);
            } else {
                snprintf(path, 100, "data/lightmaps/%i.%i.png", index, layer);
            }
            
            file = FileReader::GetReader(path);
            if (file->GetStatus() == FileStatus::READY) {
                stbi_set_flip_vertically_on_load(true);
                loadtexture = stbi_load_from_memory((const unsigned char*)file->GetContents(), file->GetSize(), &loadwidth, &loadheight, &loadchannels, 3);

                if (loadwidth != (int)width || loadheight != (int)height) {
                    std::cout << "Lightmap" << (name ? std::string(name) : std::to_string(index)) << " layer " << layer << " dimensions of " << loadwidth << " by " << loadheight << " don't match first frame dimensions of " << width << " by " << height << std::endl;
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
            std::cout << "Lightmap " << (name ? std::string(name) : std::to_string(index)) << " (" << path << ") couldn't be loaded!" << std::endl;
            
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



