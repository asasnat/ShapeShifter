#pragma once

#include <cmath>

constexpr int MAP_SIZE = 127;
constexpr int MAP_VERT_SIZE = MAP_SIZE + 1;
struct Heightmap {
    float hmap[MAP_VERT_SIZE * MAP_VERT_SIZE];

    float get(int x, int y) const
    {
        if (x < 0 || x >= MAP_VERT_SIZE || y < 0 || y >= MAP_VERT_SIZE) return 0;
        return hmap[y * MAP_VERT_SIZE + x];
    }

    void set(int x, int y, float val)
    {
        if (x < 0 || x >= MAP_VERT_SIZE || y < 0 || y >= MAP_VERT_SIZE) return;
        hmap[y * MAP_VERT_SIZE + x] = val;
    }
};

int vertPosToIndex(int x, int y);
