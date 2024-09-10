// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_VALUE_H
#define TRAM_SDK_FRAMEWORK_VALUE_H

#include <cassert>

#include <framework/type.h>
#include <framework/uid.h>
#include <framework/math.h>

namespace tram {

class ValuePtr;
    
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
    
    bool operator==(const Value& other) const {
        switch (type) {
            case TYPE_BOOL:
                if (other.GetType() != TYPE_BOOL) return false; else return bool_value == other.bool_value;
            case TYPE_INT:
            case TYPE_UINT:
            case TYPE_INT8:
            case TYPE_INT16:
            case TYPE_INT32:
            case TYPE_INT64:
            case TYPE_UINT8:
            case TYPE_UINT16:
            case TYPE_UINT32:
            case TYPE_UINT64:
                if (!other.IsInt() && !other.IsFloat()) return false; else return GetInt() == other.GetInt();
            case TYPE_FLOAT:
            case TYPE_FLOAT32:
            case TYPE_FLOAT64:
                if (!other.IsInt() && !other.IsFloat()) return false; else return GetFloat() == other.GetFloat();
            case TYPE_NAME:
                if (other.GetType() != TYPE_NAME) return false; else return name_value == other.name_value;
            case TYPE_STRING:
                return false; // TODO: fix this
            case TYPE_VEC2:
                if (other.GetType() != TYPE_VEC2) return false; else return vec2_value == other.vec2_value;
            case TYPE_VEC3:
                if (other.GetType() != TYPE_VEC3) return false; else return vec3_value == other.vec3_value;
            case TYPE_VEC4:
                if (other.GetType() != TYPE_VEC4) return false; else return vec4_value == other.vec4_value;
            case TYPE_QUAT:
                if (other.GetType() != TYPE_QUAT) return false; else return quat_value == other.quat_value;
            default: return false;
        }
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
protected:
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
    
    friend class ValuePtr;
};

class ValuePtr {
public:
    ValuePtr() : uint64_ptr(nullptr) { type = TYPE_UNDEFINED; }
    ~ValuePtr() {}
    
    ValuePtr(const ValuePtr& other) : vec4_ptr(other.vec4_ptr) { type = other.type; }
    
    ValuePtr(Value& other) {
        type = other.GetType();
        
        switch (type) {
            case TYPE_BOOL:     bool_ptr = &other.bool_value; break;
            case TYPE_INT8:     int8_ptr = &other.int8_value; break;
            case TYPE_INT16:    int16_ptr = &other.int16_value; break;
            case TYPE_INT32:    int32_ptr = &other.int32_value; break;
            case TYPE_INT64:    int64_ptr = &other.int64_value; break;
            case TYPE_UINT8:    uint8_ptr = &other.uint8_value; break;
            case TYPE_UINT16:   uint16_ptr = &other.uint16_value; break;
            case TYPE_UINT32:   uint32_ptr = &other.uint32_value; break;
            case TYPE_UINT64:   uint64_ptr = &other.uint64_value; break;
            case TYPE_FLOAT32:  float_ptr = &other.float_value; break;
            case TYPE_FLOAT64:  double_ptr = &other.double_value; break;
            case TYPE_NAME:     name_ptr = &other.name_value; break;
            case TYPE_STRING:   string_ptr = &other.string_value; break;
            case TYPE_VEC2:     vec2_ptr = &other.vec2_value; break;
            case TYPE_VEC3:     vec3_ptr = &other.vec3_value; break;
            case TYPE_VEC4:     vec4_ptr = &other.vec4_value; break;
            case TYPE_QUAT:     quat_ptr = &other.quat_value; break;
            default: assert(false);
        }
    }
    
    operator Value() const {
        switch (type) {
            case TYPE_UNDEFINED: return Value();
            case TYPE_BOOL:     return *bool_ptr;
            //case TYPE_INT:      return *int32_ptr;
            //case TYPE_UINT:     return *uint32_ptr;
            case TYPE_INT8:     return *int8_ptr;
            case TYPE_INT16:    return *int16_ptr;
            case TYPE_INT32:    return *int32_ptr;
            case TYPE_INT64:    return *int64_ptr;
            case TYPE_UINT8:    return *uint8_ptr;
            case TYPE_UINT16:   return *uint16_ptr;
            case TYPE_UINT32:   return *uint32_ptr;
            case TYPE_UINT64:   return *uint64_ptr;
            case TYPE_FLOAT:    return *float_ptr;
            case TYPE_FLOAT32:  return *float_ptr;
            case TYPE_FLOAT64:  return *double_ptr;
            case TYPE_NAME:     return *name_ptr;
            case TYPE_STRING:   return *string_ptr;
            case TYPE_VEC2:     return *vec2_ptr;
            case TYPE_VEC3:     return *vec3_ptr;
            case TYPE_VEC4:     return *vec4_ptr;
            case TYPE_QUAT:     return *quat_ptr;
            default: assert(false); return Value();
        }
    }
    
