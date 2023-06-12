// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_QUERY_H
#define TRAM_SDK_FRAMEWORK_QUERY_H

#include <cstdint>
#include <cstddef>

namespace tram {

typedef uint32_t query_t;

enum class Query : query_t {
    PICKUPABLE,
    LAST_QUERY
};

}

#endif // TRAM_SDK_FRAMEWORK_QUERY_H