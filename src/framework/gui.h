// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_GUI_H
#define TRAM_SDK_FRAMEWORK_GUI_H

#include <framework/ui.h>

namespace tram::Render {
    class Sprite;
}

namespace tram::GUI {

typedef uint32_t font_t;
typedef uint32_t glyph_t;

enum orientation : uint32_t {
    TEXT_LEFT,
    TEXT_CENTER,
    TEXT_RIGHT,
    TEXT_JUSTIFIED,
    
    LINE_LOW,
    LINE_NORMAL,
    LINE_HIGH,
    
    FRAME_LEFT,
    FRAME_RIGHT,
    FRAME_TOP,
    FRAME_BOTTOM,
    FRAME_LEFT_INV,
    FRAME_RIGHT_INV,
    FRAME_TOP_INV,
    FRAME_BOTTOM_INV,
    FRAME_INSET,
    FRAME_CENTER_VERTICAL,
    FRAME_CENTER_HORIZONTAL
};

void Init();
void Update();
font_t RegisterFont(Render::Sprite* sprite);

void SetScaling(uint32_t);
uint32_t GetScaling();

bool ClickHandled();

void Begin();
void End();

void SetColor(Render::color_t color);
void FillFrame(font_t font, glyph_t glyph);
void HorizontalDivider();
void NewLine(uint32_t line = LINE_NORMAL);
bool CheckBox(bool& selected, const char* text, bool enabled = true);
bool RadioButton(uint32_t index, uint32_t& selected, const char* text, bool enabled = true);
bool Button(const char* text, bool enabled = true, uint32_t width = 0);
bool Slider(float& value, bool enabled = true, uint32_t width = 0);
void PopFrame();
void PopFrameKeepCursor(bool = false, bool = true);
void PushFrameRelative(uint32_t orientation, uint32_t offset);
void PushFrameRelativeKeepCursor(uint32_t orientation, uint32_t offset, bool = false, bool = true);
void PushFrame(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void Glyph(font_t font, glyph_t glyph);
void Text(font_t font, const char* text, uint32_t orientation = TEXT_LEFT);
bool TextBox(char* text, uint32_t length, bool enabled = true, uint32_t w = 0, uint32_t h = 0);
void TextBox(const char* text, uint32_t w = 0, uint32_t h = 0);

void SetSelectedText(char* text);

enum widget : glyph_t {
    WIDGET_TOP_LEFT = 0,
    WIDGET_TOP_MIDDLE = 1,
    WIDGET_TOP_RIGHT = 2,
    WIDGET_MIDDLE_LEFT = 3,
    WIDGET_MIDDLE_MIDDLE = 4,
    WIDGET_MIDDLE_RIGHT = 5,
    WIDGET_BOTTOM_LEFT = 6,
    WIDGET_BOTTOM_MIDDLE = 7,
    WIDGET_BOTTOM_RIGHT = 8,
    
    WIDGET_LEFT = 0,
    WIDGET_MIDDLE = 1,
    WIDGET_RIGHT = 2,
    
    WIDGET_BUTTON = 0,
    WIDGET_BUTTON_PRESSED = 9,
    WIDGET_BUTTON_SELECTED_ENABLED = 18,
    WIDGET_BUTTON_SELECTED_DISABLED = 27,
    WIDGET_BUTTON_DISABLED = 36,
    WIDGET_BUTTON_PRESSED_DISABLED = 45,
    WIDGET_WINDOW = 54,
    WIDGET_REVERSE_WINDOW = 63,
    WIDGET_SELECT_BOX = 72,
    WIDGET_SELECT_BOX_PRESSED = 81,
    WIDGET_SELECT_BOX_SELECTED = 90,
    WIDGET_TEXT_BOX = 99,
    WIDGET_TEXT_BOX_DISABLED = 108,
    WIDGET_BORDER = 117,
    WIDGET_SCROLL_BAR_TRACK_VERTICAL = 126,
    WIDGET_SCROLL_BAR_TRACK_HORIZONTAL = 129,
    WIDGET_DIVIDER_HORIZONTAL = 132,
    WIDGET_DIVIDER_VERTICAL = 135,
    WIDGET_SLIDER_TRACK_VERTICAL = 138,
    WIDGET_SLIDER_TRACK_HORIZONTAL = 138,
    WIDGET_SCROLL_BAR_VERTICAL = 144,
    WIDGET_SCROLL_BAR_HORIZONTAL = 147,
    WIDGET_TAB = 150,
    WIDGET_PROGESS_BAR = 153,
    WIDGET_RADIO_BUTTON = 156,
    WIDGET_CHECK_BUTTON = 162,
    WIDGET_SLIDER_VERTICAL = 168,
    WIDGET_SLIDER_HORIZONTAL = 174,
    WIDGET_BUTTON_UP = 180,
    WIDGET_BUTTON_DOWN = 186,
    WIDGET_BUTTON_LEFT = 192,
    WIDGET_BUTTON_RIGHT = 198,
    WIDGET_BUTTON_EXIT = 204,
    WIDGET_BUTTON_HELP = 210,
    WIDGET_BUTTON_2_WINDOW = 216,
    WIDGET_BUTTON_1_WINDOW = 222,
    WIDGET_DEFAULT = 255,
};

}

#endif // TRAM_SDK_FRAMEWORK_GUI_H