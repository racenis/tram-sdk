// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

// This file contains some macros. All of them are completely optional.

#ifndef TEMPLATES_MACROS_H
#define TEMPLATES_MACROS_H

/// This is experimental template.
#define TRAM_SDK_REGISTER_EVENT_DATATYPE(EVENT_ID, TYPE) template <> struct Core::EventDataType<EVENT_ID> { TYPE type; };

/// Implements a pool.
#define TRAM_SDK_IMPLEMENT_POOL(TYPE, NAME, SIZE) template <> Pool<TYPE> PoolProxy<TYPE>::pool (NAME, SIZE);

/// Registers an entity.
#define TRAM_SDK_REGISTER_ENTITY(NAME, TYPE) Entity::Register(#NAME, [](std::string_view& params) -> Entity* {return new TYPE(params);});

#endif // TEMPLATES_MACROS_H