// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_VALUE_H
#define TRAM_SDK_FRAMEWORK_VALUE_H

#include <cassert>
#include <cstring>

#include <framework/type.h>
#include <framework/uid.h>
#include <framework/math.h>
#include <framework/logging.h>

namespace tram {

class ValuePtr;
    
/// Generic, type-safe union.
class Value {
public:
    Value() : vec4_value({0, 0, 0, 0}) { type = TYPE_UNDEFINED; }
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
            case TYPE_INT32:
            case TYPE_UINT32:
                if (!other.IsInt() && !other.IsFloat()) return false; else return GetInt() == other.GetInt();
            case TYPE_FLOAT32:
                if (!other.IsInt() && !other.IsFloat()) return false; else return GetFloat() == other.GetFloat();
            case TYPE_NAME:
                if (other.GetType() != TYPE_NAME) return false; else return name_value == other.name_value;
            case TYPE_STRING:
                if (other.IsString()) return strcmp(string_value, other.string_value) == 0; else if (other.IsName()) return string_value == other.name_value; else return false;
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
    
    Value(int8_t value) : int32_value(value) { type = TYPE_INT32; }
    Value(int16_t value) : int32_value(value) { type = TYPE_INT32; }
    Value(int32_t value) : int32_value(value) { type = TYPE_INT32; }
    
    Value(uint8_t value) : uint32_value(value) { type = TYPE_UINT32; }
    Value(uint16_t value) : uint32_value(value) { type = TYPE_UINT32; }
    Value(uint32_t value) : uint32_value(value) { type = TYPE_UINT32; }
    
    Value(float value) : float_value(value) { type = TYPE_FLOAT32; }
    
    Value(vec2 value) : vec2_value(value) { type = TYPE_VEC2; }
    Value(vec3 value) : vec3_value(value) { type = TYPE_VEC3; }
    Value(vec4 value) : vec4_value(value) { type = TYPE_VEC4; }
    
    Value(quat value) : quat_value(value) { type = TYPE_QUAT; }
    
    operator bool() const {
        switch (type) {
            case TYPE_UNDEFINED:    return false;
            case TYPE_BOOL:         return bool_value;
            case TYPE_INT32:        return int32_value;
            case TYPE_UINT32:       return uint32_value;
            case TYPE_FLOAT32:      return float_value != 0.0f;
            case TYPE_NAME:         return name_value;
            case TYPE_STRING:       return string_value;
            case TYPE_VEC2:         return vec2_value != vec2(0.0f, 0.0f);
            case TYPE_VEC3:         return vec3_value != vec3(0.0f, 0.0f, 0.0f);
            case TYPE_VEC4:         return vec4_value != vec4(0.0f, 0.0f, 0.0f, 0.0f);
            case TYPE_QUAT:         return quat_value != quat(1.0f, 0.0f, 0.0f, 0.0f);
            default:                return false;
        }
    }
    
    operator name_t() const {
        if (type == TYPE_STRING) {
            return (name_t)string_value;
        }
        
        AssertType(TYPE_NAME);
        
        return name_value;
    }
    operator const char*() const {
        if (type == TYPE_NAME) {
            return name_value;
        }
        
        AssertType(TYPE_STRING);
        
        return string_value;
    }
    
    operator int8_t() const { return (int32_t)*this; }
    operator int16_t() const { return (int32_t)*this; }
    operator int32_t() const { AssertType(TYPE_INT32); return int32_value; }
    
    operator uint8_t() const { return (uint32_t)*this; }
    operator uint16_t() const { return (uint32_t)*this; }
    operator uint32_t() const { AssertType(TYPE_UINT32); return uint32_value; }
    
    operator float() const { AssertType(TYPE_FLOAT32); return float_value; } 
    
    operator vec2() const { AssertType(TYPE_VEC2); return vec2_value; }
    operator vec3() const { AssertType(TYPE_VEC3); return vec3_value; }
    operator vec4() const { AssertType(TYPE_VEC4); return vec4_value; }
    
    operator quat() const { AssertType(TYPE_QUAT); return quat_value; }
    
    inline bool IsBool() const { return type == TYPE_BOOL; }
    inline bool IsInt() const { return type == TYPE_INT32 || type == TYPE_UINT32; }
    inline bool IsFloat() const { return type == TYPE_FLOAT32; }
    inline bool IsName() const { return type == TYPE_NAME; }
    inline bool IsString() const { return type == TYPE_STRING; }
    inline bool IsVec2() const { return type == TYPE_VEC2; }
    inline bool IsVec3() const { return type == TYPE_VEC3; }
    inline bool IsVec4() const { return type == TYPE_VEC4; }
    inline bool IsQuat() const { return type == TYPE_QUAT; }
    
    inline int GetInt() const {
        switch (type) {
            case TYPE_INT32:    return int32_value;
            case TYPE_UINT32:   return int32_value;
            case TYPE_FLOAT32:  return float_value;
            default: return 0;
        }
    }
    
    inline float GetFloat() const {
        switch (type) {
            case TYPE_INT32:    return int32_value;
            case TYPE_UINT32:   return uint32_value;
            case TYPE_FLOAT32:  return float_value;
            default: return 0;
        }
    }
    
