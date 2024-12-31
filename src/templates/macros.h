// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


// This file contains some macros. All of them are completely optional.

#ifndef TRAM_SDK_TEMPLATES_MACROS_H
#define TRAM_SDK_TEMPLATES_MACROS_H

/// This is experimental template.
#define TRAM_SDK_REGISTER_EVENT_DATATYPE(EVENT_ID, TYPE) template <> struct tram::EventDataType<EVENT_ID> { TYPE type; };

/// Implements a pool.
#define TRAM_SDK_IMPLEMENT_POOL(TYPE, NAME, SIZE) template <> Pool<TYPE> PoolProxy<TYPE>::pool (NAME, SIZE);


// Asserts

#ifndef NDEBUG
#define TRAM_SDK_ASSERT(EXPRESSION) if (!(EXPRESSION)) { Log("{}\nAssert at line {} in {}", #EXPRESSION, __LINE__, __FILE__); abort(); }
#define TRAM_SDK_ASSERT_MESSAGE(EXPRESSION, MESSAGE) if (!(EXPRESSION)) { Log("{}\n{}\nAssert at line {} in {}", MESSAGE, #EXPRESSION, __LINE__, __FILE__); abort(); }
#else
#define TRAM_SDK_ASSERT(EXPRESSION) ;
#define TRAM_SDK_ASSERT_MESSAGE(EXPRESSION, MESSAGE) ;
#endif


#endif // TRAM_SDK_TEMPLATES_MACROS_H