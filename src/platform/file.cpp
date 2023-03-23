#include <framework/file.h>
#include <charconv>
#include <cstring>
#include <cassert>
#include <iostream>
#include <vector>

namespace tram {

/// Reads a whole file into memory, which then can be parsed.
class DiskFileReader {
public:
    DiskFileReader (char const* filename) {
        FILE* file_handle = fopen(filename, "rb");
        
        if (file_handle != nullptr) {
            std::cout << "Opened file for reading: " << filename << std::endl;
            success = true;
        } else {
            std::cout << "Failed to open file for reading: " << filename << std::endl;
            success = false;
            return;
        }
        
        fseek(file_handle, 0, SEEK_END);
        size_t file_size = ftell(file_handle);
        fseek(file_handle, 0, SEEK_SET);
        byte_buffer.resize(file_size);
        fread(byte_buffer.data(), file_size, 1, file_handle);
        std::cout << "Read " << file_size << " from " << filename << std::endl;

        fclose(file_handle);
        std::cout << "Closed file: " << filename << std::endl;
    }
    
    ~DiskFileReader() = default;
    
    /// Returns true, if the data has been read successfully.
    bool is_open() {
        return success;
    }
    
    /// Returns pointer to data that has been read.
    const char* read () {
        return byte_buffer.data();
    }
    
    /// Returns the size of a file that has been read.
    size_t length () {
        return byte_buffer.size();
    }
    
    /// Returns the cursor position in file.
    size_t cursor() {
        return cursor_position;
    }
    
    /// Moves the cursor position in file.
    void move_cursor(size_t new_cursor) {
        cursor_position = new_cursor;
    } 
    
private:
    std::vector<char> byte_buffer;
    size_t cursor_position = 0;
    bool success = false;
};

/// Writes a whole file into memory and then flushes it to disk.
class DiskFileWriter {
public:
    DiskFileWriter (char const* filename) {
        file_handle = fopen(filename, "wb");
        
        if (file_handle != nullptr) {
            std::cout << "Opened file: " << filename << std::endl;
        } else {
            std::cout << "Failed to open file: " << filename << std::endl;
            return;
        }
    }
    
    ~DiskFileWriter() = default;

    bool flush() {
        if (file_handle == nullptr) {
            return false;
        }

        fwrite(byte_buffer.data(), byte_buffer.size(), 1, file_handle);

        fclose(file_handle);
        file_handle = nullptr;
        
        return true;
    }
    
    /// Returns true if file has been successfully opened.
    bool is_open() { return file_handle != nullptr; }
    
    /// Writes data to file.
    void write (const char* data, size_t length) {
        size_t last_data = byte_buffer.size();
        byte_buffer.resize(byte_buffer.size() + length);
        mempcpy(byte_buffer.data() + last_data, data, length);
    }
    
    /// Returns the size of a file that has been written.
    size_t length () {
        return byte_buffer.size();
    }
    
private:
    std::vector<char> byte_buffer;
    FILE* file_handle = nullptr;
};

template <typename FileReader> void skip_text_whitespace (FileReader& reader) {
    for (size_t i = reader.cursor(); i < reader.length(); i++) {
        if (!isspace(reader.read()[i])) {
            reader.move_cursor(i);
            return;
        }
    }
    
    reader.move_cursor(reader.length() - 1);
}

template <typename FileReader> void skip_text (FileReader& reader) {
    for (size_t i = reader.cursor(); i < reader.length(); i++) {
        if (isspace(reader.read()[i])) {
            reader.move_cursor(i);
            break;
        }
        
        if (i + 1 == reader.length()) {
            reader.move_cursor(reader.length());
            return;
        }
    }
    
    skip_text_whitespace(reader);
}

template <typename FileReader> bool is_text_continue (FileReader& reader) {
    for (size_t i = reader.cursor(); i < reader.length(); i++) {
        //std::cout << "skipchar: " << reader.read()[i] << std::endl;
        if (!isspace(reader.read()[i])) return true;
    }
    
    return false;
}

template <typename FileReader> int32_t read_text_int32 (FileReader& reader) {
    return atoi (reader.read() + reader.cursor());
}

template <typename FileReader> int64_t read_text_int64 (FileReader& reader) {
    return atol (reader.read() + reader.cursor());
}

template <typename FileReader> float read_text_float32 (FileReader& reader) {
    return atof (reader.read() + reader.cursor());
}

template <typename FileReader> double read_text_float64 (FileReader& reader) {
    return atof (reader.read() + reader.cursor());
}

template <typename FileReader> UID read_text_name(FileReader& reader) {
    char buffer[200];
    char* buf_it = buffer;
    
    for (size_t i = reader.cursor(); i < reader.length() && !isspace(reader.read()[i]); i++) {
        *buf_it++ = reader.read()[i];
    }
    
    *buf_it = '\0';
    return UID(buffer);
}




File::File (char const* name, FileAccessMode mode) : name(name), mode(mode) {
    if (mode == MODE_READ) {
        disk_reader = new DiskFileReader(name);
        skip_text_whitespace(*disk_reader);
    } else {
        abort();
    }
}

File::~File() {
    if (disk_reader) {
        delete disk_reader;
    }
    
    if (disk_writer) {
        delete disk_writer;
    }
}

bool File::is_open() { 
    if (disk_reader) {
        return disk_reader->is_open();
    }
    
    if (disk_writer) {
        return disk_writer->is_open();
    }
    
    return false;
}

bool File::is_continue() {
    if (disk_reader) {
        return is_text_continue (*disk_reader);
    }
    
    if (disk_writer) {
        return true;
    }
    
    return false;
}

void File::write_int8(int8_t value) { abort(); }
void File::write_int16(int16_t value) { abort();  }
void File::write_int32(int32_t value) {abort();  }
void File::write_int64(int64_t value) { abort(); }

void File::write_uint8(uint8_t value) { abort();  }
void File::write_uint16(uint16_t value) { abort();  }
void File::write_uint32(uint32_t value) { abort();  }
void File::write_uint64(uint64_t value) { abort(); }

void File::write_float32(float value) { abort(); }
void File::write_float64(double value) { abort(); }

void File::write_name(name_t value) { abort(); }
void File::write_string(const char* value) { abort(); }

int8_t File::read_int8() { auto ret = read_text_int32(*disk_reader); skip_text (*disk_reader); return ret; }
int16_t File::read_int16() { auto ret = read_text_int32(*disk_reader); skip_text (*disk_reader); return ret; }
int32_t File::read_int32() { auto ret = read_text_int32(*disk_reader); skip_text (*disk_reader); return ret; }
int64_t File::read_int64() { auto ret = read_text_int64(*disk_reader); skip_text (*disk_reader); return ret; }

uint8_t File::read_uint8() { auto ret = read_text_int32(*disk_reader); skip_text (*disk_reader); return ret; }
uint16_t File::read_uint16() { auto ret = read_text_int32(*disk_reader); skip_text (*disk_reader); return ret; }
uint32_t File::read_uint32() { auto ret = read_text_int32(*disk_reader); skip_text (*disk_reader); return ret; }
uint64_t File::read_uint64() { auto ret = read_text_int64(*disk_reader); skip_text (*disk_reader); return ret; }

float File::read_float32() { auto ret = read_text_float32(*disk_reader); skip_text (*disk_reader); return ret; }
double File::read_float64() { auto ret = read_text_float64(*disk_reader); skip_text (*disk_reader); return ret; }

name_t File::read_name() { auto ret = read_text_name(*disk_reader); skip_text (*disk_reader); return ret; }
std::string_view File::read_string() { abort(); }

}