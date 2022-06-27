#pragma once

#include <array>
#include <vector>
#include <glm/vec3.hpp>

#include "block.h"

enum BlockID
{
    BLOCK_AIR = 0x00,
    BLOCK_GRASS = 0x01,
    BLOCK_DIRT = 0x02,
    BLOCK_WATER = 0x03,
    BLOCK_STONE = 0x04
};

struct BlockType
{
    std::array<int, 6> textures;

    bool isSolid = true;
};

const std::vector<BlockType>& GetPalette();
const BlockType& GetBlockType(unsigned char block);

float GetBlockFaceU(const BlockType& blockType, int side);
float GetBlockFaceV(const BlockType& blockType, int side);