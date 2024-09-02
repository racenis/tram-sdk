// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/language.h>

#include <framework/file.h>

#include <templates/stackpool.h>
#include <templates/hashmap.h>

#include <cstring>
#include <fstream>

namespace tram::Language {

static StackPool<char> language_string_pool ("stringpool for langs", 10000);
static Hashmap<const char*> language_string_map ("Hashmap for language strings", 500);
    
void Load(const char* filename){
    char path [100] = "data/";
    strcat (path, filename);
    strcat (path, ".lang");
    
    File file (path, MODE_READ);
    
    if (!file.is_open()) {
        std::cout << "Can't find language file " << path << std::endl;
    }
    
    while (file.is_continue()) {
        name_t string_name = file.read_name();
        std::string_view string_content = file.read_line();
        
        char* string_ptr = language_string_pool.AddNew(string_content.length() + 1);

        strncpy(string_ptr, string_content.data(), string_content.length());
        string_ptr[string_content.length()] = '\0';

        language_string_map.Insert(string_name, string_ptr);
    }
}

const char* Get(name_t name) {
    const char* string = language_string_map.Find(name);
    
    if (!string) {
        return name;
    } else {
        return string;
    }
}

char UTF16ToLatin7(uint16_t code) {
    switch (code) {
        
        default: return 15;
    }
}

}