#include <platform/image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace tram::Platform {

void SaveImageToDisk(const char* name, int w, int h, const char* image) {
    stbi_flip_vertically_on_write(1);
    stbi_write_png(name, w, h, 3, image, 0);
}

}