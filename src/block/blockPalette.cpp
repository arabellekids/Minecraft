#include <iostream>

#include "blockPalette.h"

static std::array<BlockType, PALETTE_SIZE> g_palette = {
    BlockType( { 0, 0, 0, 0, 1, 0 } ) // Grass
};

const std::array<BlockType, PALETTE_SIZE>& GetPalette() { return g_palette; }
const BlockType& GetPaletteBlock(unsigned int index) { return g_palette[index - 1]; }

BlockType::BlockType(const std::array<int, 6>& textures)
{
    m_textures = textures;
}
BlockType::~BlockType() {}

float BlockType::GetU(BlockSide side) const
{
    return (m_textures[side] % 16) * 0.0625f;
}

float BlockType::GetV(BlockSide side) const
{
    return (m_textures[side] >> 4) * 0.0625f;
}
