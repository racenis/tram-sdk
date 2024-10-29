// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_FILE_H
#define TRAM_SDK_FRAMEWORK_FILE_H

#include <string>

#include <framework/core.h>
#include <framework/uid.h>

namespace tram {

enum FileAccessMode : uint32_t {
    MODE_READ = 1,
    MODE_WRITE = 2,
    MODE_PAUSE_LINE = 4
};

class FileReader;
class FileWriter;
class FileReaderParser;
class FileWriterParser;

class File {
public:
    File (char const* path, uint32_t mode);
    ~File();
    
    bool is_open();
    bool is_continue();
            
    void write_int8(int8_t value);
    void write_int16(int16_t value);
    void write_int32(int32_t value);
    void write_int64(int64_t value);
    
    void write_uint8(uint8_t value);
    void write_uint16(uint16_t value);
    void write_uint32(uint32_t value);
    void write_uint64(uint64_t value);
    
    void write_float32(float value);
    void write_float64(double value);
    
    void write_name(name_t value);
    void write_string(const char* value);
    void write_newline();
    
    int8_t read_int8();
    int16_t read_int16();
    int32_t read_int32();
    int64_t read_int64();
    
    uint8_t read_uint8();
    uint16_t read_uint16();
    uint32_t read_uint32();
    uint64_t read_uint64();
    
    float read_float32();
    double read_float64();
    
    name_t read_name();
    std::string_view read_string();
    std::string_view read_line();
    
    void skip_linebreak();
    void reset_flags();
    
    bool was_error();
    
    std::string path;
    uint32_t mode;
    
    bool pause_next;
    
    // cursors for writing
    char* buffer = nullptr;
    char* buffer_cursor = nullptr;
    char* buffer_end = nullptr;
    

    
    FileReader* reader = nullptr;
    FileWriter* writer = nullptr;
    FileReaderParser* reader_parser = nullptr;
    FileWriterParser* writer_parser = nullptr;
};
    
}

#endif // TRAM_SDK_FRAMEWORK_FILE_H