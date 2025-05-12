#include "input.h"

#include <stdio.h>
#include <string.h>

static Input_State state = {0};

void input_init() {
    memset(&state, 0, sizeof(Input_State));
    state.initialized = true;

    printf("Input system initialized.\n");
}

void input_shutdown() {
    state.initialized = false;
}

void input_update() {
    if (!state.initialized) return;

    // printf("curr f = %d, prev f = %d\n", state.keyboard_curr.keys['f'], state.keyboard_prev.keys['f']);
    memcpy(&state.keyboard_prev, &state.keyboard_curr, sizeof(Keyboard_State));
    memcpy(&state.mouse_prev, &state.mouse_curr, sizeof(Mouse_State));
}

void input_process_mouse_move(s16 x, s16 y) {
    if (state.mouse_curr.x != x || state.mouse_curr.y != y) {
        state.mouse_curr.x += x;
        state.mouse_curr.y += y;
    }
}

Input_State *input_get_state() {
    return &state;
}

void input_get_mouse_delta(s16 *x, s16 *y) {
    *x = state.mouse_curr.x - state.mouse_prev.x;
    *y = state.mouse_curr.y - state.mouse_prev.y;
}

void input_process_key(u8 key_code, bool pressed) {
    state.keyboard_curr.keys[key_code] = pressed;
    // if (state.keyboard_curr.keys[key_code] != pressed) {
    // }
}

bool input_is_key_down(u8 key_code) {
    return state.keyboard_curr.keys[key_code] == true;
}
bool input_is_key_up(u8 key_code) {
    return state.keyboard_curr.keys[key_code] == false;
}
bool input_was_key_down(u8 key_code) {
    return state.keyboard_prev.keys[key_code] == true;
}
bool input_was_key_up(u8 key_code) {
    return state.keyboard_prev.keys[key_code] == false;
}
bool input_is_key_pressed(u8 key_code) {
    return state.keyboard_curr.keys[key_code] == true && state.keyboard_prev.keys[key_code] == false;
}
bool input_is_key_released(u8 key_code) {
    return state.keyboard_curr.keys[key_code] == false && state.keyboard_prev.keys[key_code] == true;
}
