// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <platform/platform.h>
#include <platform/api.h>

#include <framework/core.h>
#include <framework/event.h>
#include <framework/ui.h>
#include <framework/system.h>

#include <render/render.h>
#include <render/api.h>

#include <fstream>
#include <cstring>





using namespace tram::Render;

namespace tram::UI {

static InputState input_state = STATE_DEFAULT;



static float screen_width = 800.0f;
static float screen_height = 600.0f;

static float camera_pitch = 0.0f;
static float camera_yaw = -90.0f;



static bool keyboard_keys_values[KEY_LASTKEY] = {false};
static float keyboard_axis_values[KEY_LASTAXIS] = {0.0f};
static float keyboard_axis_deltas[KEY_LASTAXIS] = {0.0f};

static double cursorx_last = 0.0f, cursory_last = 0.0f;

// static char* input_text = nullptr;
// static uint32_t input_text_len = 0;



struct KeyBinding {
    keyboardaction_t action = KEY_ACTION_NONE;
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

    {KEY_F1, KeyBinding {.special_option = [](){ EXIT = true; /*glfwSetWindowShouldClose(WINDOW, EXIT);*/ }}},
    //{KEY_F5, KeyBinding {.special_option = [](){ THIRD_PERSON = !THIRD_PERSON; }}},
    //{KEY_F6, KeyBinding {.special_option = [](){ THIRD_PERSON = !THIRD_PERSON; }}},
    {KEY_F9, KeyBinding {.special_option = [](){ input_state = (input_state == STATE_FLYING) ? STATE_DEFAULT : STATE_FLYING; }}},
    //{KEY_BACKSPACE, KeyBinding {.special_option = [](){ CharacterBackspaceCallback(); }}}
};

void BindKeyboardKey (KeyboardKey key, keyboardaction_t action) {
    key_action_bindings[key] = {.action = action};
}

void BindKeyboardKey (KeyboardKey key, void (*action)()) {
    key_action_bindings[key] = {.special_option = action};
}

void Init() {
    assert(System::IsInitialized(System::SYSTEM_CORE));
    
    Platform::Window::Init();
    
    System::SetInitialized(System::SYSTEM_UI, true);
}

void Uninit() {
    Platform::Window::Uninit();
}

void Update() {
    if (input_state == STATE_FLYING) {
        vec3 camera_position = GetViewPosition();
        quat camera_rotation = GetViewRotation();
        
        if (keyboard_keys_values[KEY_W])
            camera_position += camera_rotation * DIRECTION_FORWARD * CAMERA_SPEED * GetDeltaTime();
        if (keyboard_keys_values[KEY_S])
            camera_position -= camera_rotation * DIRECTION_FORWARD * CAMERA_SPEED * GetDeltaTime();
        if (keyboard_keys_values[KEY_A])
            camera_position -= camera_rotation * DIRECTION_SIDE * CAMERA_SPEED * GetDeltaTime();
        if (keyboard_keys_values[KEY_D])
            camera_position += camera_rotation * DIRECTION_SIDE * CAMERA_SPEED * GetDeltaTime();
            
        camera_yaw += PollKeyboardAxisDelta(KEY_MOUSE_X) * CAMERA_SENSITIVITY * GetDeltaTime();
        camera_pitch += PollKeyboardAxisDelta(KEY_MOUSE_Y) * CAMERA_SENSITIVITY * GetDeltaTime();
        camera_pitch = camera_pitch > 90.0f ? 90.0f : camera_pitch < -90.0f ? -90.0f : camera_pitch;
        
        SetViewPosition(camera_position);
        SetViewRotation(quat(vec3(-glm::radians(camera_pitch), -glm::radians(camera_yaw), 0.0f)));
    }
    
    // generate keypress events. these need to be generated every tick that the
    // keyboard key is being pressed down.
    if (input_state == STATE_DEFAULT) for (auto& b : key_action_bindings) {
        if (PollKeyboardKey(b.first) && b.second.action) {
            Event::Post({Event::KEYPRESS, b.second.action, 0, nullptr});
        }
    }
    
    keyboard_axis_values[KEY_MOUSE_SCROLL] = 0.0f;
    
    // idk why, but these two lines make the mouse break on emscripten
    // TODO: figure out why and fix
#ifndef __EMSCRIPTEN__
    keyboard_axis_deltas[KEY_MOUSE_X] = 0.0f;
    keyboard_axis_deltas[KEY_MOUSE_Y] = 0.0f;
#endif
    
    Platform::Input::Update();
}




void SetTextInput(char* text, uint32_t len) {
    /*input_text = text;
    input_text_len = len;
    
    text ?
    glfwSetCharCallback(WINDOW, CharacterCallback):
    glfwSetCharCallback(WINDOW, nullptr);*/
}

void EndFrame() {
    Platform::Window::Update();
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

void SetWindowTitle(const char* title) {
    Platform::Window::SetTitle(title);
    
}

void SetWindowSize(int w, int h) {
    Platform::Window::SetSize(w, h);
}

void SetCursor(CursorType cursor) {
    switch (cursor) {
        case CURSOR_NONE:
            Platform::Window::DisableCursor();
            break;
        case CURSOR_DEFAULT:
            Platform::Window::EnableCursor();
            Platform::Window::SetCursor(Platform::Window::CURSOR_DEFAULT);
            break;
        case CURSOR_TEXT:
            Platform::Window::EnableCursor();
            Platform::Window::SetCursor(Platform::Window::CURSOR_TEXT);
            break;
        case CURSOR_CLICK:
            Platform::Window::EnableCursor();
            Platform::Window::SetCursor(Platform::Window::CURSOR_CLICK);
            break;
    };
}

void SetInputState (InputState state) {
    input_state = state;
    
    if (input_state == STATE_MENU_OPEN) {
        cursorx_last = keyboard_axis_values[KEY_MOUSE_X];
        cursory_last = keyboard_axis_values[KEY_MOUSE_Y];
        
        SetCursor(CURSOR_DEFAULT);
    } else {
        keyboard_axis_values[KEY_MOUSE_X] = cursorx_last;
        keyboard_axis_values[KEY_MOUSE_Y] = cursory_last;
        
        SetCursor(CURSOR_NONE);
    }
}

InputState GetInputState() {
    return input_state;
}

/// Checks the state of a key for the current frame.
/// @return True, if key is pressed, false otherwise.
bool PollKeyboardKey(KeyboardKey key) {
    return keyboard_keys_values[key];
}

/// Checks the state of an axis for the current frame.
/// @return Value of the axis.
float PollKeyboardAxis(KeyboardAxis key) {
    return keyboard_axis_values[key];
}

/// Checks the change of an axis for since the last frame.
/// @return Value of the axis delta.
float PollKeyboardAxisDelta(KeyboardAxis key) {
    return keyboard_axis_deltas[key];
}

void KeyPress(KeyboardKey key) {
    const auto& binding = key_action_bindings[key];
    
    if (binding.action && input_state == STATE_DEFAULT) {            
        Event::Post({Event::KEYDOWN, binding.action, 0, nullptr});
    } else if (binding.special_option) {
        binding.special_option();
    }
    
    keyboard_keys_values[key] = true;
}

void KeyRelease(KeyboardKey key) {
    const auto& binding = key_action_bindings[key];
    
    if (binding.action && input_state == STATE_DEFAULT) {            
        Event::Post({Event::KEYUP, binding.action, 0, nullptr});
    }
    
    keyboard_keys_values[key] = false;
}

void KeyMouse(float xpos, float ypos) {
    keyboard_axis_deltas[KEY_MOUSE_X] = xpos - keyboard_axis_values[KEY_MOUSE_X];
    keyboard_axis_deltas[KEY_MOUSE_Y] = ypos - keyboard_axis_values[KEY_MOUSE_Y];
    keyboard_axis_values[KEY_MOUSE_X] = xpos;
    keyboard_axis_values[KEY_MOUSE_Y] = ypos;
    
    if (input_state == STATE_DEFAULT) {
        Event::Post({Event::CURSORPOS, 0xFFFF, 0, nullptr});
    }
}

void KeyScroll(float value) {
    keyboard_axis_values[KEY_MOUSE_SCROLL] = value;
}

void ScreenResize(int width, int height) {
    screen_width = width;
    screen_height = height;
    
    Render::SetScreenSize(width, height);
}

void ScreenClose() {
    EXIT = true;
}

bool ShouldExit() {
    return EXIT;
}

keyboardaction_t RegisterKeyboardAction() {
    static keyboardaction_t last = KEY_ACTION_LAST;
    return last++;
}

}
