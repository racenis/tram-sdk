// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <platform/terminal.h>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <cstdio>
#endif

namespace tram::Platform {

#ifdef _WIN32

static int ColorToCharacterAttribute(TerminalColor color, bool foreground) {
    const int red = foreground ? FOREGROUND_RED : BACKGROUND_RED;
    const int green = foreground ? FOREGROUND_GREEN : BACKGROUND_GREEN;
    const int blue = foreground ? FOREGROUND_BLUE : BACKGROUND_BLUE;
    const int intense = foreground ? FOREGROUND_INTENSITY : BACKGROUND_INTENSITY;
    
    
    switch (color) {
        case TerminalColor::BLACK:          return 0;
        case TerminalColor::BLUE:           return               blue;
        case TerminalColor::GREEN:          return       green;
        case TerminalColor::CYAN:           return       green | blue;
        case TerminalColor::RED:            return red;
        case TerminalColor::MAGENTA:        return red |         blue;
        case TerminalColor::YELLOW:         return red | green;
        case TerminalColor::WHITE:          return red | green | blue;

        case TerminalColor::GRAY:           return                      intense;
        case TerminalColor::LIGHT_BLUE:     return               blue | intense;
        case TerminalColor::LIGHT_GREEN:    return       green |        intense;
        case TerminalColor::LIGHT_CYAN:     return       green | blue | intense;
        case TerminalColor::LIGHT_RED:      return red |                intense;
        case TerminalColor::LIGHT_MAGENTA:  return red |         blue | intense;
        case TerminalColor::LIGHT_YELLOW:   return red | green |        intense;
        case TerminalColor::LIGHT_WHITE:    return red | green | blue | intense;
        
        case TerminalColor::DEFAULT:        return foreground ? (red | green | blue) : 0;
        
        default:                            return red | green | blue;
    }
}
   
void SwitchForeground(TerminalColor color) {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(console, &info);
    
    short attribute = (info.wAttributes & 0xF0) | ColorToCharacterAttribute(color, true);
    
    SetConsoleTextAttribute(console, attribute);

}

void SwitchBackground(TerminalColor color) {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(console, &info);
    
    short attribute = (info.wAttributes & 0x0F) | ColorToCharacterAttribute(color, false);
    
    SetConsoleTextAttribute(console, attribute);
}


#else

void SwitchForeground(TerminalColor color) {
    // TODO: implement
}

void SwitchBackground(TerminalColor color) {
    // TODO: implement
}

#endif





}