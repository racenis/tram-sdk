// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <platform/api.h>

#include <platform/platform.h>
#include <framework/ui.h>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
#endif

#ifndef _WIN32
    #include <GL/gl.h>
    #include <GLFW/glfw3.h>
#else
    #include <glad.c>
    #include <glfw3.h>
#endif


using namespace tram::UI;

namespace tram::Platform {

static GLFWwindow* WINDOW;

static GLFWcursor* cursors[4] = {nullptr};

static KeyboardKey GLFWKeyToKeyboardKey(int keycode);

void Window::Init() {
    glfwInit();

    glfwSetErrorCallback([](int code, const char* message){
        std::cout << "GLFW error code: " << code << " message: " << message << std::endl;
    });
    
#ifndef _WIN32
        // this is for opengl es 3.0
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#else
        // this is for opengl 4.0
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); 
#endif

    glfwWindowHint(GLFW_FOCUSED, GL_FALSE);
    
    WINDOW = glfwCreateWindow(800, 600, (const char*)u8"Tramvaju Drifta un Pagrabu Pētīšanas Simulatoru Izstrādes Rīkkopa Versija 0.0.9", nullptr, nullptr);
    if (WINDOW == nullptr) {
        std::cout << "GLFW window didn't open!" << std::endl;
        glfwTerminate();
        abort();
    }

    glfwMakeContextCurrent(WINDOW);
    
    // random settings that we don't need on web platform
#ifdef _WIN32
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "OpenGL context didn't open" << std::endl;
            abort();
        }

        glfwSwapInterval(1);
#endif

    glfwSetFramebufferSizeCallback(WINDOW, [](GLFWwindow* window, int width, int height) {
        UI::ScreenResize(width, height);
    });

    glfwSetWindowCloseCallback(WINDOW, [](GLFWwindow* window) {
        UI::ScreenClose();
    });

    glfwSetKeyCallback(WINDOW, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS) {
            KeyPress(GLFWKeyToKeyboardKey(key));
        }
        
        if (action == GLFW_RELEASE) {
            KeyRelease(GLFWKeyToKeyboardKey(key));
        }
        
        // GLFW doesn't forward backspace to CharCallback, so instead we will
        // handle this here.
        if (action == GLFW_PRESS || action == GLFW_REPEAT) switch (key) {
            case GLFW_KEY_BACKSPACE:
                KeyCode(8);
                break;
            case GLFW_KEY_ENTER:
                KeyCode(10);
                break;
            default:
                break;
        }
    });
    
    glfwSetCursorPosCallback(WINDOW, [](GLFWwindow* window, double xpos, double ypos) {
        KeyMouse(xpos, ypos);
    });
    
    glfwSetMouseButtonCallback(WINDOW, [](GLFWwindow* window, int button, int action, int mods) {
        if (action == GLFW_PRESS) {
            KeyPress(GLFWKeyToKeyboardKey(button));
        }
        
        if (action == GLFW_RELEASE) {
            KeyRelease(GLFWKeyToKeyboardKey(button));
        }
    });
    
    glfwSetScrollCallback(WINDOW, [](GLFWwindow* window, double xoffset, double yoffset) {
        KeyScroll(yoffset);
    });
    
    glfwSetCharCallback(WINDOW, [](GLFWwindow* window, unsigned int codepoint) {
        KeyCode(codepoint);
    });


    //glfwSetWindowSizeLimits(WINDOW, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetWindowSizeLimits(WINDOW, 160, 120, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwSetInputMode(WINDOW, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //glfwSetInputMode(WINDOW, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    cursors[CURSOR_DEFAULT] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    cursors[CURSOR_TEXT] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    cursors[CURSOR_CLICK] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
}

void Window::Update() {
    glfwSwapBuffers(WINDOW);
}

void Window::Uninit() {
    glfwTerminate();
}

void Window::SetTitle(const char* title) {
    assert(WINDOW);
    glfwSetWindowTitle (WINDOW, title);
}

