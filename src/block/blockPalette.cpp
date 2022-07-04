#include <iostream>

#include "blockPalette.h"

static std::vector<BlockData> g_palette = {
    { { -1, -1, -1, -1, -1, -1 }, BLOCK_AIR, BlockType::Air, 1.0f, false, 0 }, // Air

    { { 1, 1, 1, 1, 0, 2 }, BLOCK_GRASS, BlockType::Solid, 1.0f, true, 0 }, // Grass
    { { 2, 2, 2, 2, 2, 2 }, BLOCK_DIRT, BlockType::Solid, 1.0f, true, 0 }, // Dirt
    { { 3, 3, 3, 3, 3, 3 }, BLOCK_WATER, BlockType::SemiTransparent, 0.8f, false, 0 }, // Water
    { { 4, 4, 4, 4, 4, 4 }, BLOCK_STONE, BlockType::Solid, 1.0f, true, 0 }, // Stone
    { { 5, 5, 5, 5, 6, 6 }, BLOCK_OAKLOG, BlockType::Solid, 1.0f, true, 0 }, // OakLog
    { { 7, 7, 7, 7, 7, 7 }, BLOCK_OAKLEAVES, BlockType::Transparent, 1.0f, true, 0 }, // OakLeaves
    { { 8, 8, 8, 8, 8, 8 }, BLOCK_LAVA, BlockType::Transparent, 1.0f, true, 15 }, // Lava
    { { 9, 9, 9, 9, 9, 9 }, BLOCK_GLASS, BlockType::Transparent, 1.0f, true, 0 }, // Glass
    { { 10, 10, 10, 10, 10, 10 }, BLOCK_STONEBRICKS, BlockType::Solid, 1.0f, true, 0 }, // Stone bricks
    { { 11, 11, 11, 11, 11, 11 }, BLOCK_OAKPLANKS, BlockType::Solid, 1.0f, true, 0 } // Oak planks
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
