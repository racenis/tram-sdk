// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/file/archive.h>

#include <platform/file.h>

namespace tram::Ext::File  {

class ArchiveReader : public FileReader {
public:
    ArchiveReader(const char* path);
    virtual ~ArchiveReader() = default;
    
    virtual const char* GetContents() override;
    virtual size_t GetSize() override;
    virtual FileStatus GetStatus() override;
    virtual void Yeet() override;
private:
    char* contents = nullptr;
    int length = 0;
};

ArchiveReader::ArchiveReader(const char* path) {
    
}

const char* ArchiveReader::GetContents() {
    return contents;
}

size_t ArchiveReader::GetSize() {
    return length;
}

FileStatus ArchiveReader::GetStatus() {
    if (contents) return FileStatus::READY;
    return FileStatus::IO_ERROR;
}

void ArchiveReader::Yeet() {
    delete this;
}

void RegisterArchive() {
    
}

}
