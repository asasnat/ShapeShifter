#pragma once
#include <SDL3/SDL_render.h>
#include "map.hpp"
#include "render/map_renderer.hpp"
#include "game.hpp"

#include <memory>

class Engine {
    friend void mainloop();
    friend int main(int argc, char** argv);
private:
    bool m_running;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    std::unique_ptr<Game> m_game;

    ButtonState m_keyboard[SDL_SCANCODE_COUNT];
    MouseState m_mouse;

private:
    void init();
    void pollEvents();
    void update();
    void render();
    void deinit();

    void tick();

    void stop() { m_running = false; }

public:
    Engine();

    void run();
};
