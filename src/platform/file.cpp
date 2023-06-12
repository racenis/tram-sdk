#include <cassert>
#include <iostream>

#include <platform/file.h>

#include <framework/logging.h>

namespace tram {

/// Loads a file into memory.
/// Use FileReader::is_open() to check if the file was successfully loaded. If
/// it was, then you can access the contents through the FileReader::contents
/// pointer and get the length of the file from FileReader::length.
/// @param path     Path to the file.
/// @param source   Source from which the file will be loaded.
FileReader::FileReader (const char* path, FileSource source) {
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

/// Deletes the loaded file from memory.
/// This will free the memory pointed to by FileReader::contents, so if you need
/// anything from there, you'll need to copy it out before you close the file.
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
/// @param path     Path to the file.
/// @param source   Source to which the file will be written.
FileWriter::FileWriter (const char* path, FileSource source) {
    this->file_handle = fopen(path, "wb");

    if (file_handle != nullptr) {
        Log (SEVERITY_INFO, System::SYSTEM_PLATFORM, "Opened file for writing: {}", path);
    } else {
        Log (SEVERITY_ERROR, System::SYSTEM_PLATFORM, "Failed to open file for writing: {}", path);
        return;
    }
}

/// Closes the file for writing.
FileWriter::~FileWriter () {
    if (is_open()) {
        fclose((FILE*) file_handle);
        
        file_handle = nullptr;
    }
}

/// Writes to a file.
/// @note  This will completely overwrite the file contents. 
/// @param data     Data which will be written to the file.
/// @param length   Length of the data, in bytes, pointed to by data parameter.
void FileWriter::write (const char* data, size_t length) {
    fwrite(data, length, 1, (FILE*) file_handle);
}

/// Checks if a file was opened for writing.
bool FileWriter::is_open() {
    return this->file_handle != nullptr;
}

}