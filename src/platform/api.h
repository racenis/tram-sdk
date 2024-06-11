// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_PLATFORM_API_H
#define TRAM_SDK_PLATFORM_API_H

#include <cstddef>

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

}

namespace tram::Platform::Input {
    
void Init();
void Update();
void Uninit();

}

#endif // TRAM_SDK_PLATFORM_API_H