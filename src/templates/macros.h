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


// Asserts

#ifndef NDEBUG
#define TRAM_SDK_ASSERT_SYSTEM_LOADED(SYSTEM) if (!System::IsInitialized(SYSTEM)) { Log ("System {} is not loaded. It should be. Assert in {} at {}", System::GetShortName(SYSTEM), __FILE__, __LINE__); abort(); }
#define TRAM_SDK_ASSERT_SYSTEM_UNLOADED(SYSTEM) if (System::IsInitialized(SYSTEM)) { Log ("System {} is loaded. It shouldn't be. Assert in {} at {}", System::GetShortName(SYSTEM), __FILE__, __LINE__); abort(); }
#else
#define TRAM_SDK_ASSERT_SYSTEM_LOADED(SYSTEM) ;
#define TRAM_SDK_ASSERT_SYSTEM_UNLOADED(SYSTEM) ;
#endif


#endif // TEMPLATES_MACROS_H