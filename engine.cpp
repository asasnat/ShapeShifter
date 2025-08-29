#include "engine.hpp"
#include <iostream>
#include <SDL3/SDL.h>
#include "param.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif



Engine::Engine()
{
    m_mouse = { 0 };
    for (int i = 0; i < SDL_SCANCODE_COUNT; i++) {
        m_keyboard[i] = { 0 };
    }
}

void Engine::run()
{
    init();

    while (m_running) {
        tick();
    }

    deinit();
}

void Engine::tick()
{
    pollEvents();
    update();
    render();
}

void Engine::init()
{
    bool status = true;
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");
    if (!SDL_CreateWindowAndRenderer("olc::CodeJam 2025 - Shape Shifter", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &m_window, &m_renderer)) {
        std::cerr << "Failed to create window and renderer: " << SDL_GetError() << std::endl;
        status = false;
    }

    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(m_window);

    float mainScale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
#ifdef __EMSCRIPTEN__
    io.IniFilename = nullptr;
    #endif

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(mainScale);
    style.FontScaleDpi = mainScale;

    ImGui_ImplSDL3_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer3_Init(m_renderer);

    m_game = std::make_unique<Game>(m_renderer);

    m_running = status;
}

void Engine::update()
{
    static Uint64 prevTime = 0, currTime;
    currTime = SDL_GetTicks();
    float dt = (currTime - prevTime) / 1000.f;

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    m_game->update(dt, m_mouse, m_keyboard);

    prevTime = currTime;
}

void Engine::pollEvents()
{
    for (int i = 0; i < SDL_SCANCODE_COUNT; i++) {
        m_keyboard[i].pressed = false;
        m_keyboard[i].released = false;
    }

    for (int i = 0; i < 3; i++) {
        m_mouse.buttons[i].pressed = false;
        m_mouse.buttons[i].released = false;
    }

    m_mouse.scrolled = false;

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL3_ProcessEvent(&e);
        switch (e.type) {
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            m_running = false;
            break;

        case SDL_EVENT_KEY_DOWN:
            //if (e.key.repeat) break;
            m_keyboard[e.key.scancode].pressed = true;
            m_keyboard[e.key.scancode].held = true;

            //if (e.key.key == SDLK_A) std::cout << "fucka you" << std::endl;
            break;

        case SDL_EVENT_KEY_UP:
            m_keyboard[e.key.scancode].held = false;
            m_keyboard[e.key.scancode].released = true;
            break;

        case SDL_EVENT_MOUSE_MOTION:
            m_mouse.x = e.motion.x;
            m_mouse.y = e.motion.y;
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (e.button.button > 3 || e.button.button < 1) break;
            m_mouse.buttons[e.button.button - 1].pressed = true;
            m_mouse.buttons[e.button.button - 1].held = true;
            break;

        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (e.button.button > 3 || e.button.button < 1) break;
            m_mouse.buttons[e.button.button - 1].held = false;
            m_mouse.buttons[e.button.button - 1].released = true;
            break;

        case SDL_EVENT_MOUSE_WHEEL:
            m_mouse.scrolled = true;
            m_mouse.scrollDelta = e.wheel.y;
            break;

        default:
            break;
        }
    }
}

void Engine::render()
{
    m_game->render();
}

void Engine::deinit()
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();



    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
}
