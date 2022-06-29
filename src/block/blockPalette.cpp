#include <iostream>

#include "blockPalette.h"

static std::vector<BlockType> g_palette = {
    { { -1, -1, -1, -1, -1, -1 }, BLOCK_AIR, false, 1.0f }, // Air

    { { 1, 1, 1, 1, 0, 2 }, BLOCK_GRASS, true, 1.0f }, // Grass
    { { 2, 2, 2, 2, 2, 2 }, BLOCK_DIRT, true, 1.0f }, // Dirt
    { { 3, 3, 3, 3, 3, 3 }, BLOCK_WATER, false, 0.8f }, // Water
    { { 4, 4, 4, 4, 4, 4 }, BLOCK_STONE, true, 1.0f } // Stone
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
