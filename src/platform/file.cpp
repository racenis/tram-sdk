#include <framework/file.h>
#include <charconv>
#include <cstring>
#include <cassert>
#include <iostream>
#include <vector>

#include <platform/file.h>

namespace tram {

/// This will attempt to read file into memory.
FileReader::FileReader (const char* path, FileSource source) {
    FILE* file_handle = fopen(path, "rb");
    
    if (file_handle != nullptr) {
        std::cout << "Opened file for reading: " << path << std::endl;
    } else {
        std::cout << "Failed to open file for reading: " << path << std::endl;
        return;
    }
    
    fseek(file_handle, 0, SEEK_END);
    size_t file_size = ftell(file_handle);
    fseek(file_handle, 0, SEEK_SET);
    
    char* file_data = new char[file_size];
    
    fread(file_data, file_size, 1, file_handle);
    
    std::cout << "Read " << file_size << " from " << path << std::endl;

    fclose(file_handle);
    
    this->contents = file_data;
    this->length = file_size;
    std::cout << "Closed file: " << path << std::endl;
}

/// Deletes the loaded file from memory.
FileReader::~FileReader () {
    delete[] this->contents;
    
    this->contents = nullptr;
    this->length = 0;
}

/// Checks if file was loaded into memory.
bool FileReader::is_open() {
    return this->contents != nullptr;
}



/// Opens a file for writing.
FileWriter::FileWriter (const char* path, FileSource source) {
    this->file_handle = fopen(path, "wb");

    if (file_handle != nullptr) {
        std::cout << "Opened file: " << path << std::endl;
        
        this->buffer = new char[1000]; // hehe
    } else {
        std::cout << "Failed to open file: " << path << std::endl;
        return;
    }
}

/// Closes the file for writing.
FileWriter::~FileWriter () {
    if (is_open() && buffer) {
        fwrite(buffer, buffer_length, 1, (FILE*) file_handle);
    }
    
    if (is_open()) {
        fclose((FILE*) file_handle);
        
        file_handle = nullptr;
    }

    if (buffer) {
        delete[] buffer;
        
        buffer = nullptr;
        buffer_length = 0;
    }
}

/// Writes to a file.
void FileWriter::write (const char* data, size_t length) {
    assert(length + buffer_length > 1000);
    
    memcpy(buffer + buffer_length, data, length);
    
    buffer_length += length;
}

/// Checks if a file was opened for writing.
bool FileWriter::is_open() {
    return this->file_handle != nullptr;
}


}