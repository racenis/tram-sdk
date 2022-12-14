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

    bool escape_menu_open = false;
    bool debug_menu_open = false;

    // TODO: fix this
    FontCharInfo fontinfo[4][256] = {0.0f};
    GlyphCharInfo glyphinfo[4][256] = {0.0f};
    FontCharInfo* fontinfo_f = (FontCharInfo*)fontinfo;

    bool ismouse_left = false;
    bool isnotmouse_left = false;
    bool wasmouse_left = false;
    float mouse_scroll = 0.0f;

    float cur_x, cur_y;

    double cursorx = 0.0f, cursory = 0.0f;
    float cursorchangex = 0.0f, cursorchangey = 0.0f;
    double cursorx_last = 0.0f, cursory_last = 0.0f;
    
    char* input_text = nullptr;
    uint32_t input_text_len = 0;

    // these accumulate the codes from the keypress callback events 
    uint16_t keys_down = 0;
    uint16_t keys_up = 0;
    uint16_t keys_pressed = 0;

    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    void CharacterBackspaceCallback();
    
    void ToggleMenu() { INPUT_STATE = (INPUT_STATE == STATE_DEFAULT) ? STATE_MENU_OPEN : STATE_DEFAULT; }

    std::unordered_map<KeyboardKey, KeyBinding> KeyActionBindings = {
        {KEY_W, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_FORWARD}},
        {KEY_S, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_BACKWARD}},
        {KEY_A, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_LEFT}},
        {KEY_D, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_RIGHT}},
        {KEY_SPACE, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_JUMP}},
        {KEY_LEFT_CONTROL, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_CROUCH}},
        {KEY_E, KeyBinding {.type = KeyBinding::KEYBOARD_ACTION, .action = KEY_ACTION_ACTIVATE}},

        {KEY_F5, KeyBinding {.type = KeyBinding::SPECIAL_OPTION, .special_option = [](){ THIRD_PERSON = !THIRD_PERSON; }}},
        //{KEY_F4, KeyBinding {.type = KeyBinding::SPECIAL_OPTION, .special_option = [](){ DRAW_PHYSICS_DEBUG = !DRAW_PHYSICS_DEBUG; }}},
        {KEY_F9, KeyBinding {.type = KeyBinding::SPECIAL_OPTION, .special_option = [](){ INPUT_STATE = (INPUT_STATE == STATE_DEFAULT) ? STATE_FLYING : STATE_DEFAULT; }}},
        {KEY_ESCAPE, KeyBinding {.type = KeyBinding::SPECIAL_OPTION, .special_option = [](){ ToggleMenu(); escape_menu_open || debug_menu_open ? escape_menu_open = false, debug_menu_open = false : escape_menu_open = true; }}},
        {KEY_GRAVE_ACCENT, KeyBinding {.type = KeyBinding::SPECIAL_OPTION, .special_option = [](){ ToggleMenu(); escape_menu_open || debug_menu_open ? escape_menu_open = false, debug_menu_open = false : debug_menu_open = true; }}},
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
        WINDOW = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, (const char*)u8"Tramvaju Drifta un Pagrabu P??t????anas Simulatoru Izstr??des R??kkopa Versija 0.0.4-alfa", nullptr, nullptr);
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
            //SCREEN_WIDTH = width;
            //SCREEN_HEIGHT = height;
            Render::ScreenSize(width, height);
        });

        glfwSetKeyCallback(WINDOW, KeyCallback);
        glfwSetScrollCallback(WINDOW, ScrollCallback);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        glfwSetWindowSizeLimits(WINDOW, 640, 480, GLFW_DONT_CARE, GLFW_DONT_CARE);

        glfwSetInputMode(WINDOW, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        //glfwSetInputMode(WINDOW, GLFW_STICKY_KEYS, GLFW_TRUE);


        cursors[CURSOR_DEFAULT] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        cursors[CURSOR_TEXT] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        cursors[CURSOR_CLICK] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        
        LoadFontInfo("data/jost.info", 0);
        LoadFontInfo("data/inter.info", 1);
        LoadGlyphInfo("data/glyph.info", 0);
        LoadGlyphInfo("data/glyph_text.info", 1);
        LoadGlyphInfo("data/glyph_text_bold.info", 2);
        
        System::SetInitialized(System::SYSTEM_UI, true);
    }

    void Uninit(){
        glfwTerminate();
    }

    void Update(){
        TIME = glfwGetTime();

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
        }

        bool mouse_status_left =  glfwGetMouseButton(WINDOW, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        ismouse_left = !wasmouse_left && mouse_status_left;
        isnotmouse_left = wasmouse_left && !mouse_status_left;
        wasmouse_left = mouse_status_left;

        // generate cursor change position event
        glfwGetCursorPos(WINDOW, &cursorx, &cursory);
        if (INPUT_STATE == STATE_DEFAULT && (cursorx != cursorx_last || cursory != cursory_last)) {
            Event mousse = {Event::CURSORPOS, 0xFFFF, 0, nullptr};
            Event::Post(mousse);
        }

        // update camera rotation
        if (INPUT_STATE != STATE_MENU_OPEN) {
            cursorchangex = cursorx - cursorx_last;
            cursorchangey = cursory - cursory_last;
            cursorx_last = cursorx;
            cursory_last = cursory;

            CAMERA_YAW += cursorchangex * CAMERA_SENSITIVITY;
            CAMERA_PITCH += cursorchangey * CAMERA_SENSITIVITY;
            CAMERA_PITCH = CAMERA_PITCH > 90.0f ? 90.0f : CAMERA_PITCH < -90.0f ? -90.0f : CAMERA_PITCH;
        }

        CAMERA_ROTATION = glm::quat(glm::vec3(-glm::radians(CAMERA_PITCH), -glm::radians(CAMERA_YAW), 0.0f));

        cur_x = cursorx;
        cur_y = cursory;

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
    
    void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
        mouse_scroll = yoffset;
    }

    
    void SetTextInput(char* text, uint32_t len) {
        input_text = text;
        input_text_len = len;
        
        text ?
        glfwSetCharCallback(WINDOW, CharacterCallback):
        glfwSetCharCallback(WINDOW, nullptr);
    }

    void EndFrame(){

        mouse_scroll = 0.0f;
        
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

    void LoadFontInfo(const char* filename, size_t fontIndex){
        std::ifstream file;
        file.open(filename);
        while(file){
            uint32_t index;
            float left, right, top, bottom, drop;
            file >> index;
            file >> left;
            file >> bottom;
            file >> right;
            file >> top;
            file >> drop;

            fontinfo[fontIndex][index].left = left;
            fontinfo[fontIndex][index].bottom = 256.0f - top;
            fontinfo[fontIndex][index].right = right - left;
            fontinfo[fontIndex][index].top = top - bottom;
            fontinfo[fontIndex][index].drop = drop;
        }
        file.close();
    }
    
    void LoadGlyphInfo(const char* filename, size_t fontIndex){
        std::ifstream file;
        file.open(filename);
        while(file){
            uint32_t index;
            float x, y, w, h, drop;
            file >> index;
            file >> x;
            file >> y;
            file >> w;
            file >> h;
            file >> drop;

            glyphinfo[fontIndex][index].x = x;
            glyphinfo[fontIndex][index].y = y;
            glyphinfo[fontIndex][index].w = w;
            glyphinfo[fontIndex][index].h = h;
            glyphinfo[fontIndex][index].drop = drop;
        }
        file.close();
    }

    void SetText(const char* text, uint32_t x, uint32_t y, float scale, float width, bool justify, bool stretch, uint32_t font, const glm::vec3& color){
        const float CHAR_W = 16.0f;     // the width & height for the characters in ui coordinates
        const float CHAR_H = CHAR_W;    // don't touch these (unless you plan on re-generating all of the ui textures)
        const float MIN_SPACE = 4.0f;   // a space will never be smaller than this.

        float line_offset = y;
        float line_width = 0.0f;
        float line_scale_w = scale;
        float line_space_w = 0.0f;
        size_t line_spaces = 0;
        bool line_ready = false;

        float thickness = 1.0f;

        uint8_t line [400]; // set the length to whatever, but 200 probably will be enough for any line

        bool break_lines;       // break lines when exceeding width
        break_lines = (width != 0.0f && !stretch) || (stretch && justify);

        uint8_t* c = (uint8_t*)text;
        uint8_t* l = line;

        // what happens if the *c == 0? return function?
        // TODO:  check

        while (1){
            if (*c == ' '){
                line_width += MIN_SPACE * scale;
                line_spaces++;
                c++;
                *l = ' ';
                l++;
                continue;
            } else if (*c == '%'){
                // special commands
                c++;

                if(*c == '&' && false){ // && false to disable the code
                    /*
                    // getting a value
                    char name[100];
                    size_t len = 0;

                    c++;
                    while (*c != ' ' && *c != '\t' && *c != '\0'){
                        name[len] = *c;
                        len++;
                        c++;
                    }

                    name[len] = '\0';

                    ParserValue* pval = FindValuePtr(UID(name));

                    if(pval){
                        pval->GetValue((char*)l);
                        while(*l != '\0') l++;
                    }*/
                } else {
                    c--;
                }
            }

            *l = *c;
            line_width += fontinfo[font][*l].right * scale;

            c++;
            l++;

            if(break_lines && line_width > width){
                // go back to last space (if there is one)
                if(line_spaces > 0){
                    while (*c != ' ') c--;
                    while (*l != ' '){
                        line_width -= fontinfo[font][*l].right * scale;
                        l--;
                    }
                    *l = 0;              // mark the end of line
                    line_ready = true;
                }
            }



            if(line_ready || *c == 0){
                *l = 0;
                l = line;

                if(justify && stretch){
                    line_space_w = MIN_SPACE;
                    line_width = x + ((width - line_width) / 2);
                    line_scale_w = scale;
                } else if (justify){
                    line_width = line_width - (MIN_SPACE * line_spaces);
                    line_space_w = (width - line_width) / line_spaces;
                    line_width = x;
                    line_scale_w = scale;
                } else if (stretch){
                    line_scale_w = width / line_width * scale;
                    line_space_w = MIN_SPACE * line_scale_w;
                    line_width = x;
                } else {
                    line_scale_w = scale;
                    line_space_w = MIN_SPACE;
                    line_width = x;
                }

                while (*l != 0){
                    if (*l == ' '){
                        line_width += line_space_w;
                        l++;
                        continue;
                    } else if (*l == '%'){
                        l++;
                        if (*l == 'b'){
                            thickness = 1.0f + 0.5f * scale;
                            l++;
                        } else if (*l == 'r') {
                            thickness = 1.0f;
                            l++;
                        } else l--;
                    }
                    SetGlyph(line_width, line_offset + ((16.0f - fontinfo[font][*l].top - fontinfo[font][*l].drop) * scale), fontinfo[font][*l].left, fontinfo[font][*l].bottom, fontinfo[font][*l].right, fontinfo[font][*l].top, line_scale_w, scale, thickness, font, color);
                    line_width += fontinfo[font][*l].right * line_scale_w;
                    l++;
                }

                if(*c == 0) return;

                //reset


                l = line;
                line_width = 0.0f;
                line_offset += CHAR_H * scale * 1.5f;
                line_spaces = 0;
                line_space_w = 0.0f;
                line_scale_w = scale;
                line_ready = false;
            }


        }

    }

    void SetDebugText(const char* text, const glm::vec3& location, const glm::vec3& color){
        //glm::vec3 screen_pos = glm::project(location, Render::matrices.view, Render::matrices.projection, glm::vec4 (0.0f, 0.0f, 640.0f, 480.0f));
        //Core::UI::SetText(text, screen_pos.x-100.0f, 480.0f + 25.0f - screen_pos.y, 0.7f, 200.0f, 1, 1, 1, color);
    }
    
    
    KeyboardKey glfw_key_to_keyboardkey (int keycode) {
        switch (keycode) {
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
