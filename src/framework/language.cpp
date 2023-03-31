// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/language.h>

#include <templates/stackpool.h>
#include <templates/hashmap.h>

#include <cstring>
#include <fstream>

namespace tram::Language {

static StackPool<char> language_string_pool ("stringpool for langs", 10000);
static Hashmap<const char*> language_string_map ("Hashmap for language strings", 500);
    
void Load (const char* filename){
    std::ifstream file;
    file.open (filename);
    
    while (file) {
        std::string string_name;
        std::string string_content;

        file >> string_name;
        file.ignore(1);
        
        std::getline(file, string_content);

        char* string_ptr = language_string_pool.AddNew(string_content.length() + 1);

        strcpy(string_ptr, string_content.c_str());

        language_string_map.Insert(string_name, string_ptr);
    }
    
    file.close();
}

const char* Get (name_t name){
    return language_string_map.Find(name);
}

}