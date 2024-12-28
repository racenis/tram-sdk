// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <platform/file.h>
#include <framework/file.h>
#include <framework/logging.h>

#include <charconv>
#include <cstring>

/**
 * @struct tram::File framework/file.h
 * 
 * Very fast file reader/writer.
 * 
 * For reading a file, set the File::READ flag in the mode bitmask.
 * The file, referenced by the path, will be immediately opened, if possible.
 * After opening, it will be fully read into memory. After that you can use
 * all of the read_xyz() methods.
 * 
 * To check if the file has ended and no more reading can be done, use the
 * is_continue() method.
 * 
 * To check whether the file was opened, use the is_open() method.
 * 
 * For writing to a file, set the File::WRITE flag in the mode bitmask.
 * 
 * After that you can use the various write_xyz() methods.
 * 
 * All of the writing will take place in a large buffer, which after the File
 * class is destructed will be written to disk.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/file.html
 */

namespace tram {
    
class FileReaderParser {
public:
    virtual int8_t read_int8() = 0;
    virtual int16_t read_int16() = 0;
    virtual int32_t read_int32() = 0;
    virtual int64_t read_int64() = 0;
    
    virtual uint8_t read_uint8() = 0;
    virtual uint16_t read_uint16() = 0;
    virtual uint32_t read_uint32() = 0;
    virtual uint64_t read_uint64() = 0;
    
    virtual float read_float32() = 0;
    virtual double read_float64() = 0;
    
    virtual name_t read_name() = 0;
    virtual std::string_view read_string() = 0;
    virtual std::string_view read_line() = 0;
    
    virtual void skip_newline() = 0;
    virtual void skip_whitespace() = 0;
    
    virtual bool is_error() = 0;
    virtual void reset_error() = 0;
    
    virtual bool is_continue() = 0;
    
    void set_skip_value(bool skip_value) { this->skip_value = skip_value; }
    void set_skip_newline(bool skip_newline) { this->skip_newline_flag = skip_newline; }
protected:
    bool skip_newline_flag = true;
    bool skip_value = true;
};

class TextReaderParser : public FileReaderParser {
public:
    TextReaderParser(FileReader* reader) {
        this->cur = reader->GetContents();
        this->end = reader->GetContents() + reader->GetSize();
    }
    
    int8_t read_int8() {
        return from_chars<int8_t>();
    }
    
    int16_t read_int16() {
        return from_chars<int16_t>();
    }
    
    int32_t read_int32() {
        return from_chars<int32_t>();
    }
    
    int64_t read_int64() {
        return from_chars<int64_t>();
    }
    
    
    uint8_t read_uint8() {
        return from_chars<uint8_t>();
    }
    
    uint16_t read_uint16() {
        return from_chars<uint16_t>();
    }
    
    uint32_t read_uint32() {
        return from_chars<uint32_t>();
    }
    
    uint64_t read_uint64() {
        return from_chars<uint64_t>();
    }
    
    
    float read_float32() {
        return from_chars<float>();
    }
    
    double read_float64() {
        return from_chars<double>();
    }
    
    
    name_t read_name() {
        char buffer[200];
        char* buf_it = buffer;
        
        const char* ncur = cur;
        for (; ncur < end && !isspace(*ncur); ncur++) {
            *buf_it++ = *ncur;
        }
        
        if (skip_value) {
            cur = ncur;
        }
        
        error_flag = false;
        
        *buf_it = '\0';
        
        if (strlen(buffer) == 0 ) Log("NOT GOOD ZERO NAME");
        
        return UID(buffer);
    }
    
    std::string_view read_string() {
        char delimiter = *cur;
        cur++;
        
        const char* begin = cur;
        size_t length = 0;
        
        for (; *cur != delimiter && cur < end; cur++) {
            length++;
        }
        
        cur++;
        
        return {begin, length};
    }
    
    std::string_view read_line() {
        const char* first_char = cur;
        const char* last_char = cur;
    
        while (*last_char != '\r' && *last_char != '\n' && last_char < end) {
            last_char++;
        }
        
        size_t line_length = last_char - first_char;
        
        if (skip_value) {
            cur = last_char;
        }
        
        return std::string_view (first_char, line_length);
    }
    
    
    void skip_newline() {
        while (*cur != '\r' && *cur != '\n' && cur < end) {
            cur++;
        }
        
        while (cur < end && (*cur == '\n' || *cur == '\r')) cur++;
    }
    
