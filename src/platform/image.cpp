// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <platform/image.h>
#include <platform/file.h>

#include <framework/logging.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace tram::Platform {

/// Saves image to disk.
/// Image needs to be in a RGB 8-bit format.
void SaveImageToDisk(const char* name, int w, int h, const char* image) {
    FileWriter* writer = FileWriter::GetWriter(name);
    if (writer->GetStatus() != FileStatus::READY) {
        Log(Severity::ERROR, System::PLATFORM, "Couldn't open {} for image writing!", name);
        return;
    }

    stbi_flip_vertically_on_write(1);
    stbi_write_png_to_func([](void* context, void* data, int size) {
        ((FileWriter*)context)->SetContents((const char*)data, size);
    }, writer, w, h, 3, image, 0);
    
    writer->Flush();
    if (writer->GetStatus() != FileStatus::READY) {
        Log(Severity::ERROR, System::PLATFORM, "Was an error writing image {}!", name);
        return;
    }
    
    writer->Yeet();
}

}