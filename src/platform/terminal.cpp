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

static void print_escape(const char* sequence) {
    printf("\033[%sm", sequence);
}

void SwitchForeground(TerminalColor color) {
    switch (color) {
        case TerminalColor::BLACK:          print_escape("30"); return;
        case TerminalColor::BLUE:           print_escape("34"); return;
        case TerminalColor::GREEN:          print_escape("32"); return;
        case TerminalColor::CYAN:           print_escape("36"); return;
        case TerminalColor::RED:            print_escape("31"); return;
        case TerminalColor::MAGENTA:        print_escape("35"); return;
        case TerminalColor::YELLOW:         print_escape("33"); return;
        case TerminalColor::WHITE:          print_escape("37"); return;

        case TerminalColor::GRAY:           print_escape("90"); return;
        case TerminalColor::LIGHT_BLUE:     print_escape("94"); return;
        case TerminalColor::LIGHT_GREEN:    print_escape("92"); return;
        case TerminalColor::LIGHT_CYAN:     print_escape("96"); return;
        case TerminalColor::LIGHT_RED:      print_escape("91"); return;
        case TerminalColor::LIGHT_MAGENTA:  print_escape("95"); return;
        case TerminalColor::LIGHT_YELLOW:   print_escape("93"); return;
        case TerminalColor::LIGHT_WHITE:    print_escape("97"); return;
        
        case TerminalColor::DEFAULT:        print_escape("39"); return;
        
        default:                            print_escape("39"); return;
    }
}

void SwitchBackground(TerminalColor color) {
    switch (color) {
        case TerminalColor::BLACK:          print_escape("40"); return;
        case TerminalColor::BLUE:           print_escape("44"); return;
        case TerminalColor::GREEN:          print_escape("42"); return;
        case TerminalColor::CYAN:           print_escape("46"); return;
        case TerminalColor::RED:            print_escape("41"); return;
        case TerminalColor::MAGENTA:        print_escape("45"); return;
        case TerminalColor::YELLOW:         print_escape("43"); return;
        case TerminalColor::WHITE:          print_escape("47"); return;

        case TerminalColor::GRAY:           print_escape("100"); return;
        case TerminalColor::LIGHT_BLUE:     print_escape("104"); return;
        case TerminalColor::LIGHT_GREEN:    print_escape("102"); return;
        case TerminalColor::LIGHT_CYAN:     print_escape("106"); return;
        case TerminalColor::LIGHT_RED:      print_escape("101"); return;
        case TerminalColor::LIGHT_MAGENTA:  print_escape("105"); return;
        case TerminalColor::LIGHT_YELLOW:   print_escape("103"); return;
        case TerminalColor::LIGHT_WHITE:    print_escape("107"); return;
        
        case TerminalColor::DEFAULT:        print_escape("49"); return;
        
        default:                            print_escape("49"); return;
    }
}

#endif





}