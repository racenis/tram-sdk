// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <platform/platform.h>

#include <framework/core.h>
#include <framework/event.h>
#include <framework/ui.h>
#include <framework/system.h>

#include <render/render.h>
#include <render/api.h>

#include <fstream>
#include <cstring>

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #include <GL/gl.h>
    #include <GLFW/glfw3.h>
#else
    #include <glad.c>
    #include <glfw3.h>
#endif



using namespace tram::Render;

namespace tram::UI {

static KeyboardKey GLFWKeyToKeyboardKey(int keycode);

static InputState input_state = STATE_DEFAULT;

static GLFWwindow* WINDOW;

static float screen_width = 800.0f;
static float screen_height = 600.0f;

static float camera_pitch = 0.0f;
static float camera_yaw = -90.0f;

static GLFWcursor* cursors[4] = {nullptr};

static bool keyboard_keys_values[KEY_LASTKEY] = {false};
static float keyboard_axis_values[KEY_LASTAXIS] = {0.0f};

static float cursorchangex = 0.0f, cursorchangey = 0.0f;
static double cursorx_last = 0.0f, cursory_last = 0.0f;

static char* input_text = nullptr;
static uint32_t input_text_len = 0;

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
static void MouseKeyCallback(GLFWwindow* window, int button, int action, int mods);
static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
static void CharacterBackspaceCallback();

struct KeyBinding {
    KeyboardAction action = KEY_ACTION_NONE;
    void (*special_option)(void) = nullptr;
};

static std::unordered_map<KeyboardKey, KeyBinding> key_action_bindings = {
    {KEY_W, KeyBinding {.action = KEY_ACTION_FORWARD}},
    {KEY_S, KeyBinding {.action = KEY_ACTION_BACKWARD}},
    {KEY_A, KeyBinding {.action = KEY_ACTION_STRAFE_LEFT}},
    {KEY_D, KeyBinding {.action = KEY_ACTION_STRAFE_RIGHT}},
    {KEY_SPACE, KeyBinding {.action = KEY_ACTION_JUMP}},
    {KEY_LEFT_CONTROL, KeyBinding {.action = KEY_ACTION_CROUCH}},
    {KEY_LEFT_SHIFT, KeyBinding {.action = KEY_ACTION_SPRINT}},
    {KEY_E, KeyBinding {.action = KEY_ACTION_ACTIVATE}},
    
    {KEY_UP, KeyBinding {.action = KEY_ACTION_UP}},
    {KEY_DOWN, KeyBinding {.action = KEY_ACTION_DOWN}},
    {KEY_LEFT, KeyBinding {.action = KEY_ACTION_LEFT}},
    {KEY_RIGHT, KeyBinding {.action = KEY_ACTION_RIGHT}},

    {KEY_F1, KeyBinding {.special_option = [](){ EXIT = true; glfwSetWindowShouldClose(WINDOW, EXIT); }}},
    {KEY_F5, KeyBinding {.special_option = [](){ THIRD_PERSON = !THIRD_PERSON; }}},
    {KEY_F6, KeyBinding {.special_option = [](){ THIRD_PERSON = !THIRD_PERSON; }}},
    {KEY_F9, KeyBinding {.special_option = [](){ input_state = (input_state == STATE_FLYING) ? STATE_DEFAULT : STATE_FLYING; }}},
    {KEY_BACKSPACE, KeyBinding {.special_option = [](){ CharacterBackspaceCallback(); }}}
};

#ifndef __EMSCRIPTEN__
void APIENTRY RenderErrorCallback (uint32_t source, uint32_t type, uint32_t id, uint32_t severity, int32_t length, const char* message, const void*) {
    // apparently these are spammy, or something
    //if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    const char* source_str =    "UNKNOWN";
    const char* type_str =      "UNKNOWN";
    const char* severity_str =  "UNKNOWN";

    switch (source) {
        case GL_DEBUG_SOURCE_API_ARB:               source_str = "API";                 break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:     source_str = "WINDOW_SYSTEM";       break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:   source_str = "SHADER_COMPILER";     break;
        case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:       source_str = "THIRD_PARTY";         break;
        case GL_DEBUG_SOURCE_APPLICATION_ARB:       source_str = "APPLICATION";         break;
        case GL_DEBUG_SOURCE_OTHER_ARB:             source_str = "OTHER";               break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR_ARB:               type_str = "ERROR";                 break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: type_str = "DEPRECATED_BEHAVIOR";   break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:  type_str = "UNDEFINED_BEHAVIOR";    break; 
        case GL_DEBUG_TYPE_PORTABILITY_ARB:         type_str = "PORTABILITY";           break;
        case GL_DEBUG_TYPE_PERFORMANCE_ARB:         type_str = "PERFORMANCE";           break;
        case GL_DEBUG_TYPE_OTHER_ARB:               type_str = "OTHER";                 break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH_ARB:            severity_str = "HIGH";              break;
        case GL_DEBUG_SEVERITY_MEDIUM_ARB:          severity_str = "MEDIUM";            break;
        case GL_DEBUG_SEVERITY_LOW_ARB:             severity_str = "LOW";               break;
    }

    std::cout << "OpenGL Debug Message: " << source_str << " " << type_str << " " << severity_str << " " << id << std::endl;
    std::cout << message << std::endl;
}
#endif

void BindKeyboardKey (KeyboardKey key, KeyboardAction action) {
    key_action_bindings[key] = {.action = action};
}

void BindKeyboardKey (KeyboardKey key, void (*action)()) {
    key_action_bindings[key] = {.special_option = action};
}

void Init(){
    assert(System::IsInitialized(System::SYSTEM_CORE));
    
    glfwInit();

    glfwSetErrorCallback([](int code, const char* message){
        std::cout << "GLFW error code: " << code << " message: " << message << std::endl;
    });
    
    if (CURRENT_PLATFORM == PLATFORM_WEB) {
        // this is for opengl es 3.0
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    } else {
        // this is for opengl 4.0
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); 
    }

    glfwWindowHint(GLFW_FOCUSED, GL_FALSE);
    
    WINDOW = glfwCreateWindow(800, 600, (const char*)u8"Tramvaju Drifta un Pagrabu Pētīšanas Simulatoru Izstrādes Rīkkopa Versija 0.0.8", nullptr, nullptr);
    if (WINDOW == nullptr){
        std::cout << "GLFW window didn't open!" << std::endl;
        glfwTerminate();
        abort();
    }

    glfwMakeContextCurrent(WINDOW);
    
    // random settings that we don't need on web platform
#ifndef __EMSCRIPTEN__
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            std::cout << "OpenGL context didn't open" << std::endl;
            abort();
        }

        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB); 
        glDebugMessageCallbackARB(RenderErrorCallback, nullptr);
        glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

        glfwSwapInterval(1);
#endif

    glfwSetFramebufferSizeCallback(WINDOW, [](GLFWwindow* window, int width, int height) {
        screen_width = width;
        screen_height = height;
        
        Render::SetScreenSize(width, height);
    });

    glfwSetKeyCallback (WINDOW, KeyCallback);
    glfwSetCursorPosCallback (WINDOW, MouseCallback);
    glfwSetMouseButtonCallback (WINDOW, MouseKeyCallback);
    glfwSetScrollCallback (WINDOW, ScrollCallback);

    glfwSetWindowSizeLimits (WINDOW, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwSetInputMode (WINDOW, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    cursors[CURSOR_DEFAULT] = glfwCreateStandardCursor (GLFW_ARROW_CURSOR);
    cursors[CURSOR_TEXT] = glfwCreateStandardCursor (GLFW_IBEAM_CURSOR);
    cursors[CURSOR_CLICK] = glfwCreateStandardCursor (GLFW_HAND_CURSOR);
    
    System::SetInitialized (System::SYSTEM_UI, true);
}

void Uninit() {
    glfwTerminate();
}

void Update() {
    if (input_state == STATE_FLYING) {
        vec3 camera_position = GetCameraPosition();
        quat camera_rotation = GetCameraRotation();
        
        if (glfwGetKey(WINDOW, GLFW_KEY_W) == GLFW_PRESS)
            camera_position += camera_rotation * DIRECTION_FORWARD * CAMERA_SPEED;
        if (glfwGetKey(WINDOW, GLFW_KEY_S) == GLFW_PRESS)
            camera_position -= camera_rotation * DIRECTION_FORWARD * CAMERA_SPEED;
        if (glfwGetKey(WINDOW, GLFW_KEY_A) == GLFW_PRESS)
            camera_position -= camera_rotation * DIRECTION_SIDE * CAMERA_SPEED;
        if (glfwGetKey(WINDOW, GLFW_KEY_D) == GLFW_PRESS)
            camera_position += camera_rotation * DIRECTION_SIDE * CAMERA_SPEED;
            
        cursorchangex = keyboard_axis_values[KEY_MOUSE_X] - cursorx_last;
        cursorchangey = keyboard_axis_values[KEY_MOUSE_Y] - cursory_last;
        cursorx_last = keyboard_axis_values[KEY_MOUSE_X];
        cursory_last = keyboard_axis_values[KEY_MOUSE_Y];

        camera_yaw += cursorchangex * CAMERA_SENSITIVITY;
        camera_pitch += cursorchangey * CAMERA_SENSITIVITY;
        camera_pitch = camera_pitch > 90.0f ? 90.0f : camera_pitch < -90.0f ? -90.0f : camera_pitch;
        
        SetCameraPosition (camera_position);
        SetCameraRotation (quat(vec3(-glm::radians(camera_pitch), -glm::radians(camera_yaw), 0.0f)));
    }
    
    // generate keypress events. these need to be generated every tick that the
    // keyboard key is being pressed down.
    for (auto& b : key_action_bindings) {
        if (PollKeyboardKey(b.first) && b.second.action) {
            Event::Post({Event::KEYPRESS, b.second.action, 0, nullptr});
        }
    }
    
    keyboard_axis_values[KEY_MOUSE_SCROLL] = 0.0f;
    
    glfwPollEvents();
}

static void KeyCallback (GLFWwindow* window, int key, int scancode, int action, int mods) {
    const auto& binding = key_action_bindings[GLFWKeyToKeyboardKey(key)];
    
    if (binding.action && input_state == STATE_DEFAULT) {            
        if (action == GLFW_PRESS) {
            Event::Post({Event::KEYDOWN, binding.action, 0, nullptr});
        } else if (action == GLFW_RELEASE) {
            Event::Post({Event::KEYUP, binding.action, 0, nullptr});
        }
    } else if (binding.special_option && action == GLFW_PRESS) {
        binding.special_option();
    }
    
    if (action == GLFW_PRESS) {
        keyboard_keys_values[GLFWKeyToKeyboardKey(key)] = true;
    } else if (action == GLFW_RELEASE) {
        keyboard_keys_values[GLFWKeyToKeyboardKey(key)] = false;
    }
}

static void CharacterCallback(GLFWwindow* window, unsigned int codepoint) {
    auto len = strlen(input_text);
    if (len < input_text_len-1) {
        input_text[len] = codepoint;
        input_text[len+1] = '\0';
    }
}

static void CharacterBackspaceCallback() {
    if (!input_text) return;
    auto len = strlen(input_text);
    if (len > 0) {
        input_text[len-1] = '\0';
    }
}

static void MouseCallback (GLFWwindow* window, double xpos, double ypos) {
    keyboard_axis_values[KEY_MOUSE_X] = xpos;
    keyboard_axis_values[KEY_MOUSE_Y] = ypos;
    
    if (input_state == STATE_DEFAULT) {
        Event::Post({Event::CURSORPOS, 0xFFFF, 0, nullptr});
    }
}

static void MouseKeyCallback (GLFWwindow* window, int button, int action, int mods) {
    KeyCallback(window, button, button, action, mods);
}

static void ScrollCallback (GLFWwindow* window, double xoffset, double yoffset) {
    keyboard_axis_values[KEY_MOUSE_SCROLL] = yoffset;
}


void SetTextInput(char* text, uint32_t len) {
    input_text = text;
    input_text_len = len;
    
    text ?
    glfwSetCharCallback(WINDOW, CharacterCallback):
    glfwSetCharCallback(WINDOW, nullptr);
}

void EndFrame() {
    glfwSwapBuffers(WINDOW);
}

void SetWebMainLoop(void(*loop_function)(void)) {
#ifdef TRAM_SDK_PLATFORM_WEB
    emscripten_set_main_loop(loop_function, 0 , 0);
#else
    std::cout << "SetWebMainLoop() only works with web platform, aborting..." << std::endl;
    abort();
#endif
}

float GetScreenWidth() {
    return screen_width;
}
    
float GetScreenHeight() {
    return screen_height;
}

void SetCursor(CursorType cursor) {
    if (cursor == CURSOR_NONE) {
        glfwSetInputMode(WINDOW, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(WINDOW, cursorx_last, cursory_last);
    } else {
        glfwSetInputMode(WINDOW, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursor(WINDOW, cursors[cursor]);
    }
}

void SetInputState (InputState state) {
    input_state = state;
    
    if (input_state == STATE_MENU_OPEN) {
        cursorx_last = keyboard_axis_values[KEY_MOUSE_X];
        cursory_last = keyboard_axis_values[KEY_MOUSE_Y];
        
        SetCursor(CURSOR_DEFAULT);
    } else {
        SetCursor(CURSOR_NONE);
    }
}

InputState GetInputState () {
    return input_state;
}

/// Checks the state of a key for the current frame.
/// @return True, if key is pressed, false otherwise.
bool PollKeyboardKey (KeyboardKey key) {
    return keyboard_keys_values[key];
}

/// Checks the state of an axis for the current frame.
/// @return Value of the axis.
float PollKeyboardAxis (KeyboardAxis key) {
    return keyboard_axis_values[key];
}

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
