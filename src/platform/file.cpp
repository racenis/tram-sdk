#include <cassert>
#include <iostream>

#include <platform/file.h>

#include <framework/logging.h>

namespace tram {

class DiskReader : public FileReader {
public:
    DiskReader(const char* path) {
        FILE* file_handle = fopen(path, "rb");
    
        if (file_handle != nullptr) {
            Log (SEVERITY_INFO, System::SYSTEM_PLATFORM, "Opened file for reading: {}", path);
        } else {
            Log (SEVERITY_INFO, System::SYSTEM_PLATFORM, "Failed to open file for reading: {}", path);
            return;
        }
        
        fseek(file_handle, 0, SEEK_END);
        size_t file_size = ftell(file_handle);
        fseek(file_handle, 0, SEEK_SET);
        
        char* file_data = new char[file_size + 1];
        
        fread(file_data, file_size, 1, file_handle);
        Log (SEVERITY_INFO, System::SYSTEM_PLATFORM, "Read {} bytes from file: {}", file_size, path);

        file_data[file_size] = '\0';

        fclose(file_handle);
        
        this->contents = file_data;
        this->length = file_size;
        
        Log (SEVERITY_INFO, System::SYSTEM_PLATFORM, "Closed file: {}", path);
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



// TODO: implement sthese ones okey
FileReader* FileReader::GetReader(const char* path) {
    return new DiskReader(path);
}

void FileReader::SetSearchList() {
    
}

std::vector<FileSource> FileReader::GetSearchList() {
    return {};
}






class DiskWriter : public FileWriter {
public:
    DiskWriter(const char* path) {
        this->file_handle = fopen(path, "wb");

        if (file_handle != nullptr) {
            Log (SEVERITY_INFO, System::SYSTEM_PLATFORM, "Opened file for writing: {}", path);
        } else {
            Log (SEVERITY_ERROR, System::SYSTEM_PLATFORM, "Failed to open file for writing: {}", path);
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


// TODO: implemenet shtese ones okay
FileWriter* FileWriter::GetWriter(const char* path, FileMedium medium) {
    return new DiskWriter(path);
}


void FileWriter::SetMediumList() {
    
}

std::vector<FileSource> FileWriter::GetMediumList() {
    return {};
}

}