#pragma once
#include <SDL3/SDL_render.h>
#include <memory>
#include "map.hpp"
#include "controls.hpp"
#include "camera.hpp"

class MapRenderer;

class Game {
private:
    Map m_map;
    std::unique_ptr<MapRenderer> m_mapRenderer;
    SDL_Renderer* const& m_renderer;
    SDL_Cursor* m_scrollCursor;
    SDL_Cursor* m_defaultCursor;

    Camera m_camera;
    SDL_Rect m_viewport;

    SDL_FPoint m_scrollAnchor;
    bool m_scrolling = false;

public:
    Game(SDL_Renderer* const& renderer);
    ~Game();

public:
    void update(float dt, MouseState mouse, ButtonState* keyboard);
    void render();
};
