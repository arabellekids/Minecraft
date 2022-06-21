#pragma once

#include <array>
#include <glm/vec3.hpp>

#include "blockPalette.h"
#include "../vbo/vbo.h"
#include "../ibo/ibo.h"

const float BS = 1.0f; // Block scale
const float HALF_BS = BS * 0.5f; // Half block scale

const int CHUNK_SIZE_X = 16;
const int CHUNK_SIZE_Y = 32;
const int CHUNK_SIZE_Z = 16;

template<typename T, std::size_t x, std::size_t y, std::size_t z> struct grid3D : std::array<std::array<std::array<T, x>, y>, z>
{
    std::size_t xSize = x;
    std::size_t ySize = y;
    std::size_t zSize = z;
};

struct BlockFace
{
    unsigned int index;
    glm::vec3 center;
};

class Chunk
{
private:
    glm::vec3 m_pos;

    Vbo m_vb;
    Ibo m_solidIB;

    std::vector<BlockFace> m_solidFaces;
    //std::vector<BlockFace> m_transparentFaces;

    grid3D<unsigned char, CHUNK_SIZE_X,CHUNK_SIZE_Y,CHUNK_SIZE_Z> m_blocks;

    void GenBlockFace(float x, float y, float z, unsigned char block, BlockSide side);
    void GenBlock(int x, int y, int z, unsigned char block);
    void SortFaces(const glm::vec3& pPos);
public:
    Chunk(const glm::vec3& pos);
    ~Chunk();

    void GenerateVertices();
    void GenerateIndices(const glm::vec3& pPos);

    inline Vbo& GetVb() { return m_vb; }
    inline Ibo& GetSolidIb() { return m_solidIB; }
};