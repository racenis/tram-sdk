// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// UI.CPP -- User interface stuff
// Window opening, opengl context creation and user inputs.

#include <framework/core.h>
#include <framework/event.h>
#include <framework/ui.h>

#include <render/render.h>

#include <fstream>
#include <cstring>

#include <glad.h>
//#include <glad_gles3.h>
#include <glfw3.h>

using namespace Core::Render;

namespace Core::UI {

    InputState INPUT_STATE = STATE_DEFAULT;

    GLFWwindow* WINDOW;

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

    std::unordered_map<int, KeyAction> KeyActionBindings = {
        {GLFW_KEY_W, KeyAction {.type = KeyAction::KEYBOARD_KEY, .action = KEY_FORWARD}},
        {GLFW_KEY_S, KeyAction {.type = KeyAction::KEYBOARD_KEY, .action = KEY_BACKWARD}},
        {GLFW_KEY_A, KeyAction {.type = KeyAction::KEYBOARD_KEY, .action = KEY_LEFT}},
        {GLFW_KEY_D, KeyAction {.type = KeyAction::KEYBOARD_KEY, .action = KEY_RIGHT}},
        {GLFW_KEY_SPACE, KeyAction {.type = KeyAction::KEYBOARD_KEY, .action = KEY_JUMP}},
        {GLFW_KEY_LEFT_CONTROL, KeyAction {.type = KeyAction::KEYBOARD_KEY, .action = KEY_CROUCH}},
        {GLFW_KEY_E, KeyAction {.type = KeyAction::KEYBOARD_KEY, .action = KEY_ACTIVATE}},

        {GLFW_KEY_F5, KeyAction {.type = KeyAction::SPECIAL_OPTION, .special_option = [](){ THIRD_PERSON = !THIRD_PERSON; }}},
        {GLFW_KEY_F4, KeyAction {.type = KeyAction::SPECIAL_OPTION, .special_option = [](){ DRAW_PHYSICS_DEBUG = !DRAW_PHYSICS_DEBUG; }}},
        {GLFW_KEY_F9, KeyAction {.type = KeyAction::SPECIAL_OPTION, .special_option = [](){ INPUT_STATE = (INPUT_STATE == STATE_DEFAULT) ? STATE_FLYING : STATE_DEFAULT; }}},
        {GLFW_KEY_ESCAPE, KeyAction {.type = KeyAction::SPECIAL_OPTION, .special_option = [](){ ToggleMenu(); escape_menu_open || debug_menu_open ? escape_menu_open = false, debug_menu_open = false : escape_menu_open = true; }}},
        {GLFW_KEY_GRAVE_ACCENT, KeyAction {.type = KeyAction::SPECIAL_OPTION, .special_option = [](){ ToggleMenu(); escape_menu_open || debug_menu_open ? escape_menu_open = false, debug_menu_open = false : debug_menu_open = true; }}},
        {GLFW_KEY_BACKSPACE, KeyAction {.type = KeyAction::SPECIAL_OPTION, .special_option = [](){ CharacterBackspaceCallback(); }}}
    };

    void Init(){
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
        WINDOW = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, (const char*)u8"Tramvaju Drifta un Pagrabu Pētīšanas Simulatoru Izstrādes Rīkkopa Versija 0.0.2-alfa", nullptr, nullptr);
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
    }

    void Uninit(){
        glfwTerminate();
    }

    void Update(){

        FRAME_TIME = glfwGetTime();

        if(glfwGetKey(WINDOW, GLFW_KEY_F1) == GLFW_PRESS) glfwSetWindowShouldClose(WINDOW, true);
        SHOULD_CLOSE = glfwWindowShouldClose(WINDOW);

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

        // generate events from keypresses
        if (INPUT_STATE == STATE_DEFAULT){
            keys_pressed ^= keys_up;
            keys_pressed |= keys_down;

            if (keys_pressed) {Event kpress = {Event::KEYPRESS, keys_pressed, 0, nullptr}; Event::Post(kpress);}
            if (keys_down) {Event kpress = {Event::KEYDOWN, keys_down, 0, nullptr}; Event::Post(kpress);}
            if (keys_up) {Event kpress = {Event::KEYUP, keys_up, 0, nullptr}; Event::Post(kpress);}

            keys_down = 0;
            keys_up = 0;
        }

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
        if (KeyActionBindings[key].type == KeyAction::KEYBOARD_KEY) {
            if (action == GLFW_PRESS) {
                keys_down |= KeyActionBindings[key].action;
            } else if (action == GLFW_RELEASE) {
                keys_up |= KeyActionBindings[key].action;
            }
        } else if (KeyActionBindings[key].type == KeyAction::SPECIAL_OPTION && action == GLFW_PRESS) {
            KeyActionBindings[key].special_option();
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

}
