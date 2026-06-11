// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/logging.h>
#include <framework/system.h>
#include <platform/terminal.h>
#include <platform/other.h>
#include <cstring>
#include <ctime>
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
static FILE* log_file = nullptr;

/// Sets the logging severity filter.
/// @param system       System for which the filter will apply.
/// @param min_severity The lowest severity which the filter will let through
///                     All lower severities will be filtered out.
void SetSystemLoggingSeverity(System::system_t system, Severity min_severity) {
    if (system >= severities.size()) {
        severities.assign(system + 1, Severity::NOTE);
    }
    
    severities[system] = min_severity;
}

/// Sets file logging.
void SetFileLogging(bool enabled) {
    if (!enabled) {
        if (!log_file) return;
        
        fclose(log_file);
        log_file = nullptr;
        
        return;
    }
    
    std::time_t epoch_time;
    std::time(&epoch_time);
    std::tm date_time = *std::localtime(&epoch_time);
    
    char time_str[100];
    std::strftime(time_str, 100, "--%Y-%b-%d--%H-%M-%S", &date_time);
    
    char file_name[200];
    strcpy(file_name, "application-log");
    strcat(file_name, time_str);
    strcat(file_name, ".txt");
    
    log_file = fopen(file_name, "w");
    setvbuf(log_file, NULL, _IOFBF, 8192);
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

thread_local char buffer[500] = {'\0'}; // yes.. haha .. YES!!

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
    if ((!severity_known && severity < Severity::NOTE) || (severity_known && severity < severities[system])) {
        buffer[0] = '\0';
        return;
    }
    
    using namespace Platform;
    
    const char* severity_text = nullptr;
    Platform::TerminalColor color = TerminalColor::DEFAULT;
    
    switch (severity) {
        case Severity::DEFAULT:         severity_text = "[    ]";   break;
        case Severity::WARNING:         severity_text = "[WARN]";   break;
        case Severity::ERROR:           severity_text = "[ERRR]";   break;
        case Severity::CRITICAL_ERROR:  severity_text = "[CRIT]";   break;
        case Severity::INFO:            severity_text = "[INFO]";   break;
        case Severity::NOTE:            severity_text = "[NOTE]";   break;
        default:                        severity_text = "[    ]";   break;     
    }
    
    switch (severity) {
        case Severity::DEFAULT:         color = TerminalColor::DEFAULT;      break;
        case Severity::WARNING:         color = TerminalColor::LIGHT_YELLOW; break;
        case Severity::ERROR:           color = TerminalColor::LIGHT_RED;    break;
        case Severity::CRITICAL_ERROR:  color = TerminalColor::LIGHT_RED;    break;
        case Severity::INFO:            color = TerminalColor::DEFAULT;      break;
        case Severity::NOTE:            color = TerminalColor::DEFAULT;      break;
        default:                        color = TerminalColor::DEFAULT;      break;     
    }
    
    const char* system_text = /*system == 6 ? nullptr :*/ System::GetShortName(system);
    
    if (color != TerminalColor::DEFAULT) {
        SwitchForeground(color);
    }
    
    char flush_buffer[489] = {'\0'};
    if (system_text) {
        char padding[16];
        int chars = 6 - strlen(system_text);
        for (int i = 0; i <= chars; i++) padding[i] = ' ';
        if (chars < 0) chars = 0;
        if (chars >= 15) chars = 15;
        padding[chars] = '\0';
        
        sprintf(flush_buffer, "%s [%s]%s ", severity_text, system_text, padding);
    }
    
    if (console_log_callback) {
        console_log_callback(0, buffer);
    }
    
    int segment_start = 0;
    int segment_length = 0;
    int last_space = 0;
    for (int i = 0;; i++) {
        if (isspace(buffer[i])) last_space = i;
        if (++segment_length < 64 && buffer[i] != '\0') continue;
        
        if (segment_start != 0) strcat(flush_buffer, "                ");
        
        if (buffer[i] != '\0') buffer[last_space] = '\0';
        strcat(flush_buffer, &buffer[segment_start]);
        strcat(flush_buffer, "\n");
        
        if (buffer[i] == '\0') {
            break;
        }
        
        segment_length = 0;
        segment_start = last_space + 1;
        i = last_space;
    }
    
    std::cout << flush_buffer << std::flush;
    
    if (color != TerminalColor::DEFAULT) {
        SwitchForeground(TerminalColor::DEFAULT);
    }
    
    if (log_file) {
        if (system_text) {
            fprintf(log_file, "%s [%s] %s\n", severity_text, system_text, buffer);
        } else {
            fprintf(log_file, "%s\n", buffer);
        }
        
    }
    
    
    // some people might say "loggers shouldn't act as asserts" but they are wrong!
    // consider this:
    // purpose of logger:       catching bugs
    // purpose of an asserter:  catching bugs
    // see? things should be grouped by what they do, not what they say they do
    
    switch (severity) {
        default: break; 
        case Severity::ERROR:
            Platform::TryDebugging();
            Platform::ShowErrorDialog(buffer, "Very bad error");
            break;
        case Severity::CRITICAL_ERROR:
            Platform::TryDebugging();
            Platform::ShowErrorDialog(buffer, "Fatal error");
            abort();
            break;
    }
    
    buffer[0] = '\0';
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