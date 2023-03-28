// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

// This is the configuration file for the library portion of the framework.
// If you change anything in here, you'll need to recompile everything.

#ifndef TRAM_SDK_CONFIG_H
#define TRAM_SDK_CONFIG_H

namespace tram {

enum Platform {
    PLATFORM_WINDOWS,   //< Windows 7 platform
    PLATFORM_LINUX,     //< Linux Debian platform
    PLATFORM_WEB        //< Emscripten Firefox platform
};

#if defined(_WIN32)
    #define TRAM_SDK_PLATFORM_WINDOWS
    const static Platform CURRENT_PLATFORM = PLATFORM_WINDOWS; 
#elif defined(__unix__)
    #define TRAM_SDK_PLATFORM_LINUX
    const static Platform CURRENT_PLATFORM = PLATFORM_LINUX;
#elif defined(__EMSCRIPTEN__)
    #define TRAM_SDK_PLATFORM_WEB
    const static Platform CURRENT_PLATFORM = PLATFORM_WEB;
#else
    #error Undetectable platform. Please make your platform detectable.
#endif

}

#endif // TRAM_SDK_CONFIG_H