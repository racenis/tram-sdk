#include <framework/logging.h>
#include <cstring>
#include <charconv>
#include <iostream>

namespace Core::implementation {
    char buffer[500] = {'\0'}; // yes.. haha .. YES!!
    
    void concat_fmt (std::string_view& str) {
        size_t open_bracket = str.find_first_of('{');
        size_t close_bracket = str.find_first_of('}', open_bracket);
        
        // no options for specifying what's inside the bracket
        // ... for now.
        
        strncat(buffer, str.data(), open_bracket);
        
        str.remove_prefix(close_bracket + 1);
    }
    
    void flush (int system) {
        std::cout << "[" << system << "] " << buffer << std::endl;
        buffer[0] = '\0';
    }
    
    template <> void concat (const std::string_view& value) {
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