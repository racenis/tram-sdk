// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <platform/file.h>
#include <framework/logging.h>
#include <templates/hashmap.h>

#include <filesystem>

#include <cassert>
#include <cstring>

#include <config.h>

/**
 * @class tram::Platform::FileReader platform/file.h <platform/file.h>
 * 
 * File reading backend API.
 * 
 * File readers will load a whole file in memory when constructed. This memory
 * is freed when they are destructed, which can be done by calling the Yeet()
 * method.
 * 
 * File contents can be accessed via GetContents() and GetSize() methods.
 * 
 * To open a file, call the GetReader() method, it will find search until it
 * finds the file and then return an opened filereader for it.
 */
 
/**
 * @class tram::Platform::FileWriter platform/file.h <platform/file.h>
 * 
 * File writing backend API.
 * 
 * Works basically the same as FileReader, except this API is used for writing
 * to files, which can be performed with the SetContents() method.
 */

namespace tram {

static const int PROTOCOL_LENGTH = 10;

static std::vector<FileSource> search_list;
static std::vector<int> search_list_index;

struct FileAlias {
    const char* alias;
    const char* protocol;
    const char* location;
};

static std::vector<FileAlias> alias_list;

struct FileReaderProtocolInfo {
    const char* protocol;
    FileReader* (*constr)(const char* location, const char* path);
};

struct FileWriterProtocolInfo {
    const char* protocol;
    FileWriter* (*constr)(const char* location, const char* path);
};

static std::vector<FileReaderProtocolInfo> reader_infos;
static std::vector<FileWriterProtocolInfo> writer_infos;

class DiskReader : public FileReader {
public:
    DiskReader(const char* location, const char* path) {
        char full_path[PATH_LIMIT + 10];
        if (location) {
            snprintf(full_path, PATH_LIMIT + 10, "%s/%s", location, path);
        } else {
            strncpy_s(full_path, PATH_LIMIT + 10, path, -1);
        }
        
        FILE* file_handle = fopen(full_path, "rb");
    
        if (file_handle != nullptr) {
            Log(Severity::INFO, System::PLATFORM, "Opened file for reading: {}", full_path);
        } else {
            Log(Severity::INFO, System::PLATFORM, "Failed to open file for reading: {}", full_path);
            return;
        }
        
        fseek(file_handle, 0, SEEK_END);
        size_t file_size = ftell(file_handle);
        fseek(file_handle, 0, SEEK_SET);
        
        char* file_data = new char[file_size + 1];
        
        if (!fread(file_data, file_size, 1, file_handle)) {
            Log(Severity::WARNING, System::PLATFORM, "Failed to read whole file: {}", full_path);
            delete[] file_data;
            return;
        }
        
        Log(Severity::INFO, System::PLATFORM, "Read {} bytes from file: {}", file_size, full_path);

        file_data[file_size] = '\0';

        fclose(file_handle);
        
        this->contents = file_data;
        this->length = file_size;
        
        Log(Severity::INFO, System::PLATFORM, "Closed file: {}", full_path);
    }
    
    ~DiskReader() {
        delete[] this->contents;
    
        this->contents = nullptr;
        this->length = 0;
    }
    
    const char* GetContents() {
        return contents;
    }
    
    size_t GetSize() {
        return length;
    }
    
    void Yeet() {
        delete this;
    }
    
    FileStatus GetStatus() {
        if (!contents) {
            return FileStatus::IO_ERROR;
        } else {
            return FileStatus::READY;
        }
    }
private:
    const char* contents = nullptr;
    size_t length = 0;
};


static bool ExtractProtocol(const char* path, char* protocol) {
    for (int i = 0; i < PROTOCOL_LENGTH - 1; i++) {
        if (*path == '\0') goto fail;
        
        *protocol++ = *path++;
        
        if (*path == ':' && *++path == '/' && *++path == '/') {
            goto success;
        }
    }
fail:
    *protocol = '\0';
    return false;
success:
    *protocol = '\0';
    return true;
}

static const char* StripProtocol(const char* path) {
    while (*path != '\0' && *path != '/')  path++;
    path++; path++;
    
    return path;
}

FileReader* FileReader::GetReader(const char* path) {
    char protocol[PROTOCOL_LENGTH];
    if (!ExtractProtocol(path, protocol)) {
        for (size_t i = 0; i < search_list.size(); i++) {
            if (search_list_index[i] == -1) {
                auto reader = new DiskReader(search_list[i].location, path);
                if (reader->GetStatus() == FileStatus::READY) return reader;
                
                reader->Yeet();
                continue;
            }
            
            const auto constr = reader_infos[search_list_index[i]].constr;
            if (!constr) continue;
            
            auto reader = constr(search_list[i].location, path);
            if (reader->GetStatus() == FileStatus::READY) return reader;
            
            reader->Yeet();
        }
        
        return new DiskReader(nullptr, path);
    }
    
    if (strcmp("file", protocol) == 0) {
        return new DiskReader(nullptr, StripProtocol(path));
    }
    
    for (auto& p : reader_infos) {
        if (strcmp(p.protocol, protocol)) continue;
        
        if (!p.constr) continue;
        
        return p.constr(nullptr, StripProtocol(path));
    }
    
    Log(Severity::ERROR, System::PLATFORM, "FileReader could not find protocol for {}!", path);
    
    return nullptr;
}

void FileReader::SetSearchList(std::vector<FileSource> list) {
    search_list.clear();
    search_list_index.clear();
    
    for (const auto& source : list) {
        int index = -1;
        for (size_t i = 0; i < reader_infos.size(); i++) {
            if (strcmp(source.protocol, reader_infos[i].protocol)) continue;
            index = i;
            break;
        }
        if (index == -1 && strcmp(source.protocol, "file") != 0) {
            Log(Severity::ERROR, System::PLATFORM, "FileReader search list protocol {} not found in registered protocols!", source.protocol);
            continue;
        }
        search_list.push_back(source);
        search_list_index.push_back(index);
    }
}

std::vector<FileSource> FileReader::GetSearchList() {
    return search_list;
}

void FileReader::Register(const char* protocol, FileReader* (*constr)(const char* location, const char* path)) {
    for (auto& p : reader_infos) {
        if (strcmp(p.protocol, protocol)) continue;
        p.protocol = protocol;
        p.constr = constr;
        return;
    }
    
    reader_infos.push_back({protocol, constr});
}

class DiskWriter : public FileWriter {
public:
    DiskWriter(const char* location, const char* path) {
        this->full_path = new char[PATH_LIMIT + 10];
        this->temp_path = new char[PATH_LIMIT + 10];
        
        if (location) {
            snprintf(full_path, PATH_LIMIT + 10, "%s/%s", location, path);
            snprintf(temp_path, PATH_LIMIT + 10, "%s/%s.tmp", location, path);
        } else {
            snprintf(full_path, PATH_LIMIT + 10, "%s", path);
            snprintf(temp_path, PATH_LIMIT + 10, "%s.tmp", path);
        }
        
        auto directory = std::filesystem::path(full_path).parent_path();
        if (directory != "") std::filesystem::create_directories(directory);
        
        this->file_handle = fopen(temp_path, "wb");
        
        if (file_handle != nullptr) {
            Log(Severity::INFO, System::PLATFORM, "Opened file for writing: {}", path);
        } else {
            Log(Severity::ERROR, System::PLATFORM, "Failed to open file for writing: {}", path);
            return;
        }
    }
    
