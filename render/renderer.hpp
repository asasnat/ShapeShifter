#pragma once
#include <SDL3/SDL_render.h>

class Renderer {
protected:
    SDL_Renderer* const& m_renderer;

    Renderer(SDL_Renderer* const& renderer);

public:
    virtual void render() = 0;
};
