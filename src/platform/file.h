// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef PLATFORM_FILE_H
#define PLATFORM_FILE_H

#include <cstddef>

namespace tram {

enum FileSource {
    SOURCE_ANY,
    SOURCE_DISK
};

class FileReader {
public:
    FileReader (const char* path, FileSource source = SOURCE_ANY);
    ~FileReader ();
    
    bool is_open();
    
    const char* contents = nullptr;
    size_t length = 0;
};

class FileWriter {
public:
    FileWriter (const char* path, FileSource source = SOURCE_ANY);
    ~FileWriter ();
    
    bool is_open();
    void write (const char* data, size_t length);
    
private:
    void* file_handle = nullptr;
};
    
}

#endif // PLATFORM_FILE_H