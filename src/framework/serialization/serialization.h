// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_SERIALIZATION_SERIALIZATION_H
#define TRAM_SDK_FRAMEWORK_SERIALIZATION_SERIALIZATION_H

#include <cassert>

#include <framework/uid.h>
#include <framework/type.h>

#include <framework/math.h>

namespace tram {

class SerializedField {
public:
    //SerializedField() : uint64_value(0) { type = TYPE_UINT64; }
    ~SerializedField() {}
    
    SerializedField(const SerializedField& other) : uint64_value(other.uint64_value) /* hehe */ { type = other.type; }
    
    SerializedField(bool value) : bool_value(value) { type = TYPE_BOOL; }
    SerializedField(name_t value) : name_value(value) { type = TYPE_NAME; }
    SerializedField(const char* value) : string_value(value) { type = TYPE_STRING; }
    
    SerializedField(int8_t value) : int8_value(value) { type = TYPE_INT8; }
    SerializedField(int16_t value) : int16_value(value) { type = TYPE_INT16; }
    SerializedField(int32_t value) : int32_value(value) { type = TYPE_INT32; }
    SerializedField(int64_t value) : int64_value(value) { type = TYPE_INT64; }

    SerializedField(uint8_t value) : uint8_value(value) { type = TYPE_UINT8; }
    SerializedField(uint16_t value) : uint16_value(value) { type = TYPE_UINT16; }
    SerializedField(uint32_t value) : uint32_value(value) { type = TYPE_UINT32; }
    SerializedField(uint64_t value) : uint64_value(value) { type = TYPE_UINT64; }

    SerializedField(float value) : float_value(value) { type = TYPE_FLOAT32; }
    SerializedField(double value) : double_value(value) { type = TYPE_FLOAT64; }

    operator bool() const { assert(type == TYPE_BOOL); return bool_value; } 
    operator name_t() const { assert(type == TYPE_NAME); return name_value; } 
    operator const char*() const { assert(type == TYPE_STRING); return string_value; } 
    
    operator int8_t() const { assert(type == TYPE_INT8); return int8_value; } 
    operator int16_t() const { assert(type == TYPE_INT16); return int16_value; } 
    operator int32_t() const { assert(type == TYPE_INT32); return int32_value; } 
    operator int64_t() const { assert(type == TYPE_INT64); return int64_value; } 
    
    operator uint8_t() const { assert(type == TYPE_UINT8); return uint8_value; } 
    operator uint16_t() const { assert(type == TYPE_UINT16); return uint16_value; } 
    operator uint32_t() const { assert(type == TYPE_UINT32); return uint32_value; } 
    operator uint64_t() const { assert(type == TYPE_UINT64); return uint64_value; } 
    
    operator float() const { assert(type == TYPE_FLOAT32); return float_value; } 
    operator double() const { assert(type == TYPE_FLOAT64); return double_value; } 
private:
    Type type;
    
    union {
        bool bool_value;
        name_t name_value;
        const char* string_value;
        int8_t int8_value;
        int16_t int16_value;
        int32_t int32_value;
        int64_t int64_value;
        uint8_t uint8_value;
        uint16_t uint16_value;
        uint32_t uint32_value;
        uint64_t uint64_value;
        float float_value;
        double double_value;
    };
};

class SerializedFieldArray {
public:
    SerializedFieldArray(const SerializedField* first, size_t count) : first_field(first), field_count(count) {}
    const SerializedField& operator [](size_t n) const { assert(n < field_count); return first_field[n]; }
private:
    const SerializedField* first_field;
    size_t field_count;
};

}

#endif // TRAM_SDK_FRAMEWORK_SERIALIZATION_SERIALIZATION_H
