// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef PLATFORM_FILE_H
#define PLATFORM_FILE_H

#include <cstdint>

namespace tram {

enum FileSource {
    SOURCE_ANY,
    SOURCE_DISK
};

class FileReader {
public:
    FileReader (const char* path, FileSource source);
    ~FileReader ();
    
    bool is_open();
    
    const char* contents = nullptr;
    size_t length = 0;
};

class FileWriter {
public:
    FileWriter (const char* path, FileSource source);
    ~FileWriter ();
    
    bool is_open();
    void write (const char* data, size_t length);
    
private:
    void* file_handle = nullptr;
    char* buffer = nullptr;
    size_t buffer_length = 0;
};
    
}

#endif // PLATFORM_FILE_H