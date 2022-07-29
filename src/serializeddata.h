// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// SERIALIZEDDATA.H -- Entity and other stuff serialization stuff.

#ifndef SERIALIZEDDATA_H
#define SERIALIZEDDATA_H

#include "core.h"
#include <string>

namespace Core {
    class SerializedEntityData {
    public:
        SerializedEntityData() = default;
        virtual ~SerializedEntityData() = default;
        virtual void ToString(std::string& str) = 0;
        virtual void FromString(std::string_view& str) = 0;
        template <typename T>
        class Field {
        public:
            operator T() { return var; }
            Field& operator= (const T& other) { var = other; return *this;}

            void ToString(std::string& str) { assert(false); }
            T& FromString(std::string_view& str) { assert(false); }
        private:
            T var;
        };
    };

    // UNSIGNED INT TYPE
    template <>
    class SerializedEntityData::Field<uint64_t> {
    public:
        operator uint64_t() { return var; }
        Field& operator= (const uint64_t& other) { var = other; return *this;}

        uint64_t& FromString(std::string_view& str){
            char* f_end;
            var = std::strtof(str.data(), &f_end);
            str.remove_prefix(f_end - str.data());
            return var;
        }

        name_t& FromStringAsName(std::string_view& str){
            size_t uid_start = str.find_first_not_of(" \t");
            size_t uid_end = str.find_first_of(" \t", uid_start);

            std::string_view uid_str = str.substr(uid_start, uid_end - uid_start);
            var = UID(uid_str, uid_str.size());

            str.remove_prefix(uid_end);
            return var;
        }

        void ToString(std::string& str) { assert(false); }

        void ToStringAsName(std::string& str) { assert(false); }
    private:
        uint64_t var;
    };

    // FLOAT TYPE
    template <>
    inline float& SerializedEntityData::Field<float>::FromString(std::string_view &str) {
        char* f_end;
        var = std::strtof(str.data(), &f_end);
        str.remove_prefix(f_end - str.data());
        return var;
    }

}

#endif //SERIALIZEDDATA_H