void Window::SetSize(int w, int h) {
    assert(WINDOW);
    glfwSetWindowSize(WINDOW, w, h);
}

void Window::SetCursor(CursorType cursor) {
    glfwSetCursor(WINDOW, cursors[cursor]);
}

void Window::SetCursorPosition(float x, float y) {
    glfwSetCursorPos(WINDOW, x, y);
}

void Window::EnableCursor() {
    glfwSetInputMode(WINDOW, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::DisableCursor() {
    glfwSetInputMode(WINDOW, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool Window::IsRawInput() {
    return glfwGetInputMode(WINDOW, GLFW_RAW_MOUSE_MOTION);
}

void Window::SetRawInput(bool input) {
    glfwSetInputMode(WINDOW, GLFW_RAW_MOUSE_MOTION, input);
}

double Window::GetTime() {
    return glfwGetTime();
}

int current_monitor = 0;
int Window::GetCurrentMonitor() {
    return current_monitor;
}

int Window::GetMonitorCount() {
    int count;
    glfwGetMonitors(&count);
    return count;
}

void Window::SetMonitor(int monitor) {
    current_monitor = monitor;
}

bool is_fullscreen = false;
bool Window::IsFullscreen() {
    return is_fullscreen;
}

void Window::SetFullscreen(bool fullscreen) {
    is_fullscreen = fullscreen;
    if (fullscreen) {
        int monitor_count;
        GLFWmonitor** monitors = glfwGetMonitors(&monitor_count);
        assert(current_monitor < monitor_count);
        int width, height;
        glfwGetWindowSize(WINDOW, &width, &height);
        glfwSetWindowMonitor(WINDOW,
                             monitors[current_monitor],
                             0,
                             0,
                             width,
                             height,
                             60);
    } else {
        glfwSetWindowMonitor(WINDOW,
                            nullptr,
                            16,
                            32,
                            800,
                            600,
                            60);
    }
}

bool vsync = true;
bool Window::IsVsync() {
    return vsync;
}

void Window::SetVsync(bool value) {
    vsync = value;
    
    if (vsync) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }
}




void Input::Init() {
    
}

void Input::Update() {
    glfwPollEvents();
}

void Input::Uninit() {

}

/*static void CharacterCallback(GLFWwindow* window, unsigned int codepoint) {
    std::cout << "Character callback: " << codepoint << std::endl;
    auto len = strlen(input_text);
    if (len < input_text_len-1) {
        input_text[len] = codepoint;
        input_text[len+1] = '\0';
    }
}*/

/*static void CharacterBackspaceCallback() {
    std::cout << "Backspace callback! " << std::endl;
    if (!input_text) return;
    auto len = strlen(input_text);
    if (len > 0) {
        input_text[len-1] = '\0';
    }
}*/

/// Maps a glfw keycode to a KeyboardKey.
static KeyboardKey GLFWKeyToKeyboardKey (int keycode) {
    switch (keycode) {
        case GLFW_MOUSE_BUTTON_LEFT:    return KEY_LEFTMOUSE;
        case GLFW_MOUSE_BUTTON_RIGHT:   return KEY_RIGHTMOUSE;
        case GLFW_MOUSE_BUTTON_MIDDLE:  return KEY_MIDDLEMOUSE;
        case GLFW_KEY_SPACE:            return KEY_SPACE;
        case GLFW_KEY_APOSTROPHE:       return KEY_APOSTROPHE;
        case GLFW_KEY_COMMA:            return KEY_COMMA;
        case GLFW_KEY_MINUS:            return KEY_MINUS;
        case GLFW_KEY_PERIOD:           return KEY_PERIOD;
        case GLFW_KEY_SLASH:            return KEY_SLASH;
        case GLFW_KEY_0:                return KEY_0;
        case GLFW_KEY_1:                return KEY_1;
        case GLFW_KEY_2:                return KEY_2;
        case GLFW_KEY_3:                return KEY_3;
        case GLFW_KEY_4:                return KEY_4;
        case GLFW_KEY_5:                return KEY_5;
        case GLFW_KEY_6:                return KEY_6;
        case GLFW_KEY_7:                return KEY_7;
        case GLFW_KEY_8:                return KEY_8;
        case GLFW_KEY_9:                return KEY_9;
        case GLFW_KEY_SEMICOLON:        return KEY_SEMICOLON;
        case GLFW_KEY_EQUAL:            return KEY_EQUAL;
        case GLFW_KEY_A:                return KEY_A;
        case GLFW_KEY_B:                return KEY_B;
        case GLFW_KEY_C:                return KEY_C;
        case GLFW_KEY_D:                return KEY_D;
        case GLFW_KEY_E:                return KEY_E;
        case GLFW_KEY_F:                return KEY_F;
        case GLFW_KEY_G:                return KEY_G;
        case GLFW_KEY_H:                return KEY_H;
        case GLFW_KEY_I:                return KEY_I;
        case GLFW_KEY_J:                return KEY_J;
        case GLFW_KEY_K:                return KEY_K;
        case GLFW_KEY_L:                return KEY_L;
        case GLFW_KEY_M:                return KEY_M;
        case GLFW_KEY_N:                return KEY_N;
        case GLFW_KEY_O:                return KEY_O;
        case GLFW_KEY_P:                return KEY_P;
        case GLFW_KEY_Q:                return KEY_Q;
        case GLFW_KEY_R:                return KEY_R;
        case GLFW_KEY_S:                return KEY_S;
        case GLFW_KEY_T:                return KEY_T;
        case GLFW_KEY_U:                return KEY_U;
        case GLFW_KEY_V:                return KEY_V;
        case GLFW_KEY_W:                return KEY_W;
        case GLFW_KEY_X:                return KEY_X;
        case GLFW_KEY_Y:                return KEY_Y;
        case GLFW_KEY_Z:                return KEY_Z;
        case GLFW_KEY_LEFT_BRACKET:     return KEY_LEFT_BRACKET;
        case GLFW_KEY_BACKSLASH:        return KEY_BACKSLASH;
        case GLFW_KEY_RIGHT_BRACKET:    return KEY_RIGHT_BRACKET;
        case GLFW_KEY_GRAVE_ACCENT:     return KEY_GRAVE_ACCENT;
        case GLFW_KEY_WORLD_1:          return KEY_WORLD_1;
        case GLFW_KEY_WORLD_2:          return KEY_WORLD_2;
        case GLFW_KEY_ESCAPE:           return KEY_ESCAPE;
        case GLFW_KEY_ENTER:            return KEY_ENTER;
        case GLFW_KEY_TAB:              return KEY_TAB;
        case GLFW_KEY_BACKSPACE:        return KEY_BACKSPACE;
        case GLFW_KEY_INSERT:           return KEY_INSERT;
        case GLFW_KEY_DELETE:           return KEY_DELETE;
        case GLFW_KEY_RIGHT:            return KEY_RIGHT;
        case GLFW_KEY_LEFT:             return KEY_LEFT;
        case GLFW_KEY_DOWN:             return KEY_DOWN;
        case GLFW_KEY_UP:               return KEY_UP;
        case GLFW_KEY_PAGE_UP:          return KEY_PAGE_UP;
        case GLFW_KEY_PAGE_DOWN:        return KEY_PAGE_DOWN;
        case GLFW_KEY_HOME:             return KEY_HOME;
        case GLFW_KEY_END:              return KEY_END;
        case GLFW_KEY_CAPS_LOCK:        return KEY_CAPS_LOCK;
        case GLFW_KEY_SCROLL_LOCK:      return KEY_SCROLL_LOCK;
        case GLFW_KEY_NUM_LOCK:         return KEY_NUM_LOCK;
        case GLFW_KEY_PRINT_SCREEN:     return KEY_PRINT_SCREEN;
        case GLFW_KEY_PAUSE:            return KEY_PAUSE;
        case GLFW_KEY_F1:               return KEY_F1;
        case GLFW_KEY_F2:               return KEY_F2;
        case GLFW_KEY_F3:               return KEY_F3;
        case GLFW_KEY_F4:               return KEY_F4;
        case GLFW_KEY_F5:               return KEY_F5;
        case GLFW_KEY_F6:               return KEY_F6;
        case GLFW_KEY_F7:               return KEY_F7;
        case GLFW_KEY_F8:               return KEY_F8;
        case GLFW_KEY_F9:               return KEY_F9;
        case GLFW_KEY_F10:              return KEY_F10;
        case GLFW_KEY_F11:              return KEY_F11;
        case GLFW_KEY_F12:              return KEY_F12;
        case GLFW_KEY_F13:              return KEY_F13;
        case GLFW_KEY_F14:              return KEY_F14;
        case GLFW_KEY_F15:              return KEY_F15;
        case GLFW_KEY_F16:              return KEY_F16;
        case GLFW_KEY_F17:              return KEY_F17;
        case GLFW_KEY_F18:              return KEY_F18;
        case GLFW_KEY_F19:              return KEY_F19;
        case GLFW_KEY_F20:              return KEY_F20;
        case GLFW_KEY_F21:              return KEY_F21;
        case GLFW_KEY_F22:              return KEY_F22;
        case GLFW_KEY_F23:              return KEY_F23;
        case GLFW_KEY_F24:              return KEY_F24;
        case GLFW_KEY_F25:              return KEY_F25;
        case GLFW_KEY_KP_0:             return KEY_KP_0;
        case GLFW_KEY_KP_1:             return KEY_KP_1;
        case GLFW_KEY_KP_2:             return KEY_KP_2;
        case GLFW_KEY_KP_3:             return KEY_KP_3;
        case GLFW_KEY_KP_4:             return KEY_KP_4;
        case GLFW_KEY_KP_5:             return KEY_KP_5;
        case GLFW_KEY_KP_6:             return KEY_KP_6;
        case GLFW_KEY_KP_7:             return KEY_KP_7;
        case GLFW_KEY_KP_8:             return KEY_KP_8;
        case GLFW_KEY_KP_9:             return KEY_KP_9;
        case GLFW_KEY_KP_DECIMAL:       return KEY_KP_DECIMAL;
        case GLFW_KEY_KP_DIVIDE:        return KEY_KP_DIVIDE;
        case GLFW_KEY_KP_MULTIPLY:      return KEY_KP_MULTIPLY;
        case GLFW_KEY_KP_SUBTRACT:      return KEY_KP_SUBTRACT;
        case GLFW_KEY_KP_ADD:           return KEY_KP_ADD;
        case GLFW_KEY_KP_ENTER:         return KEY_KP_ENTER;
        case GLFW_KEY_KP_EQUAL:         return KEY_KP_EQUAL;
        case GLFW_KEY_LEFT_SHIFT:       return KEY_LEFT_SHIFT;
        case GLFW_KEY_LEFT_CONTROL:     return KEY_LEFT_CONTROL;
        case GLFW_KEY_LEFT_ALT:         return KEY_LEFT_ALT;
        case GLFW_KEY_LEFT_SUPER:       return KEY_LEFT_SUPER;
        case GLFW_KEY_RIGHT_SHIFT:      return KEY_RIGHT_SHIFT;
        case GLFW_KEY_RIGHT_CONTROL:    return KEY_RIGHT_CONTROL;
        case GLFW_KEY_RIGHT_ALT:        return KEY_RIGHT_ALT;
        case GLFW_KEY_RIGHT_SUPER:      return KEY_RIGHT_SUPER;
        case GLFW_KEY_MENU:             return KEY_MENU;
        default: 
            std::cout << "UNRECOGNIZED KEYCODE: " << keycode << std::endl;
            return KEY_SPACE;
    }
}    

}
