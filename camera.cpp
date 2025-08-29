#include "camera.hpp"
#include "param.hpp"

SDL_Rect Camera::getViewport() const
{
    SDL_Rect viewport;

    float tx, ty;
    tx = -x + (WINDOW_WIDTH / 2 / zoom);
    ty = -y + (WINDOW_HEIGHT / 2 / zoom);

    viewport.x = tx;
    viewport.y = ty;
    viewport.w = 1280;
    viewport.h = 960;

    return viewport;
}

float Camera::getScale() const
{
    return zoom;
}
