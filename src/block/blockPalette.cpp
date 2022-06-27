#include <iostream>

#include "blockPalette.h"

static std::vector<BlockType> g_palette = {
    { { -1, -1, -1, -1, -1, -1 }, false }, // Air

    { { 1, 1, 1, 1, 0, 2 }, true }, // Grass
    { { 2, 2, 2, 2, 2, 2 }, true }, // Dirt
    { { 3, 3, 3, 3, 3, 3 }, true }, // Water
    { { 4, 4, 4, 4, 4, 4 }, true } // Stone
};

const std::vector<BlockType>& GetPalette() { return g_palette; }
const BlockType& GetBlockType(unsigned char block) { return g_palette[block]; }

float GetBlockFaceU(const BlockType& blockType, int side)
{
    return (blockType.textures[side] % 16) * 0.0625f;
}

float GetBlockFaceV(const BlockType& blockType, int side)
{
    return (blockType.textures[side] >> 4) * 0.0625f;
}
