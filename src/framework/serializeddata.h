// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// SERIALIZEDDATA.H -- Entity and other stuff serialization stuff.

#ifndef SERIALIZEDDATA_H
#define SERIALIZEDDATA_H

#include <cassert>
#include <vector>

#include <framework/uid.h>

namespace Core {
    class SerializedEntityData {
    public:
        template <typename T>
        class Field {
        public:
            operator T() { return var; }
            Field& operator= (const T& other) { var = other; return *this;}
            Field(const T& other) { var = other; }
            Field() = default;

            void ToString(std::string& str) { assert(false); }
            T& FromString(std::string_view& str) { assert(false); }
        private:
            T var;
        };
        struct FieldInfo {
            enum Type {
                FIELD_UINT64,
                FIELD_FLOAT,
                FIELD_NAME
            } type;
            char const* display_name;
            char const* data_name;
            void* field;
        };
        SerializedEntityData() = default;
        virtual ~SerializedEntityData() = default;
        virtual void ToString(std::string& str) = 0;
        virtual void FromString(std::string_view& str) = 0;
        virtual name_t GetEditorModel() = 0;
        virtual std::vector<FieldInfo> GetEditorFieldInfo() = 0;
        virtual char const* GetEditorName() = 0;
        virtual char const* GetDataName() = 0;
    };
    
    template <typename T>
    class SerializedData {
    public:
        SerializedData() {}
        ~SerializedData() { if (ptr) delete ptr; }
        void make() { if (ptr) delete ptr; ptr = new T; }
        void clear() { delete ptr; ptr = nullptr; }
        T* get() { return ptr; }
        T* operator->() { return ptr; }
        T& operator*() { return ptr; }
        explicit operator bool() { return ptr != nullptr; }
    protected:
        T* ptr = nullptr;
    };

    // UNSIGNED INT TYPE
    /*template <>
    class SerializedEntityData::Field<uint64_t> {
    public:
        operator uint64_t() { return var; }
        Field& operator= (const uint64_t& other) { var = other; return *this;}
        Field(uint64_t& other) { var = other; }
        Field() = default;

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

        void ToString(std::string& str) { str += " "; str += std::to_string(var); }

        void ToStringAsName(std::string& str) { str += " "; str += ReverseUID(var); }
    private:
        uint64_t var;
    };*/

    template <>
    inline uint64_t& SerializedEntityData::Field<uint64_t>::FromString(std::string_view &str) {
        char* f_end;
        var = std::strtod(str.data(), &f_end);
        str.remove_prefix(f_end - str.data());
        return var;
    }
    
    template <>
    inline void SerializedEntityData::Field<uint64_t>::ToString(std::string& str) {
        str += " "; str += std::to_string(var);
    }

    // UID TYPE
    template <>
    inline name_t& SerializedEntityData::Field<name_t>::FromString(std::string_view &str) {
        size_t uid_start = str.find_first_not_of(" \t");
        size_t uid_end = str.find_first_of(" \t", uid_start);

        std::string_view uid_str = str.substr(uid_start, uid_end - uid_start);
        var = UID(std::string(uid_str));

        str.remove_prefix(uid_end);
        return var;
    }
    
    template <>
    inline void SerializedEntityData::Field<name_t>::ToString(std::string& str) {
        str += " "; str += std::string(var);
    }

    // FLOAT TYPE
    template <>
    inline float& SerializedEntityData::Field<float>::FromString(std::string_view &str) {
        char* f_end;
        var = std::strtof(str.data(), &f_end);
        str.remove_prefix(f_end - str.data());
        return var;
    }
    
    template <>
    inline void SerializedEntityData::Field<float>::ToString(std::string& str) {
        str += " "; str += std::to_string(var);
    }

}

#endif //SERIALIZEDDATA_H
