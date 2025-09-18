// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_PLATFORM_FILE_H
#define TRAM_SDK_PLATFORM_FILE_H

#include <cstddef>
#include <vector>

namespace tram {

enum class FileStatus {
    IO_ERROR,
    READY
};

struct FileSource {
    const char* protocol;
    const char* location;
};

class FileReader {
public:
    virtual ~FileReader() = default;
    
    virtual const char* GetContents() = 0;
    virtual size_t GetSize() = 0;
    virtual FileStatus GetStatus() = 0;
    virtual void Yeet() = 0;
    
    static FileReader* GetReader(const char* path);
    
    static void SetSearchList(std::vector<FileSource>);
    static std::vector<FileSource> GetSearchList();
    
    static void Register(const char* protocol, FileReader* (*constr)(const char* location, const char* path));
};

class FileWriter {
public:
    virtual ~FileWriter() = default;
    
    virtual void SetContents(const char* contents, size_t size) = 0;
    virtual FileStatus GetStatus() = 0;
    virtual void Yeet() = 0;
    
    static FileWriter* GetWriter(const char* path);
    
    static void Register(const char* protocol, FileWriter* (*constr)(const char* path));
};

}

#endif // PLATFORM_FILE_H