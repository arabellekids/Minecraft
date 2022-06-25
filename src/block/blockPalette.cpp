#include <iostream>

#include "blockPalette.h"

static std::vector<BlockType> g_palette = {
    BlockType( { 1, 1, 1, 1, 0, 2 } ), // Grass
    BlockType( { 2, 2, 2, 2, 2, 2 } ), // Dirt
    BlockType( { 3, 3, 3, 3, 3, 3 } ), // Water
    BlockType( { 4, 4, 4, 4, 4, 4 } ) // Stone
};

const std::vector<BlockType>& GetPalette() { return g_palette; }
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
