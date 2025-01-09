// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_TYPES_H
#define TRAM_SDK_FRAMEWORK_TYPES_H

#include <cstdint>

namespace tram {
    
/// Type enumeration for serialization, etc.

enum Type : uint32_t {
    TYPE_UNDEFINED, //< When you don't know what the type is.
    TYPE_BOOL,      //< Bool. Defaults to C++ bool type.
    TYPE_NAME,      //< Our own name_t hashtable string.
    TYPE_STRING,    //< Generic string. Defaults to a char array.
    TYPE_INT8,      //< Signed 8 bit long integer.
    TYPE_INT16,     //< Signed 16 bit long integer.
    TYPE_INT32,     //< Signed 32 bit long integer.
    TYPE_UINT8,     //< Unsigned 8 bit long integer.
    TYPE_UINT16,    //< Unsigned 16 bit long integer.
    TYPE_UINT32,    //< Unsigned 32 bit long integer.
    TYPE_FLOAT32,   //< Normal precision floating-point.
    TYPE_VEC2,      //< Normal precision floating-point 2 element vector.
    TYPE_VEC3,      //< Normal precision floating-point 3 element vector.
    TYPE_VEC4,      //< Normal precision floating-point 4 element vector.
    TYPE_QUAT,      //< Normal precision floating-point quaternion.
};

const char* TypeToString(Type type);

}

#endif // TRAM_SDK_FRAMEWORK_TYPES_H