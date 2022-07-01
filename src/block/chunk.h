#pragma once

#include <glm/vec2.hpp>

#include "blockPalette.h"
#include "../vao/vao.h"
#include "../vbo/vbo.h"
#include "../ibo/ibo.h"

const float BS = 1.0f; // Block scale
const float HALF_BS = BS * 0.5f; // Half block scale

const int CHUNK_SIZE_X = 16;
const int CHUNK_SIZE_Y = 128;
const int CHUNK_SIZE_Z = 16;

class World;

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

    int GetXSize() const { return m_xSize; }
    int GetYSize() const { return m_ySize; }
    int GetZSize() const { return m_zSize; }

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

    const T& operator[] (int index) const { return m_grid[index]; }

    const T& operator() (int x, int y, int z) const { return m_grid[(z*m_ySize + y)*m_xSize + x]; }
    T&       operator() (int x, int y, int z)       { return m_grid[(z*m_ySize + y)*m_xSize + x]; }
};

class BlockFace
{
public:
    BlockFace(unsigned int i, const glm::vec3& pos) : index(i), center(pos) {}
    unsigned int index;
    glm::vec3 center;

    float dist = 0.0f;

    bool operator< (const BlockFace& b) { return dist > b.dist; }
};

struct BlockPointData
{
    unsigned char block;
    glm::ivec3 pos;
};

class Chunk
{
private:
    bool m_isLoading;
    bool m_modified;

    glm::ivec2 m_pos;

    Vbo m_vb;
    Vao m_va;
    Ibo m_solidIB;
    Ibo m_transparentIB;

    std::vector<BlockFace> m_solidFaces;
    std::vector<BlockFace> m_transparentFaces;

    std::array<std::vector<BlockPointData>, 4> m_toDoBlocks;
    
    Grid3D<unsigned char> m_blocks;

    void GenBlockFace(float x, float y, float z, unsigned char block, BlockSide side);
    void GenBlock(int x, int y, int z, unsigned char block, Chunk* n, Chunk* s, Chunk* e, Chunk* w);

    void LoadBaseTerrain(const glm::i64vec2& pos);
    void LoadLayers(const glm::i64vec2& pos);
    void LoadDetails(const glm::i64vec2& pos);

    void GenTree(int x, int y, int z);

    void LoadBlock(int x, int y, int z, unsigned char block);
public:
    Chunk();
    Chunk(const glm::ivec2& pos);
    ~Chunk();

    unsigned char GetBlock(int x, int y, int z, const World& world) const;
    void SetBlock(int x, int y, int z, unsigned char block, World& world);

    void GenerateVertices(const World& world);
    void GenerateIndices(const glm::vec3& pPos);

    void RegenerateTransparency(const glm::vec3& pPos);
    void SortFaces(const glm::vec3& pPos);

    void UpdateToDoList(World& world);

    const Vao& GetVa() const { return m_va; }
    Vao& GetVa() { return m_va; }

    const Vbo& GetVb() const { return m_vb; }
    Vbo& GetVb() { return m_vb; }

    const Ibo& GetSolidIb() const { return m_solidIB; }
    Ibo& GetSolidIb() { return m_solidIB; }

    const Ibo& GetTransparentIb() const { return m_transparentIB; }
    Ibo& GetTransparentIb() { return m_transparentIB; }

    const glm::ivec2& GetPos() const { return m_pos; }
    glm::ivec2& GetPos() { return m_pos; }

    void Load(const glm::i64vec2& pos);

    void SetLoading(bool loading) { m_isLoading = loading; }
    bool GetLoading() const { return m_isLoading; }

    void SetModified(bool modified) { m_modified = modified; }
    bool GetModified() const { return m_modified; }

};