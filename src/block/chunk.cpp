#include <GLES3/gl32.h>
#include <iostream>

#include "chunk.h"

Chunk::Chunk() : m_pos(0, 0), m_vb({}, GL_DYNAMIC_DRAW), m_solidIB({}, GL_DYNAMIC_DRAW), m_blocks(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z)
{
    BufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    
    m_va.AddBuffer(m_vb, layout);
}

Chunk::~Chunk() {}

void Chunk::Load(const glm::vec<2, long, glm::defaultp>& pos)
{
    m_pos = pos;

    m_blocks.Fill(0x00);
    
    for(auto y = 0; y < m_blocks.GetYSize() - 16; ++y)
    {
        for(auto z = 0; z < m_blocks.GetZSize(); ++z)
        {
            for(auto x = 0; x < m_blocks.GetXSize(); ++x)
            {
                m_blocks(x,y,z) = 0x01;
            }
        }
    }
    
    m_blocks(m_blocks.GetXSize() >> 1, 16, m_blocks.GetZSize() >> 1) = 0x02;
}

void Chunk::GenerateVertices(Chunk* n, Chunk* s, Chunk* e, Chunk* w)
{
    m_vb.GetData().clear();
    m_solidFaces.clear();

    // +Y = up, +X = right, +Z = forward
    for(auto z = 0; z < m_blocks.GetZSize(); ++z)
    {
        for(auto y = 0; y < m_blocks.GetYSize(); ++y)
        {
            for(auto x = 0; x < m_blocks.GetXSize(); ++x)
            {
                if(m_blocks(x,y,z) != 0)
                {
                    GenBlock(x,y,z, m_blocks(x,y,z), n,s,e,w);
                }
            }
        }
    }

    m_vb.SetData(m_vb.GetData(), false);
}

void Chunk::GenerateIndices(const glm::vec3& pPos)
{
    m_solidIB.GetData().clear();
    
    // ToDo: sort the faces
    // sort(m_solidFaces)

    for(auto i = 0; i < m_solidFaces.size(); ++i)
    {
        auto index = m_solidFaces[i].index << 2;
        m_solidIB.GetData().push_back(index);
        m_solidIB.GetData().push_back(index+1);
        m_solidIB.GetData().push_back(index+2);

        m_solidIB.GetData().push_back(index);
        m_solidIB.GetData().push_back(index+2);
        m_solidIB.GetData().push_back(index+3);
    }

    m_solidIB.SetData(m_solidIB.GetData(), false);
}

unsigned char Chunk::GetBlock(int x, int y, int z, Chunk* n, Chunk* s, Chunk* e, Chunk* w)
{
    // Coord is not in y range of chunks
    if(y < 0 || y >= m_blocks.GetYSize()) { return 0; }
    
    // Block is in this chunk
    if(x >= 0 && z >= 0 && x < m_blocks.GetXSize() && z < m_blocks.GetZSize())
    {
        return m_blocks(x, y, z);
    }

    // Block is in north chunk
    if(z >= m_blocks.GetZSize() && n != nullptr)
    {
        return n->m_blocks(x,y, z - m_blocks.GetZSize());
    }
    
    // Block is in south chunk
    if(z < 0 && s != nullptr)
    {
        return s->m_blocks(x,y, z + m_blocks.GetZSize());
    }

    // Block is in east chunk
    if(x >= m_blocks.GetXSize() && e != nullptr)
    {
        return e->m_blocks(x - m_blocks.GetXSize(), y, z);
    }

    // Block is in west chunk
    if(x < 0 && w != nullptr)
    {
        return w->m_blocks(x + m_blocks.GetXSize(), y, z);
    }
    
    // Correct neighbor chunk has not been loaded, assume air
    return 0;
}