    void SetValue(const Value& other) {
        assert(type == other.GetType());
        
        switch (type) {
            case TYPE_BOOL:     *bool_ptr = (bool)other; break;
            case TYPE_INT8:     *int8_ptr = (int8_t)other; break;
            case TYPE_INT16:    *int16_ptr = (int16_t)other; break;
            case TYPE_INT32:    *int32_ptr = (int32_t)other; break;
            case TYPE_INT64:    *int64_ptr = (int64_t)other; break;
            case TYPE_UINT8:    *uint8_ptr = (uint8_t)other; break;
            case TYPE_UINT16:   *uint16_ptr = (uint16_t)other; break;
            case TYPE_UINT32:   *uint32_ptr = (uint32_t)other; break;
            case TYPE_UINT64:   *uint64_ptr = (uint64_t)other; break;
            case TYPE_FLOAT:    *float_ptr = (float)other; break;
            case TYPE_FLOAT32:  *float_ptr = (float)other; break;
            case TYPE_FLOAT64:  *double_ptr = (double)other; break;
            case TYPE_NAME:     *name_ptr = (name_t)other; break;
            case TYPE_STRING:   *string_ptr = (const char*)other; break;
            case TYPE_VEC2:     *vec2_ptr = (vec2)other; break;
            case TYPE_VEC3:     *vec3_ptr = (vec3)other; break;
            case TYPE_VEC4:     *vec4_ptr = (vec4)other; break;
            case TYPE_QUAT:     *quat_ptr = (quat)other; break;
            default: assert(false);
        }
    }
    
    ValuePtr(bool* ptr) : bool_ptr(ptr) { type = TYPE_BOOL; }
    ValuePtr(name_t* ptr) : name_ptr(ptr) { type = TYPE_NAME; }
    ValuePtr(const char** ptr) : string_ptr(ptr) { type = TYPE_STRING; }
    
    ValuePtr(int8_t* ptr) : int8_ptr(ptr) { type = TYPE_INT8; }
    ValuePtr(int16_t* ptr) : int16_ptr(ptr) { type = TYPE_INT16; }
    ValuePtr(int32_t* ptr) : int32_ptr(ptr) { type = TYPE_INT32; }
    ValuePtr(int64_t* ptr) : int64_ptr(ptr) { type = TYPE_INT64; }

    ValuePtr(uint8_t* ptr) : uint8_ptr(ptr) { type = TYPE_UINT8; }
    ValuePtr(uint16_t* ptr) : uint16_ptr(ptr) { type = TYPE_UINT16; }
    ValuePtr(uint32_t* ptr) : uint32_ptr(ptr) { type = TYPE_UINT32; }
    ValuePtr(uint64_t* ptr) : uint64_ptr(ptr) { type = TYPE_UINT64; }
    
    ValuePtr(vec2* ptr) : vec2_ptr(ptr) { type = TYPE_VEC2; }
    ValuePtr(vec3* ptr) : vec3_ptr(ptr) { type = TYPE_VEC3; }
    ValuePtr(vec4* ptr) : vec4_ptr(ptr) { type = TYPE_VEC4; }
    
    ValuePtr(quat* ptr) : quat_ptr(ptr) { type = TYPE_QUAT; }

    ValuePtr(float* ptr) : float_ptr(ptr) { type = TYPE_FLOAT32; }
    ValuePtr(double* ptr) : double_ptr(ptr) { type = TYPE_FLOAT64; }
    
    operator bool*() const { assert(type == TYPE_BOOL); return bool_ptr; } 
    operator name_t*() const { assert(type == TYPE_NAME); return name_ptr; }
    operator const char**() const { assert(type == TYPE_STRING); return string_ptr; }
    
    operator int8_t*() const { assert(type == TYPE_INT8); return int8_ptr; }
    operator int16_t*() const { assert(type == TYPE_INT16); return int16_ptr; }
    operator int32_t*() const { assert(type == TYPE_INT32); return int32_ptr; }
    operator int64_t*() const { assert(type == TYPE_INT64); return int64_ptr; }
    
    operator uint8_t*() const { assert(type == TYPE_UINT8); return uint8_ptr; }
    operator uint16_t*() const { assert(type == TYPE_UINT16); return uint16_ptr; }
    operator uint32_t*() const { assert(type == TYPE_UINT32); return uint32_ptr; }
    operator uint64_t*() const { assert(type == TYPE_UINT64); return uint64_ptr; }
    
    operator float*() const { assert(type == TYPE_FLOAT32); return float_ptr; } 
    operator double*() const { assert(type == TYPE_FLOAT64); return double_ptr; }
    
    operator vec2*() const { assert(type == TYPE_VEC2); return vec2_ptr; }
    operator vec3*() const { assert(type == TYPE_VEC3); return vec3_ptr; }
    operator vec4*() const { assert(type == TYPE_VEC4); return vec4_ptr; }
    
    operator quat*() const { assert(type == TYPE_QUAT); return quat_ptr; }
    
    inline bool IsBool() const { return type == TYPE_BOOL; }
    inline bool IsInt() const { return type >= TYPE_INT8 && type <= TYPE_UINT64; }
    inline bool IsFloat() const { return type == TYPE_FLOAT32 || type == TYPE_FLOAT64; }
    
    inline Type GetType() const { return type; }
private:
    Type type;
    
    union {
        bool* bool_ptr;
        name_t* name_ptr;
        const char** string_ptr;
        int8_t* int8_ptr;
        int16_t* int16_ptr;
        int32_t* int32_ptr;
        int64_t* int64_ptr;
        uint8_t* uint8_ptr;
        uint16_t* uint16_ptr;
        uint32_t* uint32_ptr;
        uint64_t* uint64_ptr;
        float* float_ptr;
        double* double_ptr;
        vec2* vec2_ptr;
        vec3* vec3_ptr;
        vec4* vec4_ptr;
        quat* quat_ptr;
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