#include "map.hpp"
#include <SDL3/SDL_stdinc.h>

Map::Map()
{
    for (int i = 0; i < MAP_VERT_SIZE * MAP_VERT_SIZE; i++) {
        m_hmap.hmap[i] = 0;
    }
}

Map::Map(siv::PerlinNoise::seed_type seed)
{
    generate(seed);
}

float lerp(float a, float b, float t)
{
    if (t <= 0) return a;
    if (t >= 1) return b;
    return a + t * (b - a);
}

void Map::generate(siv::PerlinNoise::seed_type seed)
{
    m_perlin.reseed(seed);

    for (int y = 0; y < MAP_VERT_SIZE; y++)
        for (int x = 0; x < MAP_VERT_SIZE; x++) {
            float mountain = m_perlin.octave2D_01((x + offsetX) / mountainScale, (y + offsetY) / mountainScale, 4);
            //mountain *= mountain;
            mountain *= mountainHite;
            float hill = m_perlin.noise2D((x + offsetX) / hillScale, (y + offsetY) / hillScale) * hillHite;

            m_perlin.reseed(seed - 1);
            float cliffNoise = m_perlin.octave2D_01((x + offsetX) / cliffScale, (y + offsetY) / cliffScale, 3);
            float cliff = (cliffNoise < 0.65f && cliffNoise > 0.45f) ? (cliffNoise - 0.65f) / 0.45f : 0;

            m_perlin.reseed(seed - 2);
            float riverNoise = m_perlin.octave2D_11((x + offsetX) / riverScale, (y + offsetY) / riverScale, 2);
            float river = powf(-(fabsf(riverNoise) - 1), riverBias);

            m_perlin.reseed(seed + 1);
            float erosion = SDL_min(m_perlin.noise2D_01((x + offsetX) / erosionScale, (y + offsetY) / erosionScale) + 0.1f, 1);
            float bedrock = m_perlin.octave2D_11((x + offsetX) / bedrockScale, (y + offsetY) / bedrockScale, 2);

            //float erosion = (sinf(x / 16.f) + 1) / 2 + 0.1f;
            float bumpiness = m_perlin.noise2D((x + offsetX) / bumpinessScale, (y + offsetY) / bumpinessScale) * bumpinessHite;

            float terrain = lerp(hill, mountain, powf(erosion, erosionBias)) + bedrock + bumpiness;
            float cliffyTerrain = lerp(terrain, -32, cliff);
            //float cliffyTerrain = terrain;

            m_perlin.reseed(seed + 3);
            float lakeland = m_perlin.octave2D_01((x + offsetX) / lakelandScale, (y + offsetY) / lakelandScale, 3);
            float lakeNoise = powf(m_perlin.octave2D_01((x + offsetX) / lakeScale, (y + offsetY) / lakeScale, 3), lakeBias) * -lakeDepth;
            float lakeyTerrain = lerp(cliffyTerrain, lakeNoise, powf(lakeland, lakelandBias));

            m_perlin.reseed(seed + 2);
            //float continent = SDL_clamp((m_perlin.octave2D_11((x + offsetX) / continentScale, (y + offsetY) / continentScale, 3) * continentBias + 1) / 2, 0, 1);
            float continent = m_perlin.octave2D_01((x + offsetX) / continentScale, (y + offsetY) / continentScale, 3);

            float landmass = lerp(lakeyTerrain, -riverDepth, river);
            float value = lerp(-oceanDepth, landmass, powf(continent, continentBias) + continentOffset);

            m_hmap.set(x, y, SDL_max(value, -16));
        }
}
