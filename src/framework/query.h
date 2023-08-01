// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_QUERY_H
#define TRAM_SDK_FRAMEWORK_QUERY_H

#include <cstdint>
#include <cstddef>

namespace tram {

typedef uint32_t query_t;

enum Query : query_t {
    QUERY_PICKUPABLE,
    QUERY_LOOK_DIRECTION,
    QUERY_BOUNDING_BOX_MIN,
    QUERY_BOUNDING_BOX_MAX,
    LAST_QUERY
};

}

#endif // TRAM_SDK_FRAMEWORK_QUERY_H