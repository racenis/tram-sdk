// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/file/archive.h>

#include <platform/file.h>
#include <framework/logging.h>

#include <miniz.h>

namespace tram::Ext::File  {

struct Archive {
    mz_zip_archive* zip = nullptr;
    const char* name = nullptr;
    
    bool open = false;
};
std::vector<Archive> archives;

Archive& AddArchive(const char* name) {
    mz_zip_archive* zip_archive = new mz_zip_archive;
    memset(zip_archive, 0, sizeof(mz_zip_archive));
    
    archives.push_back({zip_archive, name});
    
    return archives.back();
}

static void OpenArchive(Archive& archive) {
    if (!mz_zip_reader_init_file(archive.zip, archive.name, 0)) {
        Log(Severity::WARNING, System::PLATFORM, "Could not open archive: {}", archive.name);
    }
    
    archive.open = true;
}

static Archive& FindArchive(const char* name) {
    for (auto& archive : archives) {
        if (strcmp(name, archive.name) == 0) return archive;
    }
    
    return AddArchive(name);
}


class ArchiveReader : public FileReader {
public:
    ArchiveReader(const char* archive, const char* path) {
        char archive_copy[50];
        
        if (!archive) {
            size_t seperator = strcspn(path, "/");
            
            strncpy(archive_copy, path, seperator);
            archive_copy[seperator] = '\0';
            
            archive = archive_copy;
            
            if (path[seperator] != '\0') {
                path = path + seperator + 1;
            }
        }
        
        Archive& arc = FindArchive(archive);
        if (!arc.open) OpenArchive(arc);
        
        int file_index = mz_zip_reader_locate_file(arc.zip, path, NULL, 0);
        if (file_index < 0) {
            return;
        }
        
        contents = (char*)mz_zip_reader_extract_to_heap(arc.zip, file_index, &length, 0);
    }

    virtual ~ArchiveReader() {
        mz_free(contents);
    }
    
    virtual const char* GetContents() override {
        return contents;
    }
    
    virtual size_t GetSize() override {
        return length;
    }
    
    virtual FileStatus GetStatus() override {
        if (contents) return FileStatus::READY;
        return FileStatus::IO_ERROR;
    }
    
    virtual void Yeet() override {
        delete this;
    }
    
private:
    char* contents = nullptr;
    size_t length = 0;
};

void AddArchiveFile(const char* name) {
    AddArchive(name);
}

void RegisterArchive() {
    FileReader::Register("archive", [](const char* location, const char* path) -> FileReader* { return new ArchiveReader(location, path); });
}

}
