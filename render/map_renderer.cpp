#include "map_renderer.hpp"
#include "map.hpp"
#include <iostream>

MapRenderer::MapRenderer(SDL_Renderer* const& renderer, const Map& map)
    : Renderer(renderer), m_map(map)
{
    buildMesh();
}

SDL_FPoint isoToScreen(float x, float y)
{
    float ptx = x * 5 - y * 5;
    float pty = x * 2.5f + y * 2.5f;

    return SDL_FPoint{ .x = ptx, .y = pty };
}

void MapRenderer::buildMesh(float r, float g, float b, float wr, float wg, float wb)
{
    const float ambientLight = 0.1f;
    const float maxLight = 0.8f;
    m_terrainVerticies.clear();
    m_terrainIndicies.clear();

    // verts
    for (int y = 0; y < MAP_VERT_SIZE; y++)
        for (int x = 0; x < MAP_VERT_SIZE; x++) {
            SDL_FPoint a = isoToScreen(x, y);
            float height = m_map.getHeight(x, y);
            a.y -= height;
            a.y += 256;

            // calculate normal
            // glm::vec3 normal;
            // glm::vec3 xslope, yslope;
            // if (x < MAP_SIZE && y < MAP_SIZE) {
            //     xslope = glm::vec3{ 1, m_map.getHeight(x + 1, y), 0 };
            //     yslope = glm::vec3{ 0, m_map.getHeight(x, y + 1), 1 };
            // }
            // else if (x == MAP_SIZE && y == MAP_SIZE) {
            //     xslope = glm::vec3{ -1, m_map.getHeight(x - 1, y), 0 };
            //     yslope = glm::vec3{ 0, m_map.getHeight(x, y - 1), -1 };
            // }
            // else if (x == MAP_SIZE) {
            //     xslope = glm::vec3{ 1, m_map.getHeight(x - 1, y), 0 };
            //     yslope = glm::vec3{ 0, m_map.getHeight(x, y + 1), 1 };
            // }
            // else if (y == MAP_SIZE) {
            //     xslope = glm::vec3{ 1, m_map.getHeight(x + 1, y), 0 };
            //     yslope = glm::vec3{ 0, m_map.getHeight(x, y - 1), 1 };
            // }
            // normal = glm::normalize(glm::cross(xslope, yslope));

            //m_normals[y * MAP_VERT_SIZE + x] = normal;

            //float light = (glm::dot(sunDir, normal) + 1) / 2;
            //light = glm::max(light, ambientLight);
            float slope;
            if (x < MAP_SIZE) {
                slope = m_map.getHeight(x, y) - m_map.getHeight(x + 1, y);
            }
            else {
                slope = -(m_map.getHeight(x, y) - m_map.getHeight(x - 1, y));
            }
            float light = (slope + 12) / 32.f;
            light = SDL_clamp(light, ambientLight, maxLight);

            SDL_Vertex va;

            va.position = a;
            va.position.x += 640;

            if (height > -16)
                va.color = SDL_FColor{ .r = light * r, .g = light * g, .b = light * b, .a = 1 };
            else
                va.color = SDL_FColor{ .r = wr, .g = wg, .b = wb, .a = 1 };

            m_terrainVerticies.push_back(va);
        }

    // inds
    for (int i = 0; i < MAP_VERT_SIZE * MAP_VERT_SIZE - MAP_VERT_SIZE; i++) {
        if ((i + 1) % MAP_VERT_SIZE == 0) continue; // do not handle edge verticies

        m_terrainIndicies.push_back(i);
        m_terrainIndicies.push_back(i + 1);
        m_terrainIndicies.push_back(i + MAP_VERT_SIZE);

        m_terrainIndicies.push_back(i + 1);
        m_terrainIndicies.push_back(i + MAP_VERT_SIZE + 1);
        m_terrainIndicies.push_back(i + MAP_VERT_SIZE);
        
    }
}

void MapRenderer::render()
{
    SDL_RenderGeometry(m_renderer, nullptr,
        m_terrainVerticies.data(), m_terrainVerticies.size(),
        m_terrainIndicies.data(), m_terrainIndicies.size());
}
