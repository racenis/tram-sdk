// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_TYPES_H
#define TRAM_SDK_FRAMEWORK_TYPES_H

namespace tram {
    
/// Type enumeration for serialization, etc.

enum Type {
    TYPE_BOOL,      //< Bool. Defaults to C++ bool type.
    TYPE_INT,       //< Generic signed integer. Defaults to a int32_t.
    TYPE_UINT,      //< Generic unsigned integer. Defaults to a uint32_t.
    TYPE_FLOAT,     //< Generic floating-point. Defaults to C++ float type.
    TYPE_STRING,    //< Generic string. Defaults to a char array.
    TYPE_INT8,      //< Signed 8 bit long integer.
    TYPE_INT16,     //< Signed 16 bit long integer.
    TYPE_INT32,     //< Signed 32 bit long integer.
    TYPE_INT64,     //< Signed 64 bit long integer.
    TYPE_UINT8,     //< Unigned 8 bit long integer.
    TYPE_UINT16,    //< Unigned 16 bit long integer.
    TYPE_UINT32,    //< Unigned 32 bit long integer.
    TYPE_UINT64,    //< Unigned 64 bit long integer.
    TYPE_FLOAT32,   //< Normal precision floating-point.
    TYPE_FLOAT64    //< Double precision floating-point.
};

}

#endif // TRAM_SDK_FRAMEWORK_TYPES_H