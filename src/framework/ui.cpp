// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// UI.CPP -- User interface stuff
// Window opening, opengl context creation and user inputs.

#include <framework/core.h>
#include <framework/event.h>
#include <framework/ui.h>
#include <framework/system.h>

#include <render/render.h>

#include <fstream>
#include <cstring>

#include <glad.h>
//#include <glad_gles3.h>
#include <glfw3.h>

using namespace Core::Render;

namespace Core::UI {
    KeyboardKey glfw_key_to_keyboardkey (int keycode);
    
    InputState INPUT_STATE = STATE_DEFAULT;

    GLFWwindow* WINDOW;
    
    float CAMERA_PITCH = 0.0f;
    float CAMERA_YAW = -90.0f;

    GLFWcursor* cursors[4] = {nullptr};

    bool keyboard_keys_values[KEY_LASTKEY] = {false};
    float keyboard_axis_values[KEY_LASTAXIS] = {0.0f};

    float cursorchangex = 0.0f, cursorchangey = 0.0f;
    double cursorx_last = 0.0f, cursory_last = 0.0f;
    
    char* input_text = nullptr;
    uint32_t input_text_len = 0;

    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    void MouseKeyCallback(GLFWwindow* window, int button, int action, int mods);
    void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    void CharacterBackspaceCallback();

