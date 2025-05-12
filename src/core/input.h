#pragma once

#include "defines.h"

typedef enum {
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_MIDDLE,

    BUTTON_MAX_BUTTONS
} Button_Type;

typedef struct {
    s16 x, y;
    s16 dx, dy;
    bool has_movement;
    u8 buttons[BUTTON_MAX_BUTTONS];
} Mouse_State;

typedef struct {
    bool keys[256];
} Keyboard_State;

typedef struct {
    Mouse_State mouse_curr;
    Mouse_State mouse_prev;
    Keyboard_State keyboard_curr;
    Keyboard_State keyboard_prev;
    bool initialized;
    bool enabled;
} Input_State;

void input_init();
void input_shutdown();
void input_update();

void input_set_relative_mouse_mode(bool yes);
void input_process_mouse_move(s16 x, s16 y);
void input_process_key(u8 key_code, bool pressed);

Input_State *input_get_state();
void input_get_mouse_delta(s16 *x, s16 *y);

bool input_is_key_down(u8 key_code);
bool input_is_key_up(u8 key_code);
bool input_was_key_down(u8 key_code);
bool input_was_key_up(u8 key_code);

bool input_is_key_pressed(u8 key_code);
bool input_is_key_released(u8 key_code);
