// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_PLATFORM_TERMINAL_H
#define TRAM_SDK_PLATFORM_TERMINAL_H

namespace tram::Platform {

enum class TerminalColor {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    YELLOW,
    WHITE,
    GRAY,
    LIGHT_BLUE,
    LIGHT_GREEN,
    LIGHT_CYAN,
    LIGHT_RED,
    LIGHT_MAGENTA,
    LIGHT_YELLOW,
    LIGHT_WHITE,
    DEFAULT
};

void SwitchForeground(TerminalColor color);
void SwitchBackground(TerminalColor color);

}

#endif // TRAM_SDK_PLATFORM_TERMINAL_H