    ~DiskWriter() {
        if (file_handle) {
            fclose(file_handle);
            
            file_handle = nullptr;
        }
        
        if (!write_failed) {
            remove(full_path);
            rename(temp_path, full_path);
        }
        
        delete[] full_path;
        delete[] temp_path;
    }
    
    void SetContents(const char* contents, size_t size) {
        if (!fwrite(contents, size, 1, file_handle)) {
            Log(Severity::ERROR, System::PLATFORM, "Failed to write to: {}", temp_path);
            write_failed = true;
        }
    }
    
    bool Flush() {
        return true;
    }
    
    void Yeet() {
        delete this;
    }
    
    FileStatus GetStatus() {
        if (!file_handle) {
            return FileStatus::IO_ERROR;
        } else {
            return FileStatus::READY;
        }
    }
private:
    FILE* file_handle = nullptr;
    char* full_path = nullptr;
    char* temp_path = nullptr;
    
    bool write_failed = false;
};

FileWriter* FileWriter::GetWriter(const char* path) {
    char protocol[PROTOCOL_LENGTH];
    if (!ExtractProtocol(path, protocol)) {
        return new DiskWriter(nullptr, path);
    }
    
    if (strcmp("file", protocol) == 0) {
        return new DiskWriter(nullptr, StripProtocol(path));
    }
    
    const char* writer_protocol = protocol;
    const char* writer_location = nullptr;
    
    for (auto& p : alias_list) {
        if (strcmp(p.alias, protocol)) continue;
        writer_protocol = p.protocol;
        writer_location = p.location;
        break;
    }
    
    for (auto& p : writer_infos) {
        if (strcmp(p.protocol, writer_protocol)) continue;

        if (!p.constr) continue;

        return p.constr(writer_location, StripProtocol(path));
    }
    
    Log(Severity::ERROR, System::PLATFORM, "FileWriter could not find protocol for {}!", path);
    
    return nullptr;
}

/// Sets an alias for a protocol.
/// Allows rerouting writes from a protocol to another protocol, with the
/// optional addition of a location.
/// It is possible to clear the alias by setting `protocol` to a `nullptr`.
/// @param alias    Protocol that will serve as the alias.
/// @param protocol Protocol to which the writes will be redirected to.
/// @param location An optional location.
void FileWriter::SetProtocolAlias(const char* alias, const char* protocol, const char* location) {
    for (auto& p : alias_list) {
        if (strcmp(p.alias, alias)) continue;
        p.alias = alias;
        p.protocol = protocol;
        p.location = location;
        return;
    }
    
    if (strcmp(protocol, "file") == 0) {
        alias_list.push_back({alias, protocol, location});
        return;
    }
    
    bool found_protocol = false;
    for (auto& p : writer_infos) {
        if (strcmp(p.protocol, protocol)) continue;
        found_protocol = true; break;
    }
    
    if (!found_protocol) {
        Log(Severity::ERROR, System::PLATFORM, "FileWriter could not alias {} as {} was not found!", alias, protocol);
        return;
    }
    
    alias_list.push_back({alias, protocol, location});
}

void FileWriter::Register(const char* protocol, FileWriter* (*constr)(const char* location, const char* path)) {
    for (auto& p : writer_infos) {
        if (strcmp(p.protocol, protocol)) continue;
        p.protocol = protocol;
        p.constr = constr;
        return;
    }
    
    writer_infos.push_back({protocol, constr});
}

}