    void skip_whitespace() {
        while (cur < end) {
            if (*cur == '\n' && !skip_newline_flag) {
                return;
            } else if (!isspace(*cur)) {
                if (*cur != '#') {
                    return;
                }
                
                // skip until the end of the line
                while (cur < end && *cur != '\n' && *cur != '\r') cur++;
                
                // skip over linebreak
                while (cur < end && (*cur == '\n' || *cur == '\r')) cur++;
                
                // skip over comments, if any on next line
                skip_whitespace();
                
                return;
            }
            
            cur++;
        }
    }
    
    bool is_continue() {
        while (cur < end) {
            if (!isspace(*cur)) return true;
            cur++;
        }
        
        return false;
    }
    
    bool is_error() {
        return error_flag;
    }
    
    void reset_error() {
        error_flag = false;
    }
private:
    template <typename T>
    T from_chars() {
        T value;
        std::from_chars_result result = std::from_chars(cur, end, value);
        if (result.ec != std::errc()) {
            error_flag = true;
        }
        if (skip_value) {
            cur = result.ptr;
        }
        return value;
    }
    
#ifdef __clang__
    template <>
    float from_chars<float>() {
        float value;
        char* new_cursor = nullptr;
        value = strtof(cur, &new_cursor);
        
        if (skip_value) {
            cur = new_cursor;
        }
        
        return value;
    }

    template <>
    double from_chars<double>() {
        return from_chars<float>();
    }
#endif

    const char* cur = nullptr;
    const char* end = nullptr;
    bool error_flag = false;
};

class FileWriterParser {
public:
    virtual void write_int8(int8_t value) = 0;
    virtual void write_int16(int16_t value) = 0;
    virtual void write_int32(int32_t value) = 0;
    virtual void write_int64(int64_t value) = 0;
    
    virtual void write_uint8(uint8_t value) = 0;
    virtual void write_uint16(uint16_t value) = 0;
    virtual void write_uint32(uint32_t value) = 0;
    virtual void write_uint64(uint64_t value) = 0;
    
    virtual void write_float32(float value) = 0;
    virtual void write_float64(double value) = 0;
    
    virtual void write_name(name_t value) = 0;
    virtual void write_string(const char* value) = 0;
    virtual void write_newline() = 0;
};

class TextWriterParser : public FileWriterParser {
public:
    TextWriterParser(FileWriter* writer) {
        this->writer = writer;
    }
    
    virtual void write_int8(int8_t value) {
        write_to_chars(value);
    }
    
    virtual void write_int16(int16_t value) {
        write_to_chars(value);
    }
    
    virtual void write_int32(int32_t value) {
        write_to_chars(value);
    }
    
    virtual void write_int64(int64_t value) {
        write_to_chars(value);
    }
    
    
    virtual void write_uint8(uint8_t value) {
        write_to_chars(value);
    }
    
    virtual void write_uint16(uint16_t value) {
        write_to_chars(value);
    }
    
    virtual void write_uint32(uint32_t value) {
        write_to_chars(value);
    }
    
    virtual void write_uint64(uint64_t value) {
        write_to_chars(value);
    }
    
    
    virtual void write_float32(float value) {
        write_to_chars(value);
    }
    
    virtual void write_float64(double value) {
        write_to_chars(value);
    }
    
    
    virtual void write_name(name_t value) {
        writer->SetContents(value, strlen(value));
        writer->SetContents(" ", 1);
    }
    
    virtual void write_string(const char* value)  {
        writer->SetContents(value, strlen(value));
        writer->SetContents(" ", 1);
    }
    
    virtual void write_newline() {
        writer->SetContents("\n", 1);
    }
    
private:
    template <typename T>
    void write_to_chars(T value) {
        char buffer[25];
        char* begin = buffer;
        char* end = buffer + 24;
        end = std::to_chars(begin, end, value).ptr;
        *end = ' ';
        end++;
        writer->SetContents(buffer, end - begin);
    }
    FileWriter* writer = nullptr;
};

/// Opens a file.
/// @param path Path to the file. If no prefix is set, this path will be
///             interpreted as being relative to the project/executable
///             working directory.
/// @param mode Mode is a bitmask consisting of flags that can be found in the
///             AccessMode enum.
File::File(char const* path, uint32_t mode) : path(path), mode(mode) {
    if (mode & BINARY) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Sorry, File::BINARY for files not implemented yet");
    }
    
    if (mode & READ) {
        reader = FileReader::GetReader(path);
        
        if (reader->GetStatus() == FileStatus::READY) {
            reader_parser = new TextReaderParser(reader);
        }
        
        if (mode & PAUSE_LINE) {
            reader_parser->set_skip_newline(false);
        }
        
    } else if (mode & WRITE) {
        writer = FileWriter::GetWriter(path);
        
        if (writer->GetStatus() == FileStatus::READY) {
            writer_parser = new TextWriterParser(writer);
        }
        
    } else {
        Log(Severity::CRITICAL_ERROR, System::CORE, "File doesn't have File::READ or File::WRITE flag set");
    }
}

File::~File() {
    if (reader) {
        reader->Yeet();
    }
    
    if (writer) {
        writer->Yeet();
    }
}

/// Returns true if file was opened.
bool File::is_open() { 
    if (reader) {
        return reader->GetStatus() == FileStatus::READY;
    }
    
    if (writer) {
        return writer->GetStatus() == FileStatus::READY;
    }
    
    return false;
}

/// Returns true if file parser has not reached end.
bool File::is_continue() {
    if (reader_parser) {
        return reader_parser->is_continue();
    }
    
    if (writer_parser) {
        return true;
    }
    
    return false;
}

void File::write_int8(int8_t value) { writer_parser->write_int8(value); }
void File::write_int16(int16_t value) { writer_parser->write_int16(value); }
void File::write_int32(int32_t value) { writer_parser->write_int32(value); }
void File::write_int64(int64_t value) { writer_parser->write_int64(value); }

void File::write_uint8(uint8_t value) { writer_parser->write_uint8(value); }
void File::write_uint16(uint16_t value) { writer_parser->write_uint16(value); }
void File::write_uint32(uint32_t value) { writer_parser->write_uint32(value); }
void File::write_uint64(uint64_t value) { writer_parser->write_uint64(value); }

void File::write_float32(float value) { writer_parser->write_float32(value);}
void File::write_float64(double value) { writer_parser->write_float64(value); }

void File::write_name(name_t value) { writer_parser->write_name(value); }
void File::write_string(const char* value) { writer_parser->write_string(value); }

/// Writes a newline to the file.
/// The newline is just the `\n` character.
void File::write_newline() { writer_parser->write_newline(); }

int8_t File::read_int8() { reader_parser->skip_whitespace(); return reader_parser->read_int8(); }
int16_t File::read_int16() { reader_parser->skip_whitespace(); return reader_parser->read_int16(); }
int32_t File::read_int32() { reader_parser->skip_whitespace(); return reader_parser->read_int32(); }
int64_t File::read_int64() { reader_parser->skip_whitespace(); return reader_parser->read_int64(); }

uint8_t File::read_uint8() { reader_parser->skip_whitespace(); return reader_parser->read_uint8(); }
uint16_t File::read_uint16() { reader_parser->skip_whitespace(); return reader_parser->read_uint16(); }
uint32_t File::read_uint32() { reader_parser->skip_whitespace(); return reader_parser->read_uint32(); }
uint64_t File::read_uint64() { reader_parser->skip_whitespace(); return reader_parser->read_uint64(); }

float File::read_float32() { reader_parser->skip_whitespace(); return reader_parser->read_float32(); }
double File::read_float64() { reader_parser->skip_whitespace(); return reader_parser->read_float64(); }

name_t File::read_name() { reader_parser->skip_whitespace(); return reader_parser->read_name(); }
std::string_view File::read_string() { reader_parser->skip_whitespace(); return reader_parser->read_string(); }

/// Parses off the remaining line.
std::string_view File::read_line() { return reader_parser->read_line(); }

/// Skips over a linebreak.
/// Probably only useful if the file has been opened for reading in the
/// `MODE_PAUSE_LINE` mode, since the parser will stop at any new line and the
/// only way to continue parsing is to use this method.
void File::skip_linebreak() { reader_parser->skip_newline(); }

/// Resets the error flag.
void File::reset_flags() {
    if (reader_parser) reader_parser->reset_error();
}

/// Checks if there has been an error.
/// The error flag does not get cleared on its own, so if an error ocurred
/// anywhere, this method will report that.
/// To determine whether a certain value has been parsed correctly, first use
/// the reset_flags() method to reset the error flag, then use the read_xyz()
/// method to read in the value and then use this method to determine whether
/// there was an error in parsing that specific method.
bool File::was_error() {
    if (reader_parser) return reader_parser->is_error();
    return false;
}

}