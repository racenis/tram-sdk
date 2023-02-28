#include <framework/file.h>
#include <charconv>
#include <cstring>
#include <cassert>
#include <iostream>
#include <vector>

namespace Core {
    /// Reads a file from filesystem.
    /// When created in read mode, it will try to read all of the file into a
    /// buffer. This buffer can be accessed with read() and length() methods.
    /// When created in write mode, it will try to open a file for writing. It
    /// then is possible to write to the file with write() method.
    class DiskFile {
    public:
        DiskFile (char const* filename, FileAccessMode mode) : mode(mode) {
            switch (mode) {
                case MODE_READ:     file_handle = fopen(filename, "rb"); break;
                case MODE_WRITE:    file_handle = fopen(filename, "wb"); break;
            }
            
            if (is_open()) {
                std::cout << "Opened file: " << filename << std::endl;
            } else {
                std::cout << "Failed to open file: " << filename << std::endl;
            }
            
            if (is_open() && mode == MODE_READ) {
                fseek(file_handle, 0, SEEK_END);
                size_t file_size = ftell(file_handle);
                fseek(file_handle, 0, SEEK_SET);
                byte_buffer.resize(file_size);
                std::cout << "Read " << fread(byte_buffer.data(), file_size, 1, file_handle) << " from " << filename << std::endl;
            }
        }
        
        ~DiskFile() {
            if (is_open() && mode == MODE_READ) {
                fwrite(byte_buffer.data(), byte_buffer.size(), 1, file_handle);
            }
            
            if (is_open()) {
                fclose(file_handle);
                file_handle = nullptr;
                std::cout << "Closed file!" << std::endl;
            }
        }
        
        /// Returns true if file has been successfully opened.
        bool is_open() { return file_handle != nullptr; }
        
        /// Writes data to file.
        void write (char* data, size_t length) {
            assert (mode == MODE_WRITE);
            assert (is_open());
            
            size_t last_data = byte_buffer.size();
            byte_buffer.resize(byte_buffer.size() + length);
            mempcpy(byte_buffer.data() + last_data, data, length);
        }
        
        /// Returns pointer to data pointer.
        char* read () {
            assert (mode == MODE_READ);
            assert (is_open());
            
            return byte_buffer.data();
        }
        
        /// Returns the size of a file that has been read.
        size_t length () {
            return byte_buffer.size();
        }
        
    private:
        std::vector<char> byte_buffer;
        FILE* file_handle = nullptr;
        FileAccessMode mode;
    };
    
    enum Length {
        LENGTH_8,
        LENGTH_16,
        LENGTH_32,
        LENGTH_64
    };
    
    class FileParser {
    public:
        virtual void write_int(int64_t value, Length len) = 0;
        virtual void write_uint(uint64_t value, Length len) = 0;
        virtual void write_float32(float value) = 0;
        virtual void write_float64(double value) = 0;
        
        virtual int64_t read_int() = 0;
        virtual uint64_t read_uint() = 0;
        virtual float read_float32() = 0;
        virtual double read_float64() = 0;
        
        virtual bool is_open() = 0;
        
        virtual ~FileParser() = default;
    };
    
    template<typename FileAccesser>
    class TextParser : public FileParser {
        template <typename T>
        void write_impl(T value, Length len) {
            char buffer[10];
            auto last_char = std::to_chars(buffer, buffer+8, value).ptr;
            *last_char++ = ' ';
            *last_char = '\0';
            auto len_ = last_char-buffer;
            file.write(buffer, len_);
        }
        
        template <typename T>
        T read_impl() {
            T val;
            std::from_chars(iterator, file_end, val);
            return val;
        }
        
    public:
        TextParser(char const* filename, FileAccessMode mode) : file(filename, mode) {
            iterator = file.read();
            file_end = file.read() + file.length();
        }
        
        bool is_open() { return file.is_open(); }
        
        void write_int(int64_t value, Length len) { write_impl<int64_t>(value, len); }
        void write_uint(uint64_t value, Length len) { write_impl<uint64_t>(value, len); }
        void write_float32(float value) { write_impl<float>(value, LENGTH_32); }
        void write_float64(double value) { write_impl<double>(value, LENGTH_64); }
        
        int64_t read_int() { return read_impl<int64_t>(); }
        uint64_t read_uint() { return read_impl<uint64_t>(); }
        float read_float32() { return read_impl<float>(); }
        double read_float64() { return read_impl<double>(); }
        
        FileAccesser file;
        
        char* iterator = nullptr;
        char* file_end = nullptr;
    };
    
    
    File::File (char const* name, FileAccessMode mode) : name(name), mode(mode) {
        parser = new TextParser<DiskFile>(name, mode);
    }
    
    File::~File() {
        delete parser;
    }
    
    bool File::is_open() { return parser->is_open(); }
    
    void File::write_int8(int8_t value) { parser->write_int(value, LENGTH_8); }
    void File::write_int16(int16_t value) { parser->write_int(value, LENGTH_16); }
    void File::write_int32(int32_t value) { parser->write_int(value, LENGTH_32); }
    void File::write_int64(int64_t value) { parser->write_int(value, LENGTH_64); }
    
    void File::write_uint8(uint8_t value) { parser->write_uint(value, LENGTH_8); }
    void File::write_uint16(uint16_t value) { parser->write_uint(value, LENGTH_16); }
    void File::write_uint32(uint32_t value) { parser->write_uint(value, LENGTH_32); }
    void File::write_uint64(uint64_t value) { parser->write_uint(value, LENGTH_64); }
    
    void File::write_float32(float value) { parser->write_float32(value); }
    void File::write_float64(double value) { parser->write_float64(value); }
    
    int8_t File::read_int8() { return parser->read_int(); }
    int16_t File::read_int16() { return parser->read_int(); }
    int32_t File::read_int32() { return parser->read_int(); }
    int64_t File::read_int64() { return parser->read_int(); }
    
    uint8_t File::read_uint8() { return parser->read_uint(); }
    uint16_t File::read_uint16() { return parser->read_uint(); }
    uint32_t File::read_uint32() { return parser->read_uint(); }
    uint64_t File::read_uint64() { return parser->read_uint(); }
    
    float File::read_float32() { return parser->read_float32(); }
    double File::read_float64() { return parser->read_float64(); }


}