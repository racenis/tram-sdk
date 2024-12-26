// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_SERIALIZATION_H
#define TRAM_SDK_FRAMEWORK_SERIALIZATION_H

#include <cassert>

#include <framework/uid.h>
#include <framework/value.h>

#include <framework/math.h>

/**
 * Serialization, i.e. saving application state is not implemented yet.
 * 
 * I will probably get around to it sometime.
 */

namespace tram {

/// Wrapper for serialized field values.
/*class SerializedFieldArray {
public:
    SerializedFieldArray(const Value* first, size_t count) : first_field(first), field_count(count) {}
    const Value& operator [](size_t n) const { assert(n < field_count); return first_field[n]; }
private:
    const Value* first_field;
    size_t field_count;
};*/

}

#endif // TRAM_SDK_FRAMEWORK_SERIALIZATION_H
