#pragma once
#include "heightmap.hpp"
#include "noise/PerlinNoise.hpp"

class Map {
private:
    Heightmap m_hmap;
    siv::PerlinNoise m_perlin;

public:
    float mountainScale = 16, mountainHite = 128;
    float hillScale = 32, hillHite = 16;
    float erosionScale = 32, erosionBias = 3;
    float bedrockScale = 48;
    float bumpinessScale = 1, bumpinessHite = 0.5f;
    float cliffScale = 175.674f;
    float riverScale = 256, riverBias = 33.696f, riverDepth = 41.183f;

    float offsetX = 1040, offsetY = -40;

    float continentScale = 1024, continentBias = 1.576f, oceanDepth = 81.594f, continentOffset = 0.562f;
    float lakeDepth = 153.565f, lakeScale = 37.926f, lakelandScale = 48.137f, lakelandBias = 3, lakeBias = 2.761f;

public:
    Map();
    Map(siv::PerlinNoise::seed_type seed);

    float getHeight(int x, int y) const { return m_hmap.get(x, y); }

    void generate(siv::PerlinNoise::seed_type seed);
};
