#pragma once

#include <array>
#include <glm/vec3.hpp>

#include "../vbo/vbo.h"

#define PALETTE_SIZE 1

enum BlockSide
{
    BLOCK_SIDE_FRONT  = 0,
    BLOCK_SIDE_BACK   = 1,
    BLOCK_SIDE_LEFT   = 2,
    BLOCK_SIDE_RIGHT  = 3,
    BLOCK_SIDE_TOP    = 4,
    BLOCK_SIDE_BOTTOM = 5
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

const std::array<BlockType, PALETTE_SIZE>& GetPalette();
const BlockType& GetPaletteBlock(unsigned int index);