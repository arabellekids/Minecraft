#pragma once

#include "blockPalette.h"
#include "../vbo/vbo.h"
#include "../ibo/ibo.h"

const float BS = 1.0f; // Block scale
const float HALF_BS = BS * 0.5f; // Half block scale

const int CHUNK_SIZE_X = 16;
const int CHUNK_SIZE_Y = 32;
const int CHUNK_SIZE_Z = 16;

template<typename T>
class Grid3D
{
private:
    std::vector<T> m_grid;
    
    std::size_t m_xSize;
    std::size_t m_ySize;
    std::size_t m_zSize;
public:
    Grid3D(std::size_t _x, std::size_t _y, std::size_t _z)
    {
        Resize(_x, _y, _z);
    }

    inline std::size_t GetXSize() const { return m_xSize; }
    inline std::size_t GetYSize() const { return m_ySize; }
    inline std::size_t GetZSize() const { return m_zSize; }

    void Fill(const T& val)
    {
        for(auto i = 0; i < m_zSize*m_ySize*m_xSize; ++i)
        {
            m_grid[i] = val;
        }
    }

    void Resize(unsigned int x, unsigned int y, unsigned int z)
    {
        m_xSize = x;
        m_ySize = y;
        m_zSize = z;

        m_grid.resize(z * y * x);
    }

    const T& operator[] (unsigned int index) const { return m_grid[index]; }

    const T& operator() (std::size_t x, std::size_t y, std::size_t z) const { return m_grid[(z*m_ySize + y)*m_xSize + x]; }
    T&       operator() (std::size_t x, std::size_t y, std::size_t z)       { return m_grid[(z*m_ySize + y)*m_xSize + x]; }
};

struct BlockFace
{
    unsigned int index;
    glm::vec3 center;
};

class Chunk
{
private:
    Vbo m_vb;
    Ibo m_solidIB;

    std::vector<BlockFace> m_solidFaces;
    //std::vector<BlockFace> m_transparentFaces;

    Grid3D<unsigned char> m_blocks;

    void GenBlockFace(float x, float y, float z, unsigned char block, BlockSide side);
    void GenBlock(int x, int y, int z, unsigned char block);
    void SortFaces(const glm::vec3& pPos);
public:
    Chunk();
    ~Chunk();

    void GenerateVertices();
    void GenerateIndices(const glm::vec3& pPos);

    Vbo& GetVb() { return m_vb; }
    Ibo& GetSolidIb() { return m_solidIB; }
};