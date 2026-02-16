// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_LOGGING_H
#define TRAM_SDK_FRAMEWORK_LOGGING_H

#include <framework/core.h>
#include <framework/uid.h>
#include <framework/system.h>
#include <concepts>

namespace tram {

enum class Severity {
    INFO,           //< For debugging and other verbose messages. Not printed by default.
    WARNING,        //< For errors that are corrected and require no end-user intervention.
    ERROR,          //< For significant erros that can be corrected, but require the end-user to be notified.
    CRITICAL_ERROR, //< For errors that cannot be recovered from.
    DEFAULT         //< Informational messages, always printed.
};

namespace implementation {
    template <typename T> void concat(const T& value) {
        concat<const char*>("LOGGER_UNDEFINED_TYPE");
    }
    
    template <> void concat(const std::string_view& value);
    template <> void concat(const std::string& value);
    template <> void concat(const char* const& value);
    template <> void concat(const UID& value);
    template <size_t N>
    void concat(const char (&value)[N]) {
        concat<const char*>(value);
    }
    
    void concat(const char*);

    template <typename T> void concat_numeric(const T& value) {}
    template <> void concat_numeric(const int64_t& value);
    template <> void concat_numeric(const uint64_t& value);
    template <> void concat_numeric(const float& value);
    
    template <std::signed_integral T> void concat(const T& value) {
        concat_numeric<int64_t>(value);
    }
    
    template <std::unsigned_integral T> void concat(const T& value) {
        concat_numeric<uint64_t>(value);
    }
    
    template <std::floating_point T> void concat(const T& value) {
        concat_numeric<float>(value);
    }
    
    void concat_fmt(std::string_view& str);
    void flush_console(Severity severity, System::system_t system);
    void flush_display(int time, System::system_t system);
    
    
    inline void log(void(*flush)(Severity, System::system_t), Severity severity, System::system_t system, std::string_view& format) {
        concat_fmt(format);
        flush(severity, system);
    }

    template <typename T, typename... Args>
    void log(void(*flush)(Severity, System::system_t), Severity severity, System::system_t system, std::string_view& format, T& value, Args&&... args) {
        concat_fmt(format);
        concat(value);
        
        log(flush, severity, system, format, args...);
    }
}

void SetSystemLoggingSeverity(System::system_t system, Severity min_severity);
void SetFileLogging(bool enabled);

void SetDisplayLogCallback(void(int, const char*));
void SetConsoleLogCallback(void(int, const char*));

template <typename... Args>
void Log(Severity severity, System::system_t system, const std::string_view& format, Args&&... args) {
    std::string_view format_view = format;
    implementation::log(implementation::flush_console, severity, system, format_view, args...);
}

template <typename... Args>
void Log(System::system_t system, const std::string_view& format, Args&&... args) {
    std::string_view format_view = format;
    implementation::log(implementation::flush_console, Severity::DEFAULT, system, format_view, args...);
}

template <typename... Args>
void Log(const std::string_view& format, Args&&... args) {
    std::string_view format_view = format;
    implementation::log(implementation::flush_console, Severity::DEFAULT, 6, format_view, args...);
}

template <typename... Args>
void DisplayLog(int time, const std::string_view& format, Args&&... args) {
    std::string_view format_view = format;
    implementation::log(implementation::flush_display, time, 6, format_view, args...);
}

}

#endif // TRAM_SDK_LOGGING_H