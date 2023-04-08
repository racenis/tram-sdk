// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef PLATFORM_PLATFORM_H
#define PLATFORM_PLATFORM_H

namespace tram {

enum Platform {
    PLATFORM_WINDOWS,   //< Windows 7 platform
    PLATFORM_LINUX,     //< Linux Debian platform
    PLATFORM_WEB        //< Emscripten Firefox platform
};

#if defined(__EMSCRIPTEN__)
    #define TRAM_SDK_PLATFORM_WEB
    const static Platform CURRENT_PLATFORM = PLATFORM_WEB;
#elif defined(_WIN32)
    #define TRAM_SDK_PLATFORM_WINDOWS
    const static Platform CURRENT_PLATFORM = PLATFORM_WINDOWS; 
#elif defined(__unix__)
    #define TRAM_SDK_PLATFORM_LINUX
    const static Platform CURRENT_PLATFORM = PLATFORM_LINUX;
#else
    #error Undetectable platform. Please make your platform detectable.
#endif

}

#endif // PLATFORM_PLATFORM_H