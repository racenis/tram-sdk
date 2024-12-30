// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <platform/other.h>


#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
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
#endif
    
    // TODO: look up what do here on other platforms
}

void ShowErrorDialog(const char* message, const char* title) {
#ifdef _WIN32
    MessageBoxA(nullptr, message, title, MB_OK);
#endif
}

}