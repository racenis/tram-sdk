// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

// This file contains some macros. All of them are completely optional.

#ifndef TEMPLATES_MACROS_H
#define TEMPLATES_MACROS_H

/// This is experimental template.
#define TRAM_SDK_REGISTER_EVENT_DATATYPE(EVENT_ID, TYPE) template <> struct Core::EventDataType<EVENT_ID> { TYPE type; };

/// This will make your type have a pool to be accessable from PoolProxy.
#define TRAM_SDK_IMPLEMENT_POOLPROXY_POOL(TYPE, NAME, SIZE) template <> Pool<TYPE> PoolProxy<TYPE>::pool (NAME, SIZE);

#endif // TEMPLATES_MACROS_H