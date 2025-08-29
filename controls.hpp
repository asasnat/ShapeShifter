#pragma once
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_scancode.h>

struct ButtonState {
    bool pressed, released, held;
};

struct MouseState {
    float x, y;
    ButtonState buttons[3];
    bool scrolled;
    float scrollDelta;
};


