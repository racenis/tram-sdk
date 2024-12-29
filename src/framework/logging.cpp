// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/logging.h>
#include <framework/system.h>
#include <cstring>
#include <charconv>
#include <iostream>
#include <vector>

/**
 * @file framework/logging.cpp
 * 
 * Logging and console printing services.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/logging.html
 */

namespace tram {

static std::vector<Severity> severities;

/// Sets the logging severity filter.
/// @param system       System for which the filter will apply.
/// @param min_severity The lowest severity which the filter will let through
///                     All lower severities will be filtered out.
void SetSystemLoggingSeverity(System::system_t system, Severity min_severity) {
    if (system >= severities.size()) {
        severities.assign(system + 1, Severity::WARNING);
    }
    
    severities[system] = min_severity;
}

static void(*display_log_callback)(int, const char*) = nullptr;
static void(*console_log_callback)(int, const char*) = nullptr;

/// Sets the display log callback.
/// When a message is logged by calling DisplayLog(), it will be formatted and
/// then the result will be passed in to the display log callback.
void SetDisplayLogCallback(void(*callback)(int, const char*)) {
    display_log_callback = callback;
}

/// Sets the console log callback.
/// Whenever a message is written to console, it will alos be passed to the
/// console log callback.
void SetConsoleLogCallback(void(*callback)(int, const char*)) {
    console_log_callback = callback;
}

}

namespace tram::implementation {

// since all of the formats use the same buffer? we should probably add a some
// sort of a lock or something. so that you can Log from multiple threads.
// actually a better idea would be to allocate this on the Log() template
// functions stack and then pass a pointer down into concatfmt etc.
char buffer[500] = {'\0'}; // yes.. haha .. YES!!

void concat_fmt(std::string_view& str) {
    size_t open_bracket = str.find_first_of('{');
    size_t close_bracket = str.find_first_of('}', open_bracket);
    
    // no options for specifying what's inside the bracket
    // ... for now.
    
    strncat(buffer, str.data(), open_bracket);
    
    str.remove_prefix(close_bracket + 1);
}

void flush_console(Severity severity, System::system_t system) {
    const bool severity_known = (size_t) system < severities.size();
    if ((!severity_known && severity < Severity::WARNING) || (severity_known && severity < severities[system])) {
        buffer[0] = '\0';
        return;
    }
    
    const char* severity_text = nullptr;
    
    switch (severity) {
        case Severity::DEFAULT:         severity_text = "[    ]";   break;
        case Severity::WARNING:         severity_text = "[WARN]";   break;
        case Severity::ERROR:           severity_text = "[ERRR]";   break;
        case Severity::CRITICAL_ERROR:  severity_text = "[CRIT]";   break;
        case Severity::INFO:            severity_text = "[INFO]";   break;
        default:                        severity_text = "[    ]";   break;     
    }
    
    const char* system_text = system == 6 ? nullptr : System::GetShortName(system);
    
    if (system_text) {
        std::cout << severity_text << ' ' << '[' << system_text << ']' << ' ';
    }
    
    if (console_log_callback) {
        console_log_callback(0, buffer);
    }
    
    std::cout << buffer << std::endl;
    buffer[0] = '\0';
    
    // some people might say "loggers shouldn't act as asserts" but they are wrong!
    // consider this:
    // purpose of logger:       catching bugs
    // purpose of an asserter:  catching bugs
    // see? things should be grouped by what they do, not what they say they do
    
    if (severity == Severity::CRITICAL_ERROR) abort();
}

void flush_display(int time, int system) {
    if (display_log_callback) {
        display_log_callback(time, buffer);
    }
    
    buffer[0] = '\0';
}

template <> void concat(const std::string_view& value) {
    strncat(buffer, value.data(), value.size());
}

template <> void concat(const std::string& value) {
    strncat(buffer, value.data(), value.size());
}

template <> void concat(const char* const& value) {
    strcat(buffer, value);
}

void concat(const char* value) {
    strcat(buffer, value);
}


template <> void concat(const UID& value) {
    concat<char const*> (value.operator char const*());
}

template <typename T> void to_chars_concat(const T& value) {
    char buffer[100];
    *std::to_chars(buffer, buffer + 100, value).ptr = '\0';
    concat<const char*> (buffer);
}

template <> void concat_numeric(const int64_t& value) {
    to_chars_concat<int64_t>(value);
}

template <> void concat_numeric(const uint64_t& value) {
    to_chars_concat<uint64_t>(value);
}

template <> void concat_numeric(const float& value) {
    to_chars_concat<float>(value);
}

}