// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_PLATFORM_FILE_H
#define TRAM_SDK_PLATFORM_FILE_H

#include <cstddef>
#include <string>
#include <vector>

namespace tram {

enum class FileStatus {
    IO_ERROR,
    READY
};

enum class FileMedium {
    DISK,
    USER,
    ARCHIVE,
    HTTP
};

struct FileSource {
    FileMedium medium;
    std::string path;
};

class FileReader {
public:
    virtual ~FileReader() = default;
    
    virtual const char* GetContents() = 0;
    virtual size_t GetSize() = 0;
    virtual FileStatus GetStatus() = 0;
    virtual void Yeet() = 0;
    
    static FileReader* GetReader(const char* path);
    
    static void SetSearchList();
    static std::vector<FileSource> GetSearchList();
};

class FileWriter {
public:
    virtual ~FileWriter() = default;
    
    virtual void SetContents(const char* contents, size_t size) = 0;
    virtual FileStatus GetStatus() = 0;
    virtual void Yeet() = 0;
    
    static FileWriter* GetWriter(const char* path);
    
    static void SetMediumList();
    static std::vector<FileSource> GetMediumList();
};

}

#endif // PLATFORM_FILE_H