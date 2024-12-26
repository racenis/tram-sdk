// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <platform/image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace tram::Platform {

/// Saves image to disk.
/// Image needs to be in a RGB 8-bit format.
void SaveImageToDisk(const char* name, int w, int h, const char* image) {
    stbi_flip_vertically_on_write(1);
    stbi_write_png(name, w, h, 3, image, 0);
}

}