    std::unordered_map<KeyboardKey, KeyBinding> KeyActionBindings = {
        {KEY_W, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_FORWARD}},
        {KEY_S, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_BACKWARD}},
        {KEY_A, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_LEFT}},
        {KEY_D, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_RIGHT}},
        {KEY_SPACE, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_JUMP}},
        {KEY_LEFT_CONTROL, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_CROUCH}},
        {KEY_E, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_ACTIVATE}},

        {KEY_F5, KeyBinding {.type = KeyBinding::SPECIAL_OPTION, .special_option = [](){ THIRD_PERSON = !THIRD_PERSON; }}},
        {KEY_F9, KeyBinding {.type = KeyBinding::SPECIAL_OPTION, .special_option = [](){ INPUT_STATE = (INPUT_STATE == STATE_DEFAULT) ? STATE_FLYING : STATE_DEFAULT; }}},
        {KEY_BACKSPACE, KeyBinding {.type = KeyBinding::SPECIAL_OPTION, .special_option = [](){ CharacterBackspaceCallback(); }}}
    };

    void BindKeyboardKey (KeyboardKey key, KeyBinding binding) {
        KeyActionBindings[key] = binding;
    }

    void Init(){
        assert(System::IsInitialized(System::SYSTEM_CORE));
        
        // start up glfw
        glfwInit();
        
        // this is for opengl 4.0
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        
        // this is for opengl es 3.0
        //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

        glfwWindowHint(GLFW_FOCUSED, GL_FALSE);

        // make a window & context
        WINDOW = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, (const char*)u8"Tramvaju Drifta un Pagrabu Pētīšanas Simulatoru Izstrādes Rīkkopa Versija 0.0.4-alfa", nullptr, nullptr);
        if (WINDOW == nullptr){
            //const char* description;
            //int code = glfwGetError(&description);
            //std::cout << "GLFW window didn't open" << description << std::endl;
            glfwTerminate();
            abort();
        }

        // bind context to this thread
        glfwMakeContextCurrent(WINDOW);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        //if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)){
            std::cout << "OpenGL context didn't open" << std::endl;
            abort();
        }

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        if (FRAME_LIMIT) {
            glfwSwapInterval(1);
        }

        glfwSetFramebufferSizeCallback(WINDOW, [](GLFWwindow* window, int width, int height){
            glViewport(0, 0, width, height);
            Render::ScreenSize(width, height);
        });

        glfwSetKeyCallback(WINDOW, KeyCallback);
        glfwSetCursorPosCallback(WINDOW, MouseCallback);
        glfwSetMouseButtonCallback(WINDOW, MouseKeyCallback);
        glfwSetScrollCallback(WINDOW, ScrollCallback);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        glfwSetWindowSizeLimits(WINDOW, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);

        glfwSetInputMode(WINDOW, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


        cursors[CURSOR_DEFAULT] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        cursors[CURSOR_TEXT] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        cursors[CURSOR_CLICK] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        
        System::SetInitialized(System::SYSTEM_UI, true);
    }

    void Uninit(){
        glfwTerminate();
    }

    void Update(){
        if(glfwGetKey(WINDOW, GLFW_KEY_F1) == GLFW_PRESS) glfwSetWindowShouldClose(WINDOW, true);
        EXIT = glfwWindowShouldClose(WINDOW);

        if (INPUT_STATE == STATE_FLYING){
            if (glfwGetKey(WINDOW, GLFW_KEY_W) == GLFW_PRESS)
                CAMERA_POSITION += CAMERA_ROTATION * DIRECTION_FORWARD * CAMERA_SPEED;
            if (glfwGetKey(WINDOW, GLFW_KEY_S) == GLFW_PRESS)
                CAMERA_POSITION -= CAMERA_ROTATION * DIRECTION_FORWARD * CAMERA_SPEED;
            if (glfwGetKey(WINDOW, GLFW_KEY_A) == GLFW_PRESS)
                CAMERA_POSITION -= CAMERA_ROTATION * DIRECTION_SIDE * CAMERA_SPEED;
            if (glfwGetKey(WINDOW, GLFW_KEY_D) == GLFW_PRESS)
                CAMERA_POSITION += CAMERA_ROTATION * DIRECTION_SIDE * CAMERA_SPEED;
                
            cursorchangex = keyboard_axis_values[KEY_MOUSE_X] - cursorx_last;
            cursorchangey = keyboard_axis_values[KEY_MOUSE_Y] - cursory_last;
            cursorx_last = keyboard_axis_values[KEY_MOUSE_X];
            cursory_last = keyboard_axis_values[KEY_MOUSE_Y];

            CAMERA_YAW += cursorchangex * CAMERA_SENSITIVITY;
            CAMERA_PITCH += cursorchangey * CAMERA_SENSITIVITY;
            CAMERA_PITCH = CAMERA_PITCH > 90.0f ? 90.0f : CAMERA_PITCH < -90.0f ? -90.0f : CAMERA_PITCH;
            CAMERA_ROTATION = glm::quat(glm::vec3(-glm::radians(CAMERA_PITCH), -glm::radians(CAMERA_YAW), 0.0f));
        }

        
        // update camera rotation
        // TODO: move this into somewhere else
        if (INPUT_STATE != STATE_MENU_OPEN) {

        }

        static InputState input_state_last = STATE_DEFAULT;
        if (input_state_last != STATE_MENU_OPEN && INPUT_STATE == STATE_MENU_OPEN) {
            SetCursor(CURSOR_DEFAULT);
        } else if (input_state_last != STATE_DEFAULT && INPUT_STATE == STATE_DEFAULT) {
            SetCursor(CURSOR_NONE);
        }
        input_state_last = INPUT_STATE;
    }

    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        const auto& binding = KeyActionBindings[glfw_key_to_keyboardkey(key)];
        
        if (binding.type == KeyBinding::KEYBOARD_ACTION && INPUT_STATE == STATE_DEFAULT) {            
            if (action == GLFW_PRESS) {
                Event::Post({Event::KEYDOWN, binding.action, 0, nullptr});
            } else if (action == GLFW_RELEASE) {
                Event::Post({Event::KEYUP, binding.action, 0, nullptr});
            }
        } else if (binding.type == KeyBinding::SPECIAL_OPTION && action == GLFW_PRESS) {
            binding.special_option();
        }
        
        if (action == GLFW_PRESS) {
            keyboard_keys_values[glfw_key_to_keyboardkey(key)] = true;
        } else if (action == GLFW_RELEASE) {
            keyboard_keys_values[glfw_key_to_keyboardkey(key)] = false;
        }
    }
    
    void CharacterCallback(GLFWwindow* window, unsigned int codepoint) {
        auto len = strlen(input_text);
        if (len < input_text_len-1) {
            input_text[len] = codepoint;
            input_text[len+1] = '\0';
        }
    }
    
    void CharacterBackspaceCallback() {
        if (!input_text) return;
        auto len = strlen(input_text);
        if (len > 0) {
            input_text[len-1] = '\0';
        }
    }
    
    /// Callback for mouse pointer/cursor movement.
    void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
        keyboard_axis_values[KEY_MOUSE_X] = xpos;
        keyboard_axis_values[KEY_MOUSE_Y] = ypos;
        
        if (INPUT_STATE == STATE_DEFAULT) {
            Event::Post({Event::CURSORPOS, 0xFFFF, 0, nullptr});
        }
    }
    
    /// Callback for mouse button presses/clicks.
    void MouseKeyCallback(GLFWwindow* window, int button, int action, int mods) {
        KeyCallback(window, button, button, action, mods);
    }
    
    /// Callback for mouse scrollwheel scrolling.
    void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        keyboard_axis_values[KEY_MOUSE_SCROLL] = yoffset;
    }

    
    void SetTextInput(char* text, uint32_t len) {
        input_text = text;
        input_text_len = len;
        
        text ?
        glfwSetCharCallback(WINDOW, CharacterCallback):
        glfwSetCharCallback(WINDOW, nullptr);
    }

    void EndFrame(){
        keyboard_axis_values[KEY_MOUSE_SCROLL] = 0.0f;
        
        glfwSwapBuffers(WINDOW);
        glfwPollEvents();
    }
    
    double GetTime() {
        return glfwGetTime();
    }

    void SetCursor(CursorType cursor){
        if (cursor == CURSOR_NONE) {
            glfwSetInputMode(WINDOW, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetCursorPos(WINDOW, cursorx_last, cursory_last);
        } else {
            glfwSetInputMode(WINDOW, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetCursor(WINDOW, cursors[cursor]);
        }
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
    KeyboardKey glfw_key_to_keyboardkey (int keycode) {
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
