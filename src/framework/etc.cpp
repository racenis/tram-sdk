#include <framework/etc.h>

#include <templates.h>
#include <cstring>

// TODO: yeet these
#include <fstream>
#include <unordered_map>

namespace Core {
    StackPool<char> stringPoolforLangs("stringpool for langs", 10000);
    std::unordered_map<uint64_t, uint64_t> langStringHashMap;
    
    uint64_t GenerateID(){
        static uint64_t num = 0;
        num++;
        return num;
    }
    
    // when refactoring this, it would be good idea to move these out to a language.cpp or something
    void LoadText(const char* filename){
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
    
    name_t FindLangStr(name_t name){
        std::unordered_map<uint64_t, uint64_t>::iterator ff = langStringHashMap.find(name.key);

        if(ff == langStringHashMap.end()){
            return 0;
        } else {
            return ff->second;
        }
    }
}