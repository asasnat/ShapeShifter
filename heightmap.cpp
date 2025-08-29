#include "heightmap.hpp"

int vertPosToIndex(int x, int y)
{
    return y * MAP_VERT_SIZE + x;
}
