#include <cmath>
#include <GLES3/gl32.h>
#include <iostream>

#include "chunk.h"
#include "../settings/settings.h"

#define max(x,y) (x > y) ? (x) : (y)
#define min(x,y) (x < y) ? (x) : (y)
#define abs(x) ((x) > 0) ? (x) : (-x)

Chunk::Chunk() : m_pos(0, 0), m_vb({}, GL_DYNAMIC_DRAW), m_solidIB({}, GL_DYNAMIC_DRAW), m_blocks(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z)
{
    m_isLoading = false;
    
    BufferLayout layout;
    layout.Push<float>(3); // x,y,z
    layout.Push<float>(2); // u,v
    layout.Push<float>(1); // light
    
    m_va.AddBuffer(m_vb, layout);
}

Chunk::~Chunk() {}

void Chunk::Load(const glm::vec<2, long, glm::defaultp>& pos)
{
    m_pos = pos;
    
    long chunkX = pos.x * CHUNK_SIZE_X;
    long chunkZ = pos.y * CHUNK_SIZE_Z;

    for(int z = 0; z < CHUNK_SIZE_Z; ++z)
    {
        for(int x = 0; x < CHUNK_SIZE_X; ++x)
        {
            float squashingFactor = 0.03f;
            float heightOffset = 32.0f;
            
            for(int y = 0; y < CHUNK_SIZE_Y; ++y)
            {
                float density = Settings::noise.noise3D((chunkX + x) * 0.0625f, y * 0.0625f, (chunkZ + z) * 0.0625f) + (heightOffset - y)*squashingFactor;

                if(density > 0)
                {
                    m_blocks(x,y,z) = BLOCK_STONE;
                }
                else
                {
                    m_blocks(x,y,z) = BLOCK_AIR;
                }
            }
        }
    }

    // Add grass
    for(int z = 0; z < CHUNK_SIZE_Z; ++z)
    {
        for(int x = 0; x < CHUNK_SIZE_X; ++x)
        {
            int grassLayers = 0;
            for(int y = CHUNK_SIZE_Y - 1; y > 0; --y)
            {
                if(m_blocks(x,y,z) != 0)
                {
                    if(y < CHUNK_SIZE_Y && m_blocks(x,y + 1,z) == BLOCK_AIR)
                    {
                        m_blocks(x,y,z) = BLOCK_GRASS;
                    }
                    else
                    {
                        m_blocks(x,y,z) = BLOCK_DIRT;
                    }
                    grassLayers++;

                    if(grassLayers >= 5) { break; }
                }
            }
        }
    }
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
    if(y < 0 || y >= CHUNK_SIZE_Y) { return 0; }
    
    // Block is in this chunk
    if(x >= 0 && z >= 0 && x < CHUNK_SIZE_X && z < CHUNK_SIZE_Z) { return m_blocks(x, y, z); }

    // Block is in north chunk
    if(z >= CHUNK_SIZE_Z && n != nullptr) { return n->m_blocks(x,y, 0); }
    
    // Block is in south chunk
    if(z < 0 && s != nullptr) { return s->m_blocks(x,y, CHUNK_SIZE_Z - 1); }

    // Block is in east chunk
    if(x >= CHUNK_SIZE_X && e != nullptr) { return e->m_blocks(0, y, z); }

    // Block is in west chunk
    if(x < 0 && w != nullptr) { return w->m_blocks(CHUNK_SIZE_X - 1, y, z); }
    
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
    if(GetBlock(x, y, z + 1, n,s,e,w) == BLOCK_AIR)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_FRONT);
    }

    // Back
    if(GetBlock(x, y, z - 1, n,s,e,w) == BLOCK_AIR)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_BACK);
    }

    // Left
    if(GetBlock(x - 1, y, z, n,s,e,w) == BLOCK_AIR)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_LEFT);
    }

    // Right
    if(GetBlock(x + 1, y, z, n,s,e,w) == BLOCK_AIR)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_RIGHT);
    }

    // Top
    if(GetBlock(x, y + 1, z, n,s,e,w) == BLOCK_AIR)
    {
        GenBlockFace(xPos,yPos,zPos, block, BLOCK_SIDE_TOP);
    }

    // Bottom
    if(GetBlock(x, y - 1, z, n,s,e,w) == BLOCK_AIR)
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
            m_vb.GetData().push_back( { x+BS, y+BS, z+BS, u1,v1, 0.8f} );
            m_vb.GetData().push_back( { x+BS, y,    z+BS, u1,v2, 0.8f} );
            m_vb.GetData().push_back( { x,    y,    z+BS, u2,v2, 0.8f} );
            m_vb.GetData().push_back( { x,    y+BS, z+BS, u2,v1, 0.8f} );

            m_solidFaces.push_back( { index, {x+HALF_BS, y+HALF_BS, z+BS} } );
            break;
        
        case BLOCK_SIDE_BACK:
            m_vb.GetData().push_back( { x,    y+BS, z, u1,v1, 0.8f} );
            m_vb.GetData().push_back( { x,    y,    z, u1,v2, 0.8f} );
            m_vb.GetData().push_back( { x+BS, y,    z, u2,v2, 0.8f} );
            m_vb.GetData().push_back( { x+BS, y+BS, z, u2,v1, 0.8f} );

            m_solidFaces.push_back( { index, {x+HALF_BS, y+HALF_BS, z} } );
            break;
        
        case BLOCK_SIDE_LEFT:
            m_vb.GetData().push_back( { x, y+BS, z+BS, u1,v1, 0.9f} );
            m_vb.GetData().push_back( { x, y,    z+BS, u1,v2, 0.9f} );
            m_vb.GetData().push_back( { x, y,    z,    u2,v2, 0.9f} );
            m_vb.GetData().push_back( { x, y+BS, z,    u2,v1, 0.9f} );

            m_solidFaces.push_back( { index, {x, y+HALF_BS, z+HALF_BS} } );
            break;
        
        case BLOCK_SIDE_RIGHT:
            m_vb.GetData().push_back( { x+BS, y+BS, z,    u1,v1, 0.9f} );
            m_vb.GetData().push_back( { x+BS, y,    z,    u1,v2, 0.9f} );
            m_vb.GetData().push_back( { x+BS, y,    z+BS, u2,v2, 0.9f} );
            m_vb.GetData().push_back( { x+BS, y+BS, z+BS, u2,v1, 0.9f} );

            m_solidFaces.push_back( { index, {x+BS, y+HALF_BS, z+HALF_BS} } );
            break;
        
        case BLOCK_SIDE_TOP:
            m_vb.GetData().push_back( { x,    y+BS, z+BS, u1,v1, 1.0f} );
            m_vb.GetData().push_back( { x,    y+BS, z,    u1,v2, 1.0f} );
            m_vb.GetData().push_back( { x+BS, y+BS, z,    u2,v2, 1.0f} );
            m_vb.GetData().push_back( { x+BS, y+BS, z+BS, u2,v1, 1.0f} );

            m_solidFaces.push_back( { index, {x+HALF_BS, y+BS, z+HALF_BS} } );
            break;
        
        case BLOCK_SIDE_BOTTOM:
            m_vb.GetData().push_back( { x,    y, z,    u1,v1, 0.6f} );
            m_vb.GetData().push_back( { x,    y, z+BS, u1,v2, 0.6f} );
            m_vb.GetData().push_back( { x+BS, y, z+BS, u2,v2, 0.6f} );
            m_vb.GetData().push_back( { x+BS, y, z,    u2,v1, 0.6f} );

            m_solidFaces.push_back( { index, {x+HALF_BS, y, z+HALF_BS} } );
            break;
    }
}