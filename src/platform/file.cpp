// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <cassert>
#include <iostream>

#include <cstring>

#include <platform/file.h>
#include <framework/logging.h>
#include <templates/hashmap.h>

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

struct FileReaderProtocolInfo {
    const char* protocol;
    FileReader* (*constr)(const char* path);
};

struct FileWriterProtocolInfo {
    const char* protocol;
    FileWriter* (*constr)(const char* path);
};

static std::vector<FileReaderProtocolInfo> reader_infos;
static std::vector<FileWriterProtocolInfo> writer_infos;

class DiskReader : public FileReader {
public:
    DiskReader(const char* path) {
        FILE* file_handle = fopen(path, "rb");
    
        if (file_handle != nullptr) {
            Log (Severity::INFO, System::PLATFORM, "Opened file for reading: {}", path);
        } else {
            Log (Severity::INFO, System::PLATFORM, "Failed to open file for reading: {}", path);
            return;
        }
        
        fseek(file_handle, 0, SEEK_END);
        size_t file_size = ftell(file_handle);
        fseek(file_handle, 0, SEEK_SET);
        
        char* file_data = new char[file_size + 1];
        
        fread(file_data, file_size, 1, file_handle);
        Log (Severity::INFO, System::PLATFORM, "Read {} bytes from file: {}", file_size, path);

        file_data[file_size] = '\0';

        fclose(file_handle);
        
        this->contents = file_data;
        this->length = file_size;
        
        Log (Severity::INFO, System::PLATFORM, "Closed file: {}", path);
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

FileReader* FileReader::GetReader(const char* path) {
    char protocol[PROTOCOL_LENGTH];
    if (!ExtractProtocol(path, protocol)) {
        for (size_t i = 0; i < search_list.size(); i++) {
            const auto constr = reader_infos[search_list_index[i]].constr;
            if (!constr) continue;
            
            const char* strip_path = path;
            while (*strip_path != '\0' && *strip_path != '/')  strip_path++;
            strip_path++; strip_path++;
            char full_path[PATH_LIMIT + 50];
            snprintf(full_path, PATH_LIMIT + 50, "%s://%s/%s", protocol, search_list[i].prefix, strip_path);
            
            auto reader = constr(path);
            if (reader->GetStatus() == FileStatus::READY) return reader;
            
            delete reader;
        }
        
        return new DiskReader(path);
    }
    
    if (strcmp("file", protocol) == 0) {
        return new DiskReader(path);
    }
    
    for (auto& p : reader_infos) {
        if (strcmp(p.protocol, protocol)) continue;
        
        if (!p.constr) continue;
        
        return p.constr(path);
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
        if (index == -1) {
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

void FileReader::Register(const char* protocol, FileReader* (*constr)(const char* path)) {
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
    DiskWriter(const char* path) {
        this->file_handle = fopen(path, "wb");

        if (file_handle != nullptr) {
            Log (Severity::INFO, System::PLATFORM, "Opened file for writing: {}", path);
        } else {
            Log (Severity::ERROR, System::PLATFORM, "Failed to open file for writing: {}", path);
            return;
        }
    }
    
    ~DiskWriter() {
        if (file_handle) {
            fclose(file_handle);
            
            file_handle = nullptr;
        }
    }
    
    void SetContents(const char* contents, size_t size) {
        fwrite(contents, size, 1, file_handle);
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

};

FileWriter* FileWriter::GetWriter(const char* path) {
    char protocol[PROTOCOL_LENGTH];
    if (!ExtractProtocol(path, protocol)) {
        return new DiskWriter(path);
    }
    
    if (strcmp("file", protocol) == 0) {
        return new DiskWriter(path);
    }
    
    for (auto& p : writer_infos) {
        if (strcmp(p.protocol, protocol)) continue;

        if (!p.constr) continue;

        return p.constr(path);
    }
    
    Log(Severity::ERROR, System::PLATFORM, "FileWriter could not find protocol for {}!", path);
    
    return nullptr;
}

void FileWriter::Register(const char* protocol, FileWriter* (*constr)(const char* path)) {
    for (auto& p : writer_infos) {
        if (strcmp(p.protocol, protocol)) continue;
        p.protocol = protocol;
        p.constr = constr;
        return;
    }
    
    writer_infos.push_back({protocol, constr});
}

}