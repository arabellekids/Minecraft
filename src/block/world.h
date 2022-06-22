#pragma once

#include <memory>
#include <vector>

#include "../object/baseObject.h"

#include "../shader/shader.h"
#include "../texture/texture.h"

#include "chunk.h"

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

    const T& operator[] (unsigned int index) const { return m_grid[index]; }

    const T& operator() (std::size_t x, std::size_t y) const { return m_grid[y*m_xSize + x]; }
    T&       operator() (std::size_t x, std::size_t y)       { return m_grid[y*m_xSize + x]; }
};

class World : public BaseObject
{
private:
    Grid2D< std::unique_ptr<Chunk> > m_chunks;
    //std::vector<Chunk> m_renderChunks;

    Shader m_blockShader;
    Texture m_blockAtlasTex;

    void GenChunkBuffers(int x, int y, const glm::vec3& pPos);
    void ShiftGrid(BlockSide dir, Player& player);
public:
    World();
    ~World();

    void Update(Player& player);
    void RenderSolid(const glm::mat4& vp);

    //unsigned char GetBlock() const;
};