void Chunk::GenBlock(int x, int y, int z, unsigned char block, Chunk* n, Chunk* s, Chunk* e, Chunk* w)
{
    float xPos = x * BS;
    float yPos = y * BS;
    float zPos = z * BS;
    
    // GenBlockFace(x,y,z, block, BLOCK_SIDE_FRONT);
    // GenBlockFace(x,y,z, block, BLOCK_SIDE_BACK);
    // GenBlockFace(x,y,z, block, BLOCK_SIDE_LEFT);
    // GenBlockFace(x,y,z, block, BLOCK_SIDE_RIGHT);
    // GenBlockFace(x,y,z, block, BLOCK_SIDE_TOP);
    // GenBlockFace(x,y,z, block, BLOCK_SIDE_BOTTOM);
    
    // Front
    if(GetBlock(x, y, z + 1, n,s,e,w) == 0)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_FRONT);
    }

    // Back
    if(GetBlock(x, y, z - 1, n,s,e,w) == 0)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_BACK);
    }

    // Left
    if(GetBlock(x - 1, y, z, n,s,e,w) == 0)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_LEFT);
    }

    // Right
    if(GetBlock(x + 1, y, z, n,s,e,w) == 0)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_RIGHT);
    }

    // Top
    if(GetBlock(x, y + 1, z, n,s,e,w) == 0)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_TOP);
    }

    // Bottom
    if(GetBlock(x, y - 1, z, n,s,e,w) == 0)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_BOTTOM);
    }
}

void Chunk::GenBlockFace(float x, float y, float z, unsigned char block, BlockSide side)
{
    float u1 = GetPaletteBlock(block).GetU(side);
    float v1 = GetPaletteBlock(block).GetV(side);

    float u2 = u1 + 0.0624f;
    float v2 = v1 + 0.0624f;

    unsigned int index = m_vb.GetData().size() >> 2;

    switch (side)
    {
        case BLOCK_SIDE_FRONT:
            m_vb.GetData().push_back( { x+BS, y+BS, z+BS, u1,v1} );
            m_vb.GetData().push_back( { x+BS, y,    z+BS, u1,v2} );
            m_vb.GetData().push_back( { x,    y,    z+BS, u2,v2} );
            m_vb.GetData().push_back( { x,    y+BS, z+BS, u2,v1} );

            m_solidFaces.push_back( { index, {x+HALF_BS, y+HALF_BS, z+BS} } );
            break;
        
        case BLOCK_SIDE_BACK:
            m_vb.GetData().push_back( { x,    y+BS, z, u1,v1} );
            m_vb.GetData().push_back( { x,    y,    z, u1,v2} );
            m_vb.GetData().push_back( { x+BS, y,    z, u2,v2} );
            m_vb.GetData().push_back( { x+BS, y+BS, z, u2,v1} );

            m_solidFaces.push_back( { index, {x+HALF_BS, y+HALF_BS, z} } );
            break;
        
        case BLOCK_SIDE_LEFT:
            m_vb.GetData().push_back( { x, y+BS, z+BS, u1,v1} );
            m_vb.GetData().push_back( { x, y,    z+BS, u1,v2} );
            m_vb.GetData().push_back( { x, y,    z,    u2,v2} );
            m_vb.GetData().push_back( { x, y+BS, z,    u2,v1} );

            m_solidFaces.push_back( { index, {x, y+HALF_BS, z+HALF_BS} } );
            break;
        
        case BLOCK_SIDE_RIGHT:
            m_vb.GetData().push_back( { x+BS, y+BS, z,    u1,v1} );
            m_vb.GetData().push_back( { x+BS, y,    z,    u1,v2} );
            m_vb.GetData().push_back( { x+BS, y,    z+BS, u2,v2} );
            m_vb.GetData().push_back( { x+BS, y+BS, z+BS, u2,v1} );

            m_solidFaces.push_back( { index, {x+BS, y+HALF_BS, z+HALF_BS} } );
            break;
        
        case BLOCK_SIDE_TOP:
            m_vb.GetData().push_back( { x,    y+BS, z+BS, u1,v1} );
            m_vb.GetData().push_back( { x,    y+BS, z,    u1,v2} );
            m_vb.GetData().push_back( { x+BS, y+BS, z,    u2,v2} );
            m_vb.GetData().push_back( { x+BS, y+BS, z+BS, u2,v1} );

            m_solidFaces.push_back( { index, {x+HALF_BS, y+BS, z+HALF_BS} } );
            break;
        
        case BLOCK_SIDE_BOTTOM:
            m_vb.GetData().push_back( { x,    y, z,    u1,v1} );
            m_vb.GetData().push_back( { x,    y, z+BS, u1,v2} );
            m_vb.GetData().push_back( { x+BS, y, z+BS, u2,v2} );
            m_vb.GetData().push_back( { x+BS, y, z,    u2,v1} );

            m_solidFaces.push_back( { index, {x+HALF_BS, y, z+HALF_BS} } );
            break;
    }
}