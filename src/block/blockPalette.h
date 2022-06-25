#pragma once

#include <array>
#include <vector>
#include <glm/vec3.hpp>

#include "../vbo/vbo.h"

enum BlockSide
{
    BLOCK_SIDE_FRONT  = 0,
    BLOCK_SIDE_BACK   = 1,
    BLOCK_SIDE_LEFT   = 2,
    BLOCK_SIDE_RIGHT  = 3,
    BLOCK_SIDE_TOP    = 4,
    BLOCK_SIDE_BOTTOM = 5
};

enum BlockID
{
    BLOCK_AIR = 0x00,
    BLOCK_GRASS = 0x01,
    BLOCK_DIRT = 0x02,
    BLOCK_WATER = 0x03,
    BLOCK_STONE = 0x04
};

class BlockType
{
private:
    std::array<int, 6> m_textures;
public:
    BlockType(const std::array<int, 6>& textures);
    ~BlockType();

    float GetU(BlockSide side) const;
    float GetV(BlockSide side) const;
};

const std::vector<BlockType>& GetPalette();
const BlockType& GetPaletteBlock(unsigned int index);