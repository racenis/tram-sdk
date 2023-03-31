// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_SERIALIZEDDATA_H
#define TRAM_SDK_FRAMEWORK_SERIALIZEDDATA_H

#include <cassert>
#include <vector>

#include <framework/uid.h>

namespace tram {
    /// Tightly packed, binary entity data.
    class SerializedEntityData {
    public:
        /// Segment of binary entity data.
        template <typename T>
        class Field {
        public:
            operator T() { return var; }
            Field& operator= (const T& other) { var = other; return *this;}
            Field(const T& other) { var = other; }
            Field() = default;

            /// Converts a field to a string.
            /// Used for serialization to disk.
            void ToString(std::string& str) { assert(false); }
            
            /// Converts a field to binary.
            /// Used for deserialization from disk.
            T& FromString(std::string_view& str) { assert(false); }
        private:
            T var;
        };
        
        /// Field meta-data for the editor program.
        struct FieldInfo {
            /// Determines representation and semantics of a field.
            /// See framework/type.h for more information.
            enum Type {
                FIELD_INT,      //< Default integer.
                FIELD_UINT,     //< Default unsigned integer.
                FIELD_FLOAT,    //< Default floating point.
                FIELD_STRING,   //< Default string.
                FIELD_MODELNAME //< Default string that represents the name of a Model.
            } type;
            
            /// Name of the field.
            char const* name;
        };
        
        /// Converts all of the fields to a string.
        /// Used for serialization to disk.
        virtual void ToString(std::string& str) = 0;
        
        /// Converts all of the fields to binary.
        /// Used for deserialization from disk.
        virtual void FromString(std::string_view& str) = 0;
        
        /// Returns field meta-data for the editor.
        virtual std::vector<FieldInfo> GetFieldInfo() = 0;
        
        /// Returns the name of the Entity type.
        virtual char const* GetType() = 0;
        
        SerializedEntityData() = default;
        virtual ~SerializedEntityData() = default;
    };
    
    /// Smart pointer to a SerializedEntityData.
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

    // INT TYPE
    template <>
    inline int64_t& SerializedEntityData::Field<int64_t>::FromString(std::string_view &str) {
        char* f_end;
        var = std::strtod(str.data(), &f_end);
        str.remove_prefix(f_end - str.data());
        return var;
    }
    
    template <>
    inline void SerializedEntityData::Field<int64_t>::ToString(std::string& str) {
        str += " "; str += std::to_string(var);
    }

    // UINT TYPE
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

#endif // TRAM_SDK_FRAMEWORK_SERIALIZEDDATA_H
