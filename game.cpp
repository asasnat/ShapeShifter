#include "game.hpp"
#include "render/map_renderer.hpp"
#include <iostream>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_sdlrenderer3.h"
#include "base64/base64.hpp"
using namespace boost::beast::detail;
#include "param.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Game::Game(SDL_Renderer* const& renderer)
    : m_renderer(renderer), m_camera({ 0 }), m_viewport({ 0 }), m_map(1337)
{
    m_mapRenderer = std::make_unique<MapRenderer>(renderer, m_map);

    m_scrollCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);

    m_defaultCursor = SDL_GetDefaultCursor();

    m_camera.x = 640;
    m_camera.y = 480 + 128;
    m_camera.zoom = 0.5f;
}

Game::~Game()
{
    SDL_DestroyCursor(m_scrollCursor);
}

struct SerialisedData {
    int seed;
    float mountainScale, mountainHite;
    float hillScale, hillHite;
    float erosionScale, erosionBias;
    float bedrockScale;
    float bumpinessScale, bumpinessHite;
    float cliffScale;
    float riverScale, riverBias, riverDepth;
    float offsetX, offsetY;
    float continentScale, continentBias, oceanDepth, continentOffset;
    float lakeDepth, lakeScale, lakelandScale, lakelandBias, lakeBias;
    float landColour[3], waterColour[3];
};

#ifdef __EMSCRIPTEN__
EM_JS(char*, getSerialisedData, (), {
    var jsString = prompt("Paste serialised data here");
    if (jsString == null) {
        jsString = "a";
    }
    return stringToNewUTF8(jsString);
    });

EM_JS(void, giveSerialisedData, (char* string), {
    prompt("Copy this and save it somewhere", UTF8ToString(string));
    return;
    })
#endif

void Game::update(float dt, MouseState mouse, ButtonState* keyboard)
{
    static bool rebuild = true;

    {
        static int seed = -764210984;
        static const std::size_t serialisedLen = base64::encoded_size(sizeof(SerialisedData));

#ifndef __EMSCRIPTEN__
        static char serialised[serialisedLen] = { 0 };
#endif
        
        static bool autoRegen = true;
        static float colour[3] = { 0, 1, 0 };
        static float waterColour[3] = { 0, 0, 1 };
        bool forceRebuild;

        ImGui::Begin("Shape Shifter");
        ImGui::Checkbox("Auto regen", &autoRegen);
        if (ImGui::InputFloat("Offset X", &m_map.offsetX, 1, 10)) rebuild = true;
        if (ImGui::InputFloat("Offset Y", &m_map.offsetY, 1, 10)) rebuild = true;
        forceRebuild = ImGui::Button("Force regen");
        ImGui::InputInt("Seed", &seed, 0);
        if (ImGui::Button("Generate")) {
            rebuild = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Randomise")) {
            seed = SDL_rand_bits();
            rebuild = true;
        }
        if (ImGui::SliderFloat("Mountain Scale", &m_map.mountainScale, 1, 256)) rebuild = true;
        if (ImGui::SliderFloat("Mountain Height", &m_map.mountainHite, 0, 256)) rebuild = true;
        if (ImGui::SliderFloat("Hill Scale", &m_map.hillScale, 1, 256)) rebuild = true;
        if (ImGui::SliderFloat("Hill Height", &m_map.hillHite, 0, 256)) rebuild = true;
        if (ImGui::SliderFloat("Erosion Scale", &m_map.erosionScale, 1, 256)) rebuild = true;
        if (ImGui::SliderFloat("Erosion Bias", &m_map.erosionBias, 1, 16)) rebuild = true;
        if (ImGui::SliderFloat("Bedrock Scale", &m_map.bedrockScale, 1, 256)) rebuild = true;
        if (ImGui::SliderFloat("Bumpiness Scale", &m_map.bumpinessScale, 1, 16)) rebuild = true;
        if (ImGui::SliderFloat("Bumpiness Magn", &m_map.bumpinessHite, 0, 16)) rebuild = true;
        if (ImGui::SliderFloat("Cliff Scale", &m_map.cliffScale, 1, 256)) rebuild = true;
        if (ImGui::SliderFloat("River Scale", &m_map.riverScale, 1, 256)) rebuild = true;
        if (ImGui::SliderFloat("River Slope", &m_map.riverBias, 1, 48)) rebuild = true;
        if (ImGui::SliderFloat("River Depth", &m_map.riverDepth, 0, 256)) rebuild = true;
        if (ImGui::SliderFloat("Lake Scale", &m_map.lakeScale, 1, 64)) rebuild = true;
        if (ImGui::SliderFloat("Lake Depth", &m_map.lakeDepth, 0, 256)) rebuild = true;
        if (ImGui::SliderFloat("Lake Slope", &m_map.lakeBias, 1, 16)) rebuild = true;
        if (ImGui::SliderFloat("Lakeland Scale", &m_map.lakelandScale, 1, 256)) rebuild = true;
        if (ImGui::SliderFloat("Lakeland Bias", &m_map.lakelandBias, 1, 16)) rebuild = true;
        if (ImGui::SliderFloat("Continent Scale", &m_map.continentScale, 16, 4096)) rebuild = true;
        if (ImGui::SliderFloat("Continent Bias", &m_map.continentBias, 0.1f, 4)) rebuild = true;
        if (ImGui::SliderFloat("Continent Offset", &m_map.continentOffset, -1, 1)) rebuild = true;
        if (ImGui::SliderFloat("Ocean Depth", &m_map.oceanDepth, 0, 256)) rebuild = true;
        ImGui::End();

        ImGui::Begin("Colours");
        bool recolour = false;
        if (ImGui::ColorPicker3("Terrain colour", colour)) recolour = true;
        if (ImGui::ColorPicker3("Water colour", waterColour)) recolour = true;
        ImGui::End();

        ImGui::Begin("Saving");
        if (ImGui::Button("Copy to Clipboard")) {
            SerialisedData data = {
                .seed = seed,
                .mountainScale = m_map.mountainScale,
                .mountainHite = m_map.mountainHite,
                .hillScale = m_map.hillScale,
                .hillHite = m_map.hillHite,
                .erosionScale = m_map.erosionScale,
                .erosionBias = m_map.erosionBias,
                .bedrockScale = m_map.bedrockScale,
                .bumpinessScale = m_map.bumpinessScale,
                .bumpinessHite = m_map.bumpinessHite,
                .cliffScale = m_map.cliffScale,
                .riverScale = m_map.riverScale,
                .riverBias = m_map.riverBias,
                .riverDepth = m_map.riverDepth,
                .offsetX = m_map.offsetX,
                .offsetY = m_map.offsetY,
                .continentScale = m_map.continentScale,
                .continentBias = m_map.continentBias,
                .oceanDepth = m_map.oceanDepth,
                .continentOffset = m_map.continentOffset,
                .lakeDepth = m_map.lakeDepth,
                .lakeScale = m_map.lakeScale,
                .lakelandScale = m_map.lakelandScale,
                .lakelandBias = m_map.lakelandBias,
                .lakeBias = m_map.lakeBias
            };

            std::copy(std::begin(colour), std::end(colour), std::begin(data.landColour));
            std::copy(std::begin(waterColour), std::end(waterColour), std::begin(data.waterColour));

            std::string serialisedInter = base64_encode((uint8_t*)&data, (std::size_t)sizeof(SerialisedData));
#ifdef __EMSCRIPTEN__
            giveSerialisedData((char*)serialisedInter.c_str());
#else
            ImGui::SetClipboardText(serialisedInter.c_str());
#endif
            
        }
#ifndef __EMSCRIPTEN__
        ImGui::InputText("Serialised string", serialised, serialisedLen + 2);
#endif
        if (ImGui::Button("Load from string")) {
#ifndef __EMSCRIPTEN__
            std::string serialisedstr = serialised;
#else
            char* serialised = getSerialisedData();
            std::string serialisedstr = serialised;
#endif
            if (serialisedstr.length() == serialisedLen) {
                std::string dataStr = base64_decode(serialised);
                const SerialisedData* data = (const SerialisedData*)dataStr.c_str();
                seed = data->seed;
                m_map.mountainScale = data->mountainScale;
                m_map.mountainHite = data->mountainHite;
                m_map.hillScale = data->hillScale;
                m_map.hillHite = data->hillHite;
                m_map.erosionScale = data->erosionScale;
                m_map.erosionBias = data->erosionBias;
                m_map.bedrockScale = data->bedrockScale;
                m_map.bumpinessScale = data->bumpinessScale;
                m_map.bumpinessHite = data->bumpinessHite;
                m_map.cliffScale = data->cliffScale;
                m_map.riverScale = data->riverScale;
                m_map.riverBias = data->riverBias;
                m_map.riverDepth = data->riverDepth;
                m_map.offsetX = data->offsetX;
                m_map.offsetY = data->offsetY;
                m_map.continentScale = data->continentScale;
                m_map.continentBias = data->continentBias;
                m_map.oceanDepth = data->oceanDepth;
                m_map.continentOffset = data->continentOffset;
                m_map.lakeDepth = data->lakeDepth;
                m_map.lakeScale = data->lakeScale;
                m_map.lakelandScale = data->lakelandScale;
                m_map.lakelandBias = data->lakelandBias;
                m_map.lakeBias = data->lakeBias;

                for (int i = 0; i < 3; i++)
                {
                    colour[i] = data->landColour[i];
                    waterColour[i] = data->waterColour[i];
                }
                rebuild = true;
                
            }

#ifdef __EMSCRIPTEN__
            free(serialised);
#endif
        }
        ImGui::End();

        if ((rebuild && autoRegen) || forceRebuild) {
            m_map.generate(seed);
            m_mapRenderer->buildMesh(colour[0], colour[1], colour[2], waterColour[0], waterColour[1], waterColour[2]);
            rebuild = false;
        }
        else if (recolour) {
            m_mapRenderer->buildMesh(colour[0], colour[1], colour[2], waterColour[0], waterColour[1], waterColour[2]);
        }
    }

    ImGuiIO& io = ImGui::GetIO();


    const float mouseZoomSensitivity = 0.1f;
    const float mouseScrollSensitivity = 2.f;
    if (io.WantCaptureKeyboard) goto mouse;
    
    if (keyboard[SDL_SCANCODE_KP_PLUS].held) m_camera.zoom += dt * m_camera.zoom;
    if (keyboard[SDL_SCANCODE_KP_MINUS].held) m_camera.zoom -= dt * m_camera.zoom;
    if (mouse.scrolled) m_camera.zoom += m_camera.zoom * mouse.scrollDelta * mouseZoomSensitivity;
    m_camera.zoom = SDL_clamp(m_camera.zoom, 1, 25);

    if (keyboard[SDL_SCANCODE_KP_8].held) m_camera.y -= dt * 100;
    if (keyboard[SDL_SCANCODE_KP_2].held) m_camera.y += dt * 100;
    if (keyboard[SDL_SCANCODE_KP_4].held) m_camera.x -= dt * 100;
    if (keyboard[SDL_SCANCODE_KP_6].held) m_camera.x += dt * 100;

    
    if (keyboard[SDL_SCANCODE_W].held) {
        m_map.offsetX -= 5;
        m_map.offsetY -= 5;
        rebuild = true;
    }
    if (keyboard[SDL_SCANCODE_S].held) {
        m_map.offsetX += 5;
        m_map.offsetY += 5;
        rebuild = true;
    }
    if (keyboard[SDL_SCANCODE_A].held) {
        m_map.offsetX -= 5;
        m_map.offsetY += 5;
        rebuild = true;
    }
    if (keyboard[SDL_SCANCODE_D].held) {
        m_map.offsetX += 5;
        m_map.offsetY -= 5;
        rebuild = true;
    }

mouse:
    if (io.WantCaptureMouse) goto camera;

    if (mouse.buttons[2].pressed) {
        m_scrollAnchor.x = mouse.x;
        m_scrollAnchor.y = mouse.y;
        m_scrolling = true;
        SDL_SetCursor(m_scrollCursor);
    }
    else if (mouse.buttons[2].released) {
        m_scrolling = false;
        SDL_SetCursor(m_defaultCursor);
    }

    if (mouse.buttons[2].held) {
        float dx = mouse.x - m_scrollAnchor.x;
        float dy = mouse.y - m_scrollAnchor.y;

        m_camera.x += dt * dx * mouseScrollSensitivity / m_camera.zoom;
        m_camera.y += dt * dy * mouseScrollSensitivity / m_camera.zoom;
    }

camera:
    m_camera.x = SDL_clamp(m_camera.x, 0, 1280);
    m_camera.y = SDL_clamp(m_camera.y, 0, 960);
}

void Game::render()
{
    m_viewport = m_camera.getViewport();
    SDL_SetRenderViewport(m_renderer, &m_viewport);
    float scale = m_camera.getScale();
    SDL_SetRenderScale(m_renderer, scale, scale);
    
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);

    SDL_RenderClear(m_renderer);
    m_mapRenderer->render();

    SDL_SetRenderViewport(m_renderer, nullptr);

    ImGui::Render();
    ImGuiIO& io = ImGui::GetIO();
    SDL_SetRenderScale(m_renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_renderer);


    SDL_RenderPresent(m_renderer);

    
}