    inline std::string ToString() {
        switch (type) {
            case TYPE_UNDEFINED:    return "undefined";
            case TYPE_BOOL:         return bool_value ? "true" : "false";
            case TYPE_INT32:        return std::to_string(int32_value);
            case TYPE_UINT32:       return std::to_string(uint32_value);
            case TYPE_FLOAT32:      return std::to_string(float_value);
            case TYPE_NAME:         return name_value;
            case TYPE_STRING:       return string_value;
            case TYPE_VEC2:         return std::to_string(vec2_value.x) + ";" + std::to_string(vec2_value.y);
            case TYPE_VEC3:         return std::to_string(vec3_value.x) + ";" + std::to_string(vec3_value.y) + ";" + std::to_string(vec3_value.z);
            case TYPE_VEC4:         return std::to_string(vec4_value.x) + ";" + std::to_string(vec4_value.y) + ";" + std::to_string(vec4_value.z) + ";" + std::to_string(vec4_value.w);
            case TYPE_QUAT:         return std::to_string(quat_value.x) + ";" + std::to_string(quat_value.y) + ";" + std::to_string(quat_value.z) + ";" + std::to_string(quat_value.w);
            default:                return "unknown";
        }
    }
    
    inline Type GetType() const { return type; }
protected:
    Type type;
    
    void AssertType(Type type) const {
        if (this->type != type) {
            Log(Severity::CRITICAL_ERROR, System::CORE, "Value of type {} used in a {} context", TypeToString(this->type), TypeToString(type));
        }
    }
    
    union {
        bool bool_value;
        name_t name_value;
        const char* string_value;
        int32_t int32_value;
        uint32_t uint32_value;
        float float_value;
        vec2 vec2_value;
        vec3 vec3_value;
        vec4 vec4_value;
        quat quat_value;
    };
    
    friend class ValuePtr;
};

class ValuePtr {
public:
    ValuePtr() : uint32_ptr(nullptr) { type = TYPE_UNDEFINED; }
    ~ValuePtr() {}
    
    ValuePtr(const ValuePtr& other) : vec4_ptr(other.vec4_ptr) { type = other.type; }
    
    ValuePtr(Value& other) {
        type = other.GetType();
        
        switch (type) {
            case TYPE_BOOL:     bool_ptr = &other.bool_value; break;
            case TYPE_INT32:    int32_ptr = &other.int32_value; break;
            case TYPE_UINT32:   uint32_ptr = &other.uint32_value; break;
            case TYPE_FLOAT32:  float_ptr = &other.float_value; break;
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
            case TYPE_INT32:    return *int32_ptr;
            case TYPE_UINT32:   return *uint32_ptr;
            case TYPE_FLOAT32:  return *float_ptr;
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
            case TYPE_INT32:    *int32_ptr = (int32_t)other; break;
            case TYPE_UINT32:   *uint32_ptr = (uint32_t)other; break;
            case TYPE_FLOAT32:  *float_ptr = (float)other; break;
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
    
    ValuePtr(int32_t* ptr) : int32_ptr(ptr) { type = TYPE_INT32; }

    ValuePtr(uint32_t* ptr) : uint32_ptr(ptr) { type = TYPE_UINT32; }

    ValuePtr(vec2* ptr) : vec2_ptr(ptr) { type = TYPE_VEC2; }
    ValuePtr(vec3* ptr) : vec3_ptr(ptr) { type = TYPE_VEC3; }
    ValuePtr(vec4* ptr) : vec4_ptr(ptr) { type = TYPE_VEC4; }
    
    ValuePtr(quat* ptr) : quat_ptr(ptr) { type = TYPE_QUAT; }

    ValuePtr(float* ptr) : float_ptr(ptr) { type = TYPE_FLOAT32; }

    operator bool*() const { assert(type == TYPE_BOOL); return bool_ptr; } 
    operator name_t*() const { assert(type == TYPE_NAME); return name_ptr; }
    operator const char**() const { assert(type == TYPE_STRING); return string_ptr; }
    
    operator int32_t*() const { assert(type == TYPE_INT32); return int32_ptr; }

    operator uint32_t*() const { assert(type == TYPE_UINT32); return uint32_ptr; }
    
    operator float*() const { assert(type == TYPE_FLOAT32); return float_ptr; } 

    operator vec2*() const { assert(type == TYPE_VEC2); return vec2_ptr; }
    operator vec3*() const { assert(type == TYPE_VEC3); return vec3_ptr; }
    operator vec4*() const { assert(type == TYPE_VEC4); return vec4_ptr; }
    
    operator quat*() const { assert(type == TYPE_QUAT); return quat_ptr; }
    
    inline bool IsBool() const { return type == TYPE_BOOL; }
    inline bool IsInt() const { return type == TYPE_INT32 || type == TYPE_UINT32; }
    inline bool IsFloat() const { return type == TYPE_FLOAT32; }
    
    inline Type GetType() const { return type; }
private:
    Type type;
    
    union {
        bool* bool_ptr;
        name_t* name_ptr;
        const char** string_ptr;
        int32_t* int32_ptr;
        uint32_t* uint32_ptr;
        float* float_ptr;
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
    const size_t size() const { return field_count; }
private:
    const Value* first_field;
    size_t field_count;
};

typedef Value value_t;
typedef ValueArray valuearray_t;

}

#endif // TRAM_SDK_FRAMEWORK_VALUE_H