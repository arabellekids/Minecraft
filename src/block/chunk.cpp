#include <GLES3/gl32.h>
#include <iostream>

#include "chunk.h"

Chunk::Chunk() : m_vb({}, GL_DYNAMIC_DRAW), m_solidIB({}, GL_DYNAMIC_DRAW), m_blocks(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z)
{
    m_blocks.Fill(0x00);
    
    for(auto z = 0; z < CHUNK_SIZE_Z; ++z)
    {
        for(auto x = 0; x < CHUNK_SIZE_X; ++x)
        {
            m_blocks(x,0,z) = 0x01;
        }
    }

    // Place random blocks
    for(int i = 0; i < 20; ++i)
    {
        int x = rand() / (float)RAND_MAX * m_blocks.GetXSize();
        int y = rand() / (float)RAND_MAX * m_blocks.GetYSize();
        int z = rand() / (float)RAND_MAX * m_blocks.GetZSize();
        
        m_blocks(x,y,z) = 0x01;
    }
}

Chunk::~Chunk() {}

void Chunk::GenerateVertices()
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
                    GenBlock(x,y,z, m_blocks(x,y,z));
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

void Chunk::GenBlock(int x, int y, int z, unsigned char block)
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
    if(z > 0 && m_blocks(x, y, z - 1) == 0)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_FRONT);
    }

    // Back
    if(z < CHUNK_SIZE_Z - 1 && m_blocks(x, y, z + 1) == 0)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_BACK);
    }

    // Left
    if(x > 0 && m_blocks(x - 1, y, z) == 0)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_LEFT);
    }

    // Right
    if(x < CHUNK_SIZE_X - 1 && m_blocks(x + 1, y, z) == 0)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_RIGHT);
    }

    // Top
    if(y < CHUNK_SIZE_Y - 1 && m_blocks(x, y + 1, z) == 0)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_TOP);
    }

    // Bottom
    if(y > 0 && m_blocks(x, y - 1, z) == 0)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_BOTTOM);
    }
}

void Chunk::GenBlockFace(float x, float y, float z, unsigned char block, BlockSide side)
{
    // float u1 = 0.0f;
    // float v1 = 0.0f;
    float u1 = GetPaletteBlock(block).GetU(side);
    float v1 = GetPaletteBlock(block).GetV(side);

    float u2 = u1 + 0.0624f;
    float v2 = v1 + 0.0624f;

    unsigned int index = m_vb.GetData().size() >> 2;

    switch (side)
    {
        case BLOCK_SIDE_FRONT:
            m_vb.GetData().push_back( { x,    y+BS, z, u1,v1} );
            m_vb.GetData().push_back( { x,    y,    z, u1,v2} );
            m_vb.GetData().push_back( { x+BS, y,    z, u2,v2} );
            m_vb.GetData().push_back( { x+BS, y+BS, z, u2,v1} );

            m_solidFaces.push_back( { index, {x+HALF_BS, y+HALF_BS, z} } );
            break;
        
        case BLOCK_SIDE_BACK:
            m_vb.GetData().push_back( { x+BS, y+BS, z+BS, u1,v1} );
            m_vb.GetData().push_back( { x+BS, y,    z+BS, u1,v2} );
            m_vb.GetData().push_back( { x,    y,    z+BS, u2,v2} );
            m_vb.GetData().push_back( { x,    y+BS, z+BS, u2,v1} );

            m_solidFaces.push_back( { index, {x+HALF_BS, y+HALF_BS, z+BS} } );
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