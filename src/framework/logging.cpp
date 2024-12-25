// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#include <framework/logging.h>
#include <framework/system.h>
#include <cstring>
#include <charconv>
#include <iostream>
#include <vector>

namespace tram {

static std::vector <Severity> severities;

void SetSystemLoggingSeverity (System::system_t system, Severity min_severity) {
    if (system >= severities.size()) {
        severities.assign(system + 1, SEVERITY_WARNING);
    }
    
    severities[system] = min_severity;
}

static void(*display_log_callback)(int, const char*) = nullptr;
static void(*console_log_callback)(int, const char*) = nullptr;

void SetDisplayLogCallback(void(*callback)(int, const char*)) {
    display_log_callback = callback;
}

void SetConsoleLogCallback(void(*callback)(int, const char*)) {
    console_log_callback = callback;
}

}

namespace tram::implementation {

// since all of the formats use the same buffer? we should probably add a some
// sort of a lock or something. so that you can Log from multiple threads.
char buffer[500] = {'\0'}; // yes.. haha .. YES!!

void concat_fmt (std::string_view& str) {
    size_t open_bracket = str.find_first_of('{');
    size_t close_bracket = str.find_first_of('}', open_bracket);
    
    // no options for specifying what's inside the bracket
    // ... for now.
    
    strncat(buffer, str.data(), open_bracket);
    
    str.remove_prefix(close_bracket + 1);
}

void flush_console(int severity, int system) {
    const bool severity_known = (size_t) system < severities.size();
    if ((!severity_known && severity < 1) || (severity_known && severity < severities[system])) {
        buffer[0] = '\0';
        return;
    }
    
    const char* severity_text;
    
    switch (severity) {
        case 4:     severity_text = "[    ]";   break;
        case 3:     severity_text = "[CRIT]";   break;
        case 2:     severity_text = "[ERRR]";   break;
        case 1:     severity_text = "[WARN]";   break;
        case 0:     severity_text = "[INFO]";   break;
        default:    severity_text = "[    ]";   break;
    }
    
    const char* system_text = system == 6 ? nullptr : System::GetShortName(system);
    
    if (system_text) {
        std::cout << severity_text << ' ' << '[' << system_text << ']' << ' ';
    }
    
    if (console_log_callback) {
        console_log_callback(severity, buffer);
    }
    
    std::cout << buffer << std::endl;
    buffer[0] = '\0';
}

void flush_display(int severity, int system) {
    if (display_log_callback) {
        display_log_callback(severity, buffer);
    }
    
    buffer[0] = '\0';
}

template <> void concat (const std::string_view& value) {
    strncat(buffer, value.data(), value.size());
}

template <> void concat (const std::string& value) {
    strncat(buffer, value.data(), value.size());
}

template <> void concat (const char* const& value) {
    strcat(buffer, value);
}

template <> void concat (const UID& value) {
    concat<char const*> (value.operator char const*());
}

template <typename T> void to_chars_concat (const T& value) {
    char buffer[100];
    *std::to_chars(buffer, buffer + 100, value).ptr = '\0';
    concat<const char*> (buffer);
}

template <> void concat_numeric (const int64_t& value) {
    to_chars_concat<int64_t>(value);
}

template <> void concat_numeric (const uint64_t& value) {
    to_chars_concat<uint64_t>(value);
}

template <> void concat_numeric (const float& value) {
    to_chars_concat<float>(value);
}

}