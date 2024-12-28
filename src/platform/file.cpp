// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <cassert>
#include <iostream>

#include <platform/file.h>

#include <framework/logging.h>

/**
 * @class tram::Platform::FileReader
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
 * @class tram::Platform::FileWriter
 * 
 * File writing backend API.
 * 
 * Works basically the same as FileReader, except this API is used for writing
 * to files, which can be performed with the SetContents() method.
 */

namespace tram {

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


// TODO: implemenet shtese ones okay
FileWriter* FileWriter::GetWriter(const char* path) {
    return new DiskWriter(path);
}


void FileWriter::SetMediumList() {
    
}

std::vector<FileSource> FileWriter::GetMediumList() {
    return {};
}

}