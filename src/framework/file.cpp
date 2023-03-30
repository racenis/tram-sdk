#include <platform/file.h>
#include <framework/file.h>

#include <charconv>

namespace tram {

void skip_text_whitespace (const char*& cursor, const char* cursor_end) {
    while (cursor < cursor_end) {
        if (!isspace(*cursor)) {
            return;
        }
        
        cursor++;
    }
}

void skip_text (const char*& cursor, const char* cursor_end) {
    while (cursor < cursor_end) {
        if (isspace(*cursor)) {
            break;
        }
        
        cursor++;
    }
    
    skip_text_whitespace(cursor, cursor_end);
}

bool is_text_continue (const char* cursor, const char* cursor_end) {
    while (cursor < cursor_end) {
        if (!isspace(*cursor)) return true;
        cursor++;
    }
    
    return false;
}

template <typename T>
auto read_text_from_chars (const char*& cursor, const char* cursor_end) {
    T value;
    cursor = std::from_chars(cursor, cursor_end, value).ptr;
    skip_text_whitespace(cursor, cursor_end);
    return value;
}

int32_t read_text_int32 (const char*& cursor, const char* cursor_end) {
    return atoi (cursor);
}

int64_t read_text_int64 (const char*& cursor, const char* cursor_end) {
    return atol (cursor);
}

float read_text_float32 (const char*& cursor, const char* cursor_end) {
    return atof (cursor);
}

double read_text_float64 (const char*& cursor, const char* cursor_end) {
    return atof (cursor);
}

UID read_text_name(const char*& cursor, const char* cursor_end) {
    char buffer[200];
    char* buf_it = buffer;
    
    for (const char* cur = cursor; cur < cursor_end && !isspace(*cur); cur++) {
        *buf_it++ = *cur;
    }
    
    *buf_it = '\0';
    return UID(buffer);
}


File::File (char const* path, FileAccessMode mode) : path(path), mode(mode) {
    if (mode == MODE_READ) {
        disk_reader = new FileReader(path, SOURCE_ANY);
        
        if (disk_reader->is_open()) {
            cursor = disk_reader->contents;
            cursor_end = disk_reader->contents + disk_reader->length;
        }
        
        skip_text_whitespace(cursor, cursor_end);
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
        return is_text_continue (cursor, cursor_end);
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

int8_t File::read_int8() { auto ret = read_text_int32(cursor, cursor_end); skip_text (cursor, cursor_end); return ret; }
int16_t File::read_int16() { auto ret = read_text_int32(cursor, cursor_end); skip_text (cursor, cursor_end); return ret; }
int32_t File::read_int32() { auto ret = read_text_int32(cursor, cursor_end); skip_text (cursor, cursor_end); return ret; }
int64_t File::read_int64() { auto ret = read_text_int64(cursor, cursor_end); skip_text (cursor, cursor_end); return ret; }

uint8_t File::read_uint8() { auto ret = read_text_int32(cursor, cursor_end); skip_text (cursor, cursor_end); return ret; }
uint16_t File::read_uint16() { auto ret = read_text_int32(cursor, cursor_end); skip_text (cursor, cursor_end); return ret; }
uint32_t File::read_uint32() { auto ret = read_text_int32(cursor, cursor_end); skip_text (cursor, cursor_end); return ret; }
uint64_t File::read_uint64() { auto ret = read_text_int64(cursor, cursor_end); skip_text (cursor, cursor_end); return ret; }

float File::read_float32() { return read_text_from_chars<float>(cursor, cursor_end); }
double File::read_float64() { return read_text_from_chars<double>(cursor, cursor_end); }

name_t File::read_name() { auto ret = read_text_name(cursor, cursor_end); skip_text (cursor, cursor_end); return ret; }
std::string_view File::read_string() { abort(); }

}