#pragma once

#include <memory>
#include <vector>

#include "../shader/shader.h"
#include "../texture/texture.h"

#include "chunk.h"

class Player;
class Ray;

template<typename T>
class Grid2D
{
private:
    std::vector<T> m_grid;
    
    std::size_t m_xSize;
    std::size_t m_ySize;

public:
    Grid2D(std::size_t _x, std::size_t _y)
    {
        Resize(_x, _y);
    }

    int GetXSize() const { return m_xSize; }
    int GetYSize() const { return m_ySize; }
    
    void Fill(const T& val)
    {
        for(auto i = 0; i < m_ySize*m_xSize; ++i)
        {
            m_grid[i] = val;
        }
    }

    void Resize(unsigned int x, unsigned int y)
    {
        m_xSize = x;
        m_ySize = y;
        
        m_grid.resize(y * x);
    }

    const T& operator[] (int index) const { return m_grid[index]; }

    const T& operator() (int x, int y) const { return m_grid[y*m_xSize + x]; }
    T&       operator() (int x, int y)       { return m_grid[y*m_xSize + x]; }
};

class ChunkLoader
{
public:
    ChunkLoader(long xPos, long zPos, int xChunk, int yChunk);

    glm::vec<2, long, glm::defaultp> pos;
    glm::vec2 chunkIndex;
};

struct BlockHitInfo
{
    glm::ivec3 pos;
    glm::ivec3 neighbor;
    const BlockData* blockData;
};

struct RenderChunk
{
    glm::mat4 model;
    Chunk* chunk;
    
    float dist = 0.0f;

    bool operator< (const RenderChunk& b) const { return dist > b.dist; }
};

class World
{
private:
    Grid2D< std::unique_ptr<Chunk> > m_chunks;
    std::vector<ChunkLoader*> m_loadingChunks;

    glm::ivec2 m_offset;

    std::vector<RenderChunk> m_renderChunks;

    Shader m_blockShader;
    Texture m_blockAtlasTex;

    const int m_chunkLoadLimit = 1;

    void GenChunkBuffers(int x, int y, const glm::vec3& pPos);
    
    void ShiftGrid(BlockSide dir, Player& player);
    void LoadChunks(Player& player);
    void QueueChunk(long xPos, long zPos, int xChunk, int yChunk);
public:
    World();
    ~World();

    void Update(Player& player);
    void RenderSolid(const glm::mat4& vp, const glm::vec3& pPos);

    bool Raycast(const Ray& ray, BlockHitInfo& info) const;

    unsigned char GetBlockFromPos(float x, float y, float z) const;
    unsigned char GetBlockFromIndex(int x, int y, int z) const;

    unsigned char GetLightFromIndex(int x, int y, int z) const;

    void SetBlockFromPos(float x, float y, float z, unsigned char block);
    void SetBlockFromIndex(int x, int y, int z, unsigned char block);
    
    bool IsValidChunk(int x, int y) const;
    bool IsValidBlock(int x, int y, int z) const;

    glm::ivec3 PosToBlock(float x, float y, float z) const;
    glm::vec3 BlockToPos(int x, int y, int z) const;
};