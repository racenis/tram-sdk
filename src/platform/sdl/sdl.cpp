// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <platform/api.h>

#include <platform/platform.h>
#include <framework/ui.h>
#include <framework/logging.h>

#include <render/api.h>

#include <thread>

#include <SDL.h>
#include <SDL_syswm.h>
#include <glad.c>

#ifdef _WIN32
#include <d3dx9.h>
#endif

#undef ERROR

using namespace tram::UI;

namespace tram::Platform {

static SDL_Window* window = nullptr;

static SDL_Renderer* renderer = nullptr;
static SDL_Texture* frame_texture = nullptr;
static uint32_t* frame_buffer = nullptr;

static int screen_width = 800;
static int screen_height = 600;
static int screen_scale = 1;
static int relpos_x = screen_width/2;
static int relpos_y = screen_height/2;
static bool cursor_enabled = false;

static std::thread::id render_context_thread = std::this_thread::get_id();

#ifdef _WIN32
static IDirect3DDevice9* d3d_device = nullptr;
#endif

static void SoftwareRenderContextUpdate() {
    //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    //SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    //SDL_RenderSetLogicalSize(renderer, screen_width/screen_scale, screen_height/screen_scale);
    
    frame_texture = SDL_CreateTexture(renderer,
                           //SDL_PIXELFORMAT_ARGB8888,
                           SDL_PIXELFORMAT_RGB565,
                           SDL_TEXTUREACCESS_STREAMING,
                           screen_width/screen_scale, screen_height/screen_scale);
    
    frame_buffer = (uint32_t*)malloc(screen_width * screen_height * sizeof(uint32_t));
    
    Render::API::SetDevice(frame_buffer);
}

void Window::Init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Log(Severity::WARNING, System::PLATFORM, "SDL2 didn't open!");
        abort();
    }
    
    uint32_t window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;
    
    if (Render::API::GetContext() == Render::API::CONTEXT_OPENGL) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
        
        window_flags |= SDL_WINDOW_OPENGL;
    }
    
    window = SDL_CreateWindow((const char*)u8"Tramvaju Drifta un Pagrabu Pētīšanas Simulatoru Izstrādes Rīkkopa Versija 0.1.1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, window_flags);
    if (window == nullptr) {
        Log(Severity::WARNING, System::PLATFORM, "SDL2 window didn't open! {}", SDL_GetError());
        abort();
    }
    
    if (Render::API::GetContext() == Render::API::CONTEXT_OPENGL) {
        void* context = SDL_GL_CreateContext(window);
        if (context == nullptr) {
            Log(Severity::ERROR, System::PLATFORM, "SDL2 context didn't open! {}", SDL_GetError());
            abort();
        }
        
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            Log(Severity::ERROR, System::PLATFORM, "OpenGL context didn't open!");
            abort();
        }
        
        SDL_GL_SetSwapInterval(1);
    }
    
#ifdef _WIN32
    if (Render::API::GetContext() == Render::API::CONTEXT_DIRECT3D) {
        IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
        if (!d3d9) {
            Log(Severity::ERROR, System::PLATFORM, "Direct3D didn't open!");
            abort();
        }
        
        D3DCAPS9 caps;
        d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

        int vertex_processing = 0;
        if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
            vertex_processing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
            Log(Severity::DEFAULT, System::PLATFORM, "Hardware T&L available.");
        } else {
            vertex_processing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            Log(Severity::DEFAULT, System::PLATFORM, "Hardware T&L unavailable.");
        }
        
        if (caps.MaxVertexBlendMatrixIndex == 0) {
            vertex_processing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
            Log(Severity::DEFAULT, System::PLATFORM, "Hardware matrix blending unavailable.");
        }
        
        SDL_SysWMinfo wm_info;
        SDL_VERSION(&wm_info.version);
        SDL_GetWindowWMInfo(window, &wm_info);
   
        D3DPRESENT_PARAMETERS d3dpp;
        d3dpp.BackBufferWidth               = screen_width;
        d3dpp.BackBufferHeight              = screen_height;
        d3dpp.BackBufferFormat              = D3DFMT_A8R8G8B8;
        d3dpp.BackBufferCount               = 1;
        d3dpp.MultiSampleType               = D3DMULTISAMPLE_NONE;
        d3dpp.MultiSampleQuality            = 0;
        d3dpp.SwapEffect                    = D3DSWAPEFFECT_DISCARD; 
        d3dpp.hDeviceWindow                 = wm_info.info.win.window;
        d3dpp.Windowed                      = true;
        d3dpp.EnableAutoDepthStencil        = true; 
        d3dpp.AutoDepthStencilFormat        = D3DFMT_D24S8;
        d3dpp.Flags                         = 0;
        d3dpp.FullScreen_RefreshRateInHz    = D3DPRESENT_RATE_DEFAULT;
        //d3dpp.PresentationInterval          = D3DPRESENT_INTERVAL_IMMEDIATE; // no vsync
        d3dpp.PresentationInterval          = D3DPRESENT_INTERVAL_ONE; // vsync
   

        HRESULT hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT,
                                        D3DDEVTYPE_HAL,
                                        wm_info.info.win.window,
                                        vertex_processing,
                                        &d3dpp,
                                        &d3d_device);
                                   
        if (FAILED(hr)) {
            d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

            hr = d3d9->CreateDevice(D3DADAPTER_DEFAULT,
                                    D3DDEVTYPE_HAL,
                                    wm_info.info.win.window,
                                    vertex_processing,
                                    &d3dpp,
                                    &d3d_device);

            if (FAILED(hr)) {
                d3d9->Release();
                std::cout << "Direct3D device didn't get created!" << std::endl;
                abort();
            }
        }
    
        d3d9->Release();

        Render::API::SetDevice(d3d_device);
    }
#endif
    
    if (Render::API::GetContext() == Render::API::CONTEXT_SOFTWARE) {
        renderer = SDL_CreateRenderer(window, -1, 0 /*SDL_RENDERER_SOFTWARE*/);
        
        if (!renderer) {
            Log(Severity::ERROR, System::PLATFORM, "SDL_Renderer didn't open!");
            abort();
        }

        int ww = screen_width/screen_scale;
        int hh = screen_height/screen_scale;
        //Render::API::SetScreenSize(ww, hh);
        Render::SetScreenSize(ww, hh);

        UI::ScreenResize(screen_width/screen_scale, screen_height/screen_scale);

        SoftwareRenderContextUpdate();
    }
    
    render_context_thread = std::this_thread::get_id();
    
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Window::Update() {
    if (Render::API::GetContext() == Render::API::CONTEXT_OPENGL) {
        SDL_GL_SwapWindow(window);
    }
    
#ifdef _WIN32
    if (Render::API::GetContext() == Render::API::CONTEXT_DIRECT3D) {
        d3d_device->Present(0, 0, 0, 0);
    }
#endif
    
    if (Render::API::GetContext() == Render::API::CONTEXT_SOFTWARE) {
        SDL_UpdateTexture(frame_texture, nullptr, frame_buffer, (screen_width/screen_scale) * sizeof(uint16_t) /*sizeof(uint32_t)*/);
        
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, frame_texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
}

void Window::Uninit() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Window::SetTitle(const char* title) {
    SDL_SetWindowTitle(window, title);
}

void Window::SetSize(int w, int h) {
    SDL_SetWindowSize(window, w, h);
    
    int ww = w/screen_scale;
    int hh = h/screen_scale;
    UI::ScreenResize(ww, hh);
    
    SoftwareRenderContextUpdate();

    screen_width = w;
    screen_height = h;
}

void Window::SetScale(int s) {
    screen_scale = s;
}

void Window::SetCursor(CursorType cursor) {
    
}

void Window::SetCursorPosition(float x, float y) {
    SDL_WarpMouseInWindow(window, x, y);
}

void Window::EnableCursor() {
    SDL_SetRelativeMouseMode(SDL_FALSE);
    
    cursor_enabled = true;
}

void Window::DisableCursor() {
    SDL_SetRelativeMouseMode(SDL_TRUE);
    
    cursor_enabled = false;
    
    relpos_x = screen_width/2;
    relpos_y = screen_height/2;
}

double Window::GetTime() {
    return (float)SDL_GetTicks() / 1000.0f;
}

// TODO: properly implement all of these very nice functions
int Window::GetCurrentMonitor() {
    return 0;
}

int Window::GetMonitorCount() {
    return 1;
}

void Window::SetMonitor(int monitor) {
    
}

bool Window::IsFullscreen() {
    return false;
}

void Window::SetFullscreen(bool) {
    
}

bool Window::IsVsync() {
    return false;
}

void Window::SetVsync(bool) {
    
}

bool Window::IsRawInput() {
    return false;
}

void Window::SetRawInput(bool) {
    
}

bool Window::IsRenderContextThread() {
    return render_context_thread == std::this_thread::get_id();
}

static KeyboardKey SDLKeyToKeyboardKey(SDL_Keysym keycode);
static KeyboardKey SDLMouseKeyToKeyboardKey(uint8_t button);

void Input::Init() {
    
}

void Input::Update() {
    SDL_Event event;
    while( SDL_PollEvent(&event) != 0) {
        switch (event.type) {
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED: {
                        int off_x = relpos_x - screen_width/2;
                        int off_y = relpos_y - screen_height/2;
                    
                        screen_width = event.window.data1;
                        screen_height = event.window.data2;
                    
                        relpos_x = off_x + screen_width/2;
                        relpos_y = off_y + screen_height/2;

                        SoftwareRenderContextUpdate();

                        UI::ScreenResize(screen_width/screen_scale, screen_height/screen_scale);
                        } break;
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
                if (cursor_enabled) {
                    KeyMouse((float)event.motion.x/screen_scale, (float)event.motion.y/screen_scale);
                } else {
                    relpos_x += event.motion.xrel * 2;
                    relpos_y += event.motion.yrel * 2;
                    
                    KeyMouse((float)relpos_x/screen_scale, (float)relpos_y/screen_scale);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                KeyPress(SDLMouseKeyToKeyboardKey(event.button.button));
                break;
            case SDL_MOUSEBUTTONUP:
                KeyRelease(SDLMouseKeyToKeyboardKey(event.button.button));
                break;
            case SDL_MOUSEWHEEL:
                KeyScroll(event.wheel.y);
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

static KeyboardKey SDLMouseKeyToKeyboardKey(uint8_t button) {
    switch (button) {
        case SDL_BUTTON_LEFT: return UI::KEY_LEFTMOUSE;
        case SDL_BUTTON_MIDDLE: return UI::KEY_MIDDLEMOUSE;
        case SDL_BUTTON_RIGHT: return UI::KEY_RIGHTMOUSE;
        default: return KEY_SPACE;
    }
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
            Log(Severity::WARNING, System::PLATFORM, "UNRECOGNIZED KEYCODE: {}", keycode.scancode);
            return KEY_SPACE;
}}

}