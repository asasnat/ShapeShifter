#pragma once
#include "renderer.hpp"
#include <vector>
#include "map.hpp"

class MapRenderer : public Renderer {
private:
    const Map& m_map;
    std::vector<SDL_Vertex> m_terrainVerticies;
    std::vector<int> m_terrainIndicies;

    //glm::vec3 m_normals[MAP_VERT_SIZE * MAP_VERT_SIZE];

public:
    MapRenderer(SDL_Renderer* const& renderer, const Map& map);

public:
    void render() override;
    void buildMesh(float r = 0, float g = 1, float b = 0, float wr = 0, float wg = 0, float wb = 1);
};
