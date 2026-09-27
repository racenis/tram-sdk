// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <platform/other.h>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif

namespace tram::Platform {

/// Attempts to break into debugger.
/// This function will try to determine if the program is being debugged, and if
/// it is, then it will break into debugger. If it isn't being debugged, it will
/// do nothing.
void TryDebugging() {

#ifdef _WIN32
    if (IsDebuggerPresent()) {
        DebugBreak();
    }
#elif defined(__EMSCRIPTEN__)
    emscripten_debugger();
#else
    raise(SIGTRAP);
#endif

}

void ShowErrorDialog(const char* message, const char* title) {
#ifdef _WIN32
    MessageBoxA(nullptr, message, title, MB_OK);
#elif defined(__EMSCRIPTEN__)
    EM_ASM(alert(UTF8ToString($0));, message);
#endif
}

}