// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_PLATFORM_API_H
#define TRAM_SDK_PLATFORM_API_H

#include <cstddef>

/**
 * @namespace tram::Platform::Window
 * 
 * Windowing backend API.
 */
 
/**
 * @namespace tram::Platform::Input
 * 
 * Input backend API.
 */

namespace tram::Platform::Window {

void Init();
void Update();
void Uninit();

void SetTitle(const char*);
void SetSize(int w, int h);

enum CursorType {
    CURSOR_DEFAULT,
    CURSOR_TEXT,
    CURSOR_CLICK
};

void SetCursor(CursorType);
void SetCursorPosition(float x, float y);

void EnableCursor();
void DisableCursor();

void SetScale(int s);

double GetTime();

int GetCurrentMonitor();
int GetMonitorCount();
void SetMonitor(int monitor);
bool IsFullscreen();
void SetFullscreen(bool);
bool IsVsync();
void SetVsync(bool);
bool IsRawInput();
void SetRawInput(bool);

bool IsRenderContextThread();

}

namespace tram::Platform::Input {
    
void Init();
void Update();
void Uninit();

}

#endif // TRAM_SDK_PLATFORM_API_H