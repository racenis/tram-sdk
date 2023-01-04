// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// LOGGING.H -- Logging function

#ifndef LOGGING_H
#define LOGGING_H

#include <framework/core.h>
#include <framework/uid.h>
#include <concepts>

namespace Core {
    namespace implementation {
        template <typename T> void concat (const T& value) {
            concat<const char*> ("UNDEFINED");
        }
        
        template <> void concat (const std::string_view& value);
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
        void flush (int system);
        
        
        inline void log (int system, std::string_view& format) {
            concat_fmt(format);
            flush(system);
        }

        template <typename T, typename... Args>
        void log (int system, std::string_view& format, T value, Args&&... args) {
            concat_fmt(format);
            concat<T>(value);
            
            log(system, format, args...);
        }
    }
    
    template <typename... Args>
    void Log (int system, const std::string_view& format, Args&&... args) {
        std::string_view format_view = format;
        implementation::log (system, format_view, args...);
    }
    
    template <typename... Args>
    void Log (const std::string_view& format, Args&&... args) {
        std::string_view format_view = format;
        implementation::log (0, format_view, args...);
    }
}

#endif // LOGGING_H