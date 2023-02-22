#include <framework/language.h>

#include <templates/stackpool.h>

#include <framework/templates.h>
#include <cstring>

#include <fstream>
#include <unordered_map>

namespace Core::Language {
    StackPool<char> stringPoolforLangs("stringpool for langs", 10000);
    std::unordered_map<uint64_t, uint64_t> langStringHashMap;
        
    // when refactoring this, it would be good idea to move these out to a language.cpp or something
    void Load (const char* filename){
        std::ifstream file;
        file.open(filename);
        while(file){
            std::string strid;
            std::string langstr;

            file >> strid;
            file.ignore(1);
            std::getline(file, langstr);

            name_t strkey = UID(strid);
            uint64_t langkey = stringPoolforLangs.GetSize();

            char* tbr = stringPoolforLangs.AddNew(langstr.length() + 1);

            strcpy(tbr, langstr.c_str());

            langStringHashMap[strkey.key] = langkey;

        }
        file.close();
    }
    
    name_t Get (name_t name){
        std::unordered_map<uint64_t, uint64_t>::iterator ff = langStringHashMap.find(name.key);

        if(ff == langStringHashMap.end()){
            return 0;
        } else {
            return ff->second;
        }
    }
}