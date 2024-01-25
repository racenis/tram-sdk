// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_VALUE_H
#define TRAM_SDK_FRAMEWORK_VALUE_H

#include <cassert>

#include <framework/type.h>
#include <framework/uid.h>
#include <framework/math.h>

namespace tram {

/// Generic, type-safe union.
class Value {
public:
    Value() : uint64_value(0) { type = TYPE_UNDEFINED; }
    ~Value() {}
    
    Value(const Value& other) : vec4_value(other.vec4_value) /* hehe */ { type = other.type; }
    
    Value& operator=(const Value& other) {
        vec4_value = other.vec4_value;
        type = other.type; return *this;
    }
    
    Value(bool value) : bool_value(value) { type = TYPE_BOOL; }
    Value(name_t value) : name_value(value) { type = TYPE_NAME; }
    Value(const char* value) : string_value(value) { type = TYPE_STRING; }
    
    Value(int8_t value) : int8_value(value) { type = TYPE_INT8; }
    Value(int16_t value) : int16_value(value) { type = TYPE_INT16; }
    Value(int32_t value) : int32_value(value) { type = TYPE_INT32; }
    Value(int64_t value) : int64_value(value) { type = TYPE_INT64; }

    Value(uint8_t value) : uint8_value(value) { type = TYPE_UINT8; }
    Value(uint16_t value) : uint16_value(value) { type = TYPE_UINT16; }
    Value(uint32_t value) : uint32_value(value) { type = TYPE_UINT32; }
    Value(uint64_t value) : uint64_value(value) { type = TYPE_UINT64; }
    
    Value(vec2 value) : vec2_value(value) { type = TYPE_VEC2; }
    Value(vec3 value) : vec3_value(value) { type = TYPE_VEC3; }
    Value(vec4 value) : vec4_value(value) { type = TYPE_VEC4; }
    
    Value(quat value) : quat_value(value) { type = TYPE_QUAT; }

    Value(float value) : float_value(value) { type = TYPE_FLOAT32; }
    Value(double value) : double_value(value) { type = TYPE_FLOAT64; }
    
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
    
    operator vec2() const { assert(type == TYPE_VEC2); return vec2_value; }
    operator vec3() const { assert(type == TYPE_VEC3); return vec3_value; }
    operator vec4() const { assert(type == TYPE_VEC4); return vec4_value; }
    
    operator quat() const { assert(type == TYPE_QUAT); return quat_value; }
    
    
    
    inline bool IsBool() const { return type == TYPE_BOOL; }
    inline bool IsInt() const { return type >= TYPE_INT8 && type <= TYPE_UINT64; }
    inline bool IsFloat() const { return type == TYPE_FLOAT32 || type == TYPE_FLOAT64; }
    
    inline int GetInt() const {
        switch (type) {
            case TYPE_INT8:     return int8_value;
            case TYPE_INT16:    return int16_value;
            case TYPE_INT32:    return int32_value;
            case TYPE_INT64:    return int64_value;
            case TYPE_UINT8:    return int8_value;
            case TYPE_UINT16:   return int16_value;
            case TYPE_UINT32:   return int32_value;
            case TYPE_UINT64:   return int64_value;
            case TYPE_FLOAT32:  return float_value;
            case TYPE_FLOAT64:  return double_value;
            default: return 0;
        }
    }
    
    inline float GetFloat() const {
        switch (type) {
            case TYPE_INT8:     return int8_value;
            case TYPE_INT16:    return int16_value;
            case TYPE_INT32:    return int32_value;
            case TYPE_INT64:    return int64_value;
            case TYPE_UINT8:    return int8_value;
            case TYPE_UINT16:   return int16_value;
            case TYPE_UINT32:   return int32_value;
            case TYPE_UINT64:   return int64_value;
            case TYPE_FLOAT32:  return float_value;
            case TYPE_FLOAT64:  return double_value;
            default: return 0;
        }
    }
    
    
    
    inline Type GetType() const { return type; }
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
        vec2 vec2_value;
        vec3 vec3_value;
        vec4 vec4_value;
        quat quat_value;
    };
};

class ValueArray {
public:
    ValueArray(const Value* first, size_t count) : first_field(first), field_count(count) {}
    const Value& operator [](size_t n) const { assert(n < field_count); return first_field[n]; }
private:
    const Value* first_field;
    size_t field_count;
};

typedef Value value_t;
typedef ValueArray valuearray_t;

}

#endif // TRAM_SDK_FRAMEWORK_VALUE_H