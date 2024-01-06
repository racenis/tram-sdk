#include <platform/file.h>
#include <framework/file.h>

#include <charconv>
#include <cstring>

namespace tram {

// skips until linebreak
static void skip_text_line(const char*& cursor, const char* cursor_end) {
    while (*cursor != '\r' && *cursor != '\n' && cursor < cursor_end) {
        cursor++;
    }
}

// skips whitespace
static void skip_text_whitespace(const char*& cursor, const char* cursor_end, bool pause_linebreak) {
    while (cursor < cursor_end) {
        if (*cursor == '\n' && pause_linebreak) {
            return;
        } else if (!isspace(*cursor)) {
            if (*cursor != '#') {
                return;
            }
            
            // skips over a comment
            skip_text_line(cursor, cursor_end);
            skip_text_whitespace(cursor, cursor_end, false);
            
            return;
        }
        
        cursor++;
    }
}

// skips non-whitespace
static void skip_text(const char*& cursor, const char* cursor_end, bool pause_linebreak) {
    while (cursor < cursor_end) {
        if (isspace(*cursor)) {
            break;
        }
        
        cursor++;
    }
    
    skip_text_whitespace(cursor, cursor_end, pause_linebreak);
}

static bool is_text_continue(const char* cursor, const char* cursor_end) {
    while (cursor < cursor_end) {
        if (!isspace(*cursor)) return true;
        cursor++;
    }
    
    return false;
}

template <typename T>
auto read_text_from_chars(const char*& cursor, const char* cursor_end) {
    T value;
    cursor = std::from_chars(cursor, cursor_end, value).ptr;
    return value;
}

#ifdef __clang__
template <>
auto read_text_from_chars<float> (const char*& cursor, const char* cursor_end) {
    float value;
    char* new_cursor = nullptr;
    value = strtof(cursor, &new_cursor);
    cursor = new_cursor;
    return value;
}

template <>
auto read_text_from_chars<double> (const char*& cursor, const char* cursor_end) {
    return read_text_from_chars<float> (cursor, cursor_end);
}
#endif

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

UID read_text_name (const char*& cursor, const char* cursor_end) {
    char buffer[200];
    char* buf_it = buffer;
    
    for (const char* cur = cursor; cur < cursor_end && !isspace(*cur); cur++) {
        *buf_it++ = *cur;
    }
    
    *buf_it = '\0';
    
    return UID(buffer);
}

std::string_view read_text_line(const char*& cursor, const char* cursor_end) {
    const char* first_char = cursor;
    
    while (*cursor != '\r' && *cursor != '\n' && cursor < cursor_end) {
        cursor++;
    }
    
    const char* last_char = cursor;
    
    return std::string_view (first_char, last_char - first_char);
}

template <typename T>
void write_text_to_chars (T value, char*& cursor, char* cursor_end) {
    cursor = std::to_chars(cursor, cursor_end, value).ptr;
    *cursor++ = ' ';
    *cursor = '\0';
}

File::File (char const* path, uint32_t mode) : path(path), mode(mode) {
    if (mode & MODE_READ) {
        disk_reader = new FileReader(path, SOURCE_ANY);
        
        if (disk_reader->is_open()) {
            cursor = disk_reader->contents;
            cursor_end = disk_reader->contents + disk_reader->length;
        }
        
        pause_next = mode & MODE_PAUSE_LINE;
        
        skip_text_whitespace(cursor, cursor_end, pause_next);
    } else if (mode & MODE_WRITE) {
        disk_writer = new FileWriter(path, SOURCE_ANY);
        
        if (disk_writer->is_open()) {
            buffer = new char [10000];
            buffer_cursor = buffer;
            buffer_end = buffer + 10000;
        }
    } else {
        abort();
    }
}

File::~File() {
    if (disk_reader) {
        delete disk_reader;
    }
    
    if (disk_writer) {
        disk_writer->write(buffer, buffer_cursor - buffer);
        
        delete disk_writer;
        delete[] buffer;
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

void File::write_int8(int8_t value) { write_text_to_chars (value, buffer_cursor, buffer_end); }
void File::write_int16(int16_t value) { write_text_to_chars (value, buffer_cursor, buffer_end); }
void File::write_int32(int32_t value) { write_text_to_chars (value, buffer_cursor, buffer_end); }
void File::write_int64(int64_t value) { write_text_to_chars (value, buffer_cursor, buffer_end); }

void File::write_uint8(uint8_t value) { write_text_to_chars (value, buffer_cursor, buffer_end); }
void File::write_uint16(uint16_t value) { write_text_to_chars (value, buffer_cursor, buffer_end); }
void File::write_uint32(uint32_t value) { write_text_to_chars (value, buffer_cursor, buffer_end); }
void File::write_uint64(uint64_t value) { write_text_to_chars (value, buffer_cursor, buffer_end); }

void File::write_float32(float value) { write_text_to_chars (value, buffer_cursor, buffer_end); }
void File::write_float64(double value) { write_text_to_chars (value, buffer_cursor, buffer_end); }

void File::write_name(name_t value) { strcpy(buffer_cursor, value); buffer_cursor += strlen(value); *buffer_cursor++ = ' '; }
void File::write_string(const char* value) { *buffer_cursor++ = '"'; strcpy(buffer_cursor, value); buffer_cursor += strlen(value); *buffer_cursor++ = '"'; *buffer_cursor++ = ' '; }
void File::write_newline() { *buffer_cursor++ = '\n'; }

int8_t File::read_int8() { auto ret = read_text_int32(cursor, cursor_end); skip_text(cursor, cursor_end, pause_next); reset_flags(); return ret; }
int16_t File::read_int16() { auto ret = read_text_int32(cursor, cursor_end); skip_text(cursor, cursor_end, pause_next); reset_flags(); return ret; }
int32_t File::read_int32() { auto ret = read_text_int32(cursor, cursor_end); skip_text(cursor, cursor_end, pause_next); reset_flags(); return ret; }
int64_t File::read_int64() { auto ret = read_text_int64(cursor, cursor_end); skip_text(cursor, cursor_end, pause_next); reset_flags(); return ret; }

uint8_t File::read_uint8() { auto ret = read_text_int32(cursor, cursor_end); skip_text(cursor, cursor_end, pause_next); reset_flags(); return ret; }
uint16_t File::read_uint16() { auto ret = read_text_from_chars<uint16_t>(cursor, cursor_end); skip_text_whitespace(cursor, cursor_end, pause_next); reset_flags(); return ret; }
uint32_t File::read_uint32() { auto ret = read_text_int32(cursor, cursor_end); skip_text(cursor, cursor_end, pause_next); reset_flags(); return ret; }
uint64_t File::read_uint64() { auto ret = read_text_int64(cursor, cursor_end); skip_text(cursor, cursor_end, pause_next); reset_flags(); return ret; }

float File::read_float32() { auto ret = read_text_from_chars<float>(cursor, cursor_end); skip_text_whitespace(cursor, cursor_end, pause_next); reset_flags(); return ret; }
double File::read_float64() { auto ret = read_text_from_chars<double>(cursor, cursor_end); skip_text_whitespace(cursor, cursor_end, pause_next); reset_flags(); return ret; }

name_t File::read_name() { auto ret = read_text_name(cursor, cursor_end); skip_text (cursor, cursor_end, pause_next); reset_flags(); return ret; }
std::string_view File::read_string() { abort(); } // TODO: implement
std::string_view File::read_line() { auto ret = read_text_line(cursor, cursor_end); skip_text_whitespace(cursor, cursor_end, pause_next); reset_flags(); return ret; }

void File::skip_linebreak() { skip_text_line(cursor, cursor_end); skip_text_whitespace(cursor, cursor_end, false); }

void File::reset_flags() {
    // nothing here idk
}
}