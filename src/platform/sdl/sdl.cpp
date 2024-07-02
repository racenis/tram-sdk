#include <platform/api.h>

#include <platform/platform.h>
#include <framework/ui.h>

#include <sdl2/SDL.h>
#include <glad.c>

using namespace tram::UI;

namespace tram::Platform {

static SDL_Window* window;
    
void Window::Init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL2 window didn't open!" << std::endl;
        abort();
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    //Create window
    window = SDL_CreateWindow((const char*)u8"Tramvaju Drifta un Pagrabu Pētīšanas Simulatoru Izstrādes Rīkkopa Versija 0.0.9", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cout << "SDL2 window didn't open! " << SDL_GetError() << std::endl;
        abort();
    }
    
    //Create context
    void* context = SDL_GL_CreateContext(window);
    if (context == nullptr) {
        std::cout << "SDL2 context didn't open! " << SDL_GetError() << std::endl;
        abort();
    }
    
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cout << "OpenGL context didn't open!" << std::endl;
        abort();
    }
    
    SDL_GL_SetSwapInterval(1);
    
    //glClearColor(0.f, 1.f, 0.f, 1.f);
}

void Window::Update() {
    SDL_GL_SwapWindow(window);
}

void Window::Uninit() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Window::SetTitle(const char* title) {
    
}

void Window::SetSize(int w, int h) {
    
}

void Window::SetCursor(CursorType cursor) {
    
}

void Window::SetCursorPosition(float x, float y) {
    
}

void Window::EnableCursor() {
    
}

void Window::DisableCursor() {
    
}


static KeyboardKey SDLKeyToKeyboardKey(SDL_Keysym keycode);

void Input::Init() {
    
}

void Input::Update() {
    SDL_Event event;
    while( SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        UI::ScreenResize(event.window.data1, event.window.data2);
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYDOWN:
                KeyPress(SDLKeyToKeyboardKey(event.key.keysym));
                break;
            case SDL_KEYUP:
                KeyRelease(SDLKeyToKeyboardKey(event.key.keysym));
                break;
    
            case SDL_MOUSEMOTION:
                break;
            case SDL_MOUSEBUTTONDOWN:
                break;
            case SDL_MOUSEBUTTONUP:
                break;
            case SDL_MOUSEWHEEL:
                break;
            
            case SDL_QUIT:
                UI::ScreenClose();
                break;
            
            default:
                break;
        }
    }
}

void Input::Uninit() {

}

static KeyboardKey SDLKeyToKeyboardKey(SDL_Keysym keycode) {
    switch (keycode.scancode) {
        case SDL_SCANCODE_A: return KEY_A;
        case SDL_SCANCODE_B: return KEY_B;
        case SDL_SCANCODE_C: return KEY_C;
        case SDL_SCANCODE_D: return KEY_D;
        case SDL_SCANCODE_E: return KEY_E;
        case SDL_SCANCODE_F: return KEY_F;
        case SDL_SCANCODE_G: return KEY_G;
        case SDL_SCANCODE_H: return KEY_H;
        case SDL_SCANCODE_I: return KEY_I;
        case SDL_SCANCODE_J: return KEY_J;
        case SDL_SCANCODE_K: return KEY_K;
        case SDL_SCANCODE_L: return KEY_L;
        case SDL_SCANCODE_M: return KEY_M;
        case SDL_SCANCODE_N: return KEY_N;
        case SDL_SCANCODE_O: return KEY_O;
        case SDL_SCANCODE_P: return KEY_P;
        case SDL_SCANCODE_Q: return KEY_Q;
        case SDL_SCANCODE_R: return KEY_R;
        case SDL_SCANCODE_S: return KEY_S;
        case SDL_SCANCODE_T: return KEY_T;
        case SDL_SCANCODE_U: return KEY_U;
        case SDL_SCANCODE_V: return KEY_V;
        case SDL_SCANCODE_W: return KEY_W;
        case SDL_SCANCODE_X: return KEY_X;
        case SDL_SCANCODE_Y: return KEY_Y;
        case SDL_SCANCODE_Z: return KEY_Z;

        case SDL_SCANCODE_1: return KEY_1;
        case SDL_SCANCODE_2: return KEY_2;
        case SDL_SCANCODE_3: return KEY_3;
        case SDL_SCANCODE_4: return KEY_4;
        case SDL_SCANCODE_5: return KEY_5;
        case SDL_SCANCODE_6: return KEY_6;
        case SDL_SCANCODE_7: return KEY_7;
        case SDL_SCANCODE_8: return KEY_8;
        case SDL_SCANCODE_9: return KEY_9;
        case SDL_SCANCODE_0: return KEY_0;

        case SDL_SCANCODE_RETURN: return KEY_ENTER;
        case SDL_SCANCODE_ESCAPE: return KEY_ESCAPE;
        case SDL_SCANCODE_BACKSPACE: return KEY_BACKSPACE;
        case SDL_SCANCODE_TAB: return KEY_TAB;
        case SDL_SCANCODE_SPACE: return KEY_SPACE;

        case SDL_SCANCODE_MINUS: return KEY_MINUS;
        case SDL_SCANCODE_EQUALS: return KEY_EQUAL;
        case SDL_SCANCODE_LEFTBRACKET: return KEY_LEFT_BRACKET;
        case SDL_SCANCODE_RIGHTBRACKET: return KEY_RIGHT_BRACKET;
        case SDL_SCANCODE_BACKSLASH: return KEY_BACKSLASH;
        case SDL_SCANCODE_NONUSHASH: return KEY_BACKSLASH;
        case SDL_SCANCODE_SEMICOLON: return KEY_SEMICOLON;
        case SDL_SCANCODE_APOSTROPHE: return KEY_APOSTROPHE;
        case SDL_SCANCODE_GRAVE: return KEY_GRAVE_ACCENT;
        case SDL_SCANCODE_COMMA: return KEY_COMMA;
        case SDL_SCANCODE_PERIOD: return KEY_PERIOD;
        case SDL_SCANCODE_SLASH: return KEY_SLASH;

        case SDL_SCANCODE_CAPSLOCK: return KEY_CAPS_LOCK;

        case SDL_SCANCODE_F1: return KEY_F1;
        case SDL_SCANCODE_F2: return KEY_F2;
        case SDL_SCANCODE_F3: return KEY_F3;
        case SDL_SCANCODE_F4: return KEY_F4;
        case SDL_SCANCODE_F5: return KEY_F5;
        case SDL_SCANCODE_F6: return KEY_F6;
        case SDL_SCANCODE_F7: return KEY_F7;
        case SDL_SCANCODE_F8: return KEY_F8;
        case SDL_SCANCODE_F9: return KEY_F9;
        case SDL_SCANCODE_F10: return KEY_F10;
        case SDL_SCANCODE_F11: return KEY_F11;
        case SDL_SCANCODE_F12: return KEY_F12;

        case SDL_SCANCODE_PRINTSCREEN: return KEY_PRINT_SCREEN;
        case SDL_SCANCODE_SCROLLLOCK: return KEY_SCROLL_LOCK;
        case SDL_SCANCODE_PAUSE: return KEY_PAUSE;
        case SDL_SCANCODE_INSERT: return KEY_INSERT;
        case SDL_SCANCODE_HOME: return KEY_HOME;
        case SDL_SCANCODE_PAGEUP: return KEY_PAGE_UP;
        case SDL_SCANCODE_DELETE: return KEY_DELETE;
        case SDL_SCANCODE_END: return KEY_END;
        case SDL_SCANCODE_PAGEDOWN: return KEY_PAGE_DOWN;
        case SDL_SCANCODE_RIGHT: return KEY_RIGHT;
        case SDL_SCANCODE_LEFT: return KEY_LEFT;
        case SDL_SCANCODE_DOWN: return KEY_DOWN;
        case SDL_SCANCODE_UP: return KEY_UP;

        case SDL_SCANCODE_NUMLOCKCLEAR: return KEY_NUM_LOCK;
        case SDL_SCANCODE_KP_DIVIDE: return KEY_KP_DIVIDE;
        case SDL_SCANCODE_KP_MULTIPLY: return KEY_KP_MULTIPLY;
        case SDL_SCANCODE_KP_MINUS: return KEY_KP_SUBTRACT;
        case SDL_SCANCODE_KP_PLUS: return KEY_KP_ADD;
        case SDL_SCANCODE_KP_ENTER: return KEY_KP_ENTER;
        case SDL_SCANCODE_KP_1: return KEY_KP_1;
        case SDL_SCANCODE_KP_2: return KEY_KP_2;
        case SDL_SCANCODE_KP_3: return KEY_KP_3;
        case SDL_SCANCODE_KP_4: return KEY_KP_4;
        case SDL_SCANCODE_KP_5: return KEY_KP_5;
        case SDL_SCANCODE_KP_6: return KEY_KP_6;
        case SDL_SCANCODE_KP_7: return KEY_KP_7;
        case SDL_SCANCODE_KP_8: return KEY_KP_8;
        case SDL_SCANCODE_KP_9: return KEY_KP_9;
        case SDL_SCANCODE_KP_0: return KEY_KP_0;
        case SDL_SCANCODE_KP_PERIOD: return KEY_KP_DECIMAL;

        case SDL_SCANCODE_NONUSBACKSLASH: return KEY_BACKSLASH;
        case SDL_SCANCODE_KP_EQUALS: return KEY_KP_EQUAL;
        case SDL_SCANCODE_F13: return KEY_F13;
        case SDL_SCANCODE_F14: return KEY_F14;
        case SDL_SCANCODE_F15: return KEY_F15;
        case SDL_SCANCODE_F16: return KEY_F16;
        case SDL_SCANCODE_F17: return KEY_F17;
        case SDL_SCANCODE_F18: return KEY_F18;
        case SDL_SCANCODE_F19: return KEY_F19;
        case SDL_SCANCODE_F20: return KEY_F20;
        case SDL_SCANCODE_F21: return KEY_F21;
        case SDL_SCANCODE_F22: return KEY_F22;
        case SDL_SCANCODE_F23: return KEY_F23;
        case SDL_SCANCODE_F24: return KEY_F24;

        case SDL_SCANCODE_LCTRL: return KEY_LEFT_CONTROL;
        case SDL_SCANCODE_LSHIFT: return KEY_LEFT_SHIFT;
        case SDL_SCANCODE_LALT: return KEY_LEFT_ALT;
        case SDL_SCANCODE_RCTRL: return KEY_RIGHT_CONTROL;
        case SDL_SCANCODE_RSHIFT: return KEY_RIGHT_SHIFT;
        case SDL_SCANCODE_RALT: return KEY_RIGHT_ALT;
        
        default: 
            std::cout << "UNRECOGNIZED KEYCODE: " << keycode.scancode << std::endl;
            return KEY_SPACE;
}}

}