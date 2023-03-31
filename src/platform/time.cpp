#include <platform/time.h>

#ifndef ENGINE_EDITOR_MODE
    #ifdef __EMSCRIPTEN__
        #include <GLFW/glfw3.h>
    #else
        #include <glfw3.h>
    #endif
#endif

namespace tram {

double GetTime() {
#ifndef ENGINE_EDITOR_MODE
    return glfwGetTime();
#else
    return 0.0f;
#endif
}
    
}