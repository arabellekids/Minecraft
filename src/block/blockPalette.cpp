#include <iostream>

#include "blockPalette.h"

static std::vector<BlockData> g_palette = {
    { { -1, -1, -1, -1, -1, -1 }, BLOCK_AIR, BlockType::Air, 1.0f, false }, // Air

    { { 1, 1, 1, 1, 0, 2 }, BLOCK_GRASS, BlockType::Solid, 1.0f, true }, // Grass
    { { 2, 2, 2, 2, 2, 2 }, BLOCK_DIRT, BlockType::Solid, 1.0f, true }, // Dirt
    { { 3, 3, 3, 3, 3, 3 }, BLOCK_WATER, BlockType::SemiTransparent, 0.8f, false }, // Water
    { { 4, 4, 4, 4, 4, 4 }, BLOCK_STONE, BlockType::Solid, 1.0f, true }, // Stone
    { { 5, 5, 5, 5, 6, 6 }, BLOCK_OAKLOG, BlockType::Solid, 1.0f, true }, // OakLog
    { { 7, 7, 7, 7, 7, 7 }, BLOCK_OAKLEAVES, BlockType::Transparent, 1.0f, true }, // OakLeaves
    { { 8, 8, 8, 8, 8, 8 }, BLOCK_LAVA, BlockType::Solid, 1.0f, true }, // Lava
    { { 9, 9, 9, 9, 9, 9 }, BLOCK_GLASS, BlockType::Transparent, 1.0f, true }, // Glass
    { { 10, 10, 10, 10, 10, 10 }, BLOCK_STONEBRICKS, BlockType::Solid, 1.0f, true }, // Stone bricks
    { { 11, 11, 11, 11, 11, 11 }, BLOCK_OAKPLANKS, BlockType::Solid, 1.0f, true } // Oak planks
};

const std::vector<BlockData>& GetBlockPalette() { return g_palette; }
const BlockData& GetBlockData(unsigned char block) { return g_palette[block]; }

float GetBlockFaceU(const BlockData& blockType, int side)
{
    return (blockType.textures[side] % 16) * 0.0625f;
}

float GetBlockFaceV(const BlockData& blockType, int side)
{
    return (blockType.textures[side] >> 4) * 0.0625f;
}
