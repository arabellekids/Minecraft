#pragma once

#include <array>
#include <vector>
#include <glm/vec3.hpp>

#include "block.h"

enum BlockID
{
    BLOCK_AIR = 0,
    BLOCK_GRASS = 1,
    BLOCK_DIRT = 2,
    BLOCK_WATER = 3,
    BLOCK_STONE = 4,
    BLOCK_OAKLOG = 5,
    BLOCK_OAKLEAVES = 6,
    BLOCK_LAVA = 7,
    BLOCK_GLASS = 8,
    BLOCK_STONEBRICKS = 9,
    BLOCK_OAKPLANKS = 10
};

enum BlockType
{
    Air = 0,
    Solid = 1,
    Transparent = 2,
    SemiTransparent = 3
};

struct BlockData
{
    std::array<int, 6> textures;
    BlockID id;
    BlockType type;

    float height = 1.0f;
    bool isRayHitable = true;
};

const std::vector<BlockData>& GetBlockPalette();
const BlockData& GetBlockData(unsigned char block);

float GetBlockFaceU(const BlockData& blockType, int side);
float GetBlockFaceV(const BlockData& blockType, int side);