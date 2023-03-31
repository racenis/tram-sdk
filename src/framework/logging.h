// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_LOGGING_H
#define TRAM_SDK_FRAMEWORK_LOGGING_H

#include <framework/core.h>
#include <framework/uid.h>
#include <concepts>

namespace tram {
    namespace implementation {
        template <typename T> void concat (const T& value) {
            concat<const char*> ("LOGGER_UNDEFINED_TYPE");
        }
        
        template <> void concat (const std::string_view& value);
        template <> void concat (const std::string& value);
        template <> void concat (const char* const& value);
        template <> void concat (const UID& value);
        
        template <typename T> void concat_numeric (const T& value) {}
        template <> void concat_numeric (const int64_t& value);
        template <> void concat_numeric (const uint64_t& value);
        template <> void concat_numeric (const float& value);
        
        template <std::signed_integral T> void concat (const T& value) {
            concat_numeric<int64_t>(value);
        }
        
        template <std::unsigned_integral T> void concat (const T& value) {
            concat_numeric<uint64_t>(value);
        }
        
        template <std::floating_point T> void concat (const T& value) {
            concat_numeric<float>(value);
        }
        
        void concat_fmt (std::string_view& str);
        void flush (int severity, int system);
        
        
        inline void log (int severity, int system, std::string_view& format) {
            concat_fmt(format);
            flush(severity, system);
        }

        template <typename T, typename... Args>
        void log (int severity, int system, std::string_view& format, T value, Args&&... args) {
            concat_fmt(format);
            concat<T>(value);
            
            log(severity, system, format, args...);
        }
    }
    
    enum Severity {
        SEVERITY_DEFAULT,
        SEVERITY_CRITICAL_ERROR,
        SEVERITY_ERROR,
        SEVERITY_WARNING,
        SEVERITY_INFO
    };
    
    template <typename... Args>
    void Log (int severity, int system, const std::string_view& format, Args&&... args) {
        std::string_view format_view = format;
        implementation::log (severity, system, format_view, args...);
    }
    
    template <typename... Args>
    void Log (int system, const std::string_view& format, Args&&... args) {
        std::string_view format_view = format;
        implementation::log (0, system, format_view, args...);
    }
    
    template <typename... Args>
    void Log (const std::string_view& format, Args&&... args) {
        std::string_view format_view = format;
        implementation::log (0, 6, format_view, args...);
    }
}

#endif // TRAM_SDK_LOGGING_H