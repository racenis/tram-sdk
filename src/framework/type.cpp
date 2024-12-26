// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/type.h>

namespace tram {

/// Converts type enum to string.
const char* TypeToString(Type type) {
    switch (type) {
        case TYPE_UNDEFINED:    return "undefined";
        case TYPE_BOOL:         return "bool";
        case TYPE_NAME:         return "name";
        case TYPE_STRING:       return "string";
        case TYPE_INT8:         return "int8";
        case TYPE_INT16:        return "int16";
        case TYPE_INT32:        return "int32";
        case TYPE_UINT8:        return "uint8";
        case TYPE_UINT16:       return "uint16";
        case TYPE_UINT32:       return "uint32";
        case TYPE_FLOAT32:      return "float32";
        case TYPE_VEC2:         return "vec2";
        case TYPE_VEC3:         return "vec3";
        case TYPE_VEC4:         return "vec4";
        case TYPE_QUAT:         return "quat";
        default:                return "invalid";
    }
}


}