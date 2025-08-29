#pragma once
#include <SDL3/SDL_rect.h>

struct Camera {
    float x, y;
    float zoom;

    SDL_Rect getViewport() const;
    float getScale() const;
};
