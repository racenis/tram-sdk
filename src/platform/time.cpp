#include <platform/time.h>

#ifndef ENGINE_EDITOR_MODE
    #ifdef __EMSCRIPTEN__
        #include <GLFW/glfw3.h>
    #else
        #include <glfw3.h>
    #endif
#endif

namespace tram {

/// Retrieves the current time.
/// Returns the time, in seconds, since the program was started.
/// @note   If you want to use this time for game logic, then it might be better
///         to use GetTickTime() function.
double GetTime() {
#ifndef ENGINE_EDITOR_MODE
    return glfwGetTime();
#else
    return 0.0f;
#endif
}
    
}