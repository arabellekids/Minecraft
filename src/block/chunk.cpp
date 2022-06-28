#include <cmath>
#include <GLES3/gl32.h>
#include <iostream>

#include "chunk.h"
#include "../settings/settings.h"
#include "world.h"

#define max(x,y) (x > y) ? (x) : (y)
#define min(x,y) (x < y) ? (x) : (y)
#define abs(x) ((x) > 0) ? (x) : (-x)

Chunk::Chunk() : m_pos(0, 0), m_blocks(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z)
{
    m_isLoading = false;
    
    BufferLayout layout;
    layout.Push<float>(3); // x,y,z
    layout.Push<float>(2); // u,v
    layout.Push<float>(1); // light
    
    m_va.AddBuffer(m_vb, layout);
}

Chunk::Chunk(const glm::ivec2& pos) : m_pos(pos), m_blocks(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z)
{
    m_isLoading = false;
    
    BufferLayout layout;
    layout.Push<float>(3); // x,y,z
    layout.Push<float>(2); // u,v
    layout.Push<float>(1); // light
    
    m_va.AddBuffer(m_vb, layout);
}

Chunk::~Chunk() {}

void Chunk::Load(const glm::i64vec2& pos)
{
    //std::cout << "m_pos = { x = " << m_pos.x << ", y = " << m_pos.y << " }\n";
    long chunkX = (m_pos.x + pos.x) * CHUNK_SIZE_X;
    long chunkZ = (m_pos.y + pos.y) * CHUNK_SIZE_Z;

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

void Chunk::GenerateVertices(const World& world)
{
    m_vb.GetData().clear();
    m_solidFaces.clear();

    std::array< glm::ivec3, 6> neighbors = {
        glm::vec3( 0,  0,  1),
        glm::vec3( 0,  0, -1),
        glm::vec3(-1,  0,  0),
        glm::vec3( 1,  0,  0),
        glm::vec3( 0,  1,  0),
        glm::vec3( 0, -1,  0)
    };

    // The vertices and face centers (used to create a BlockFace object for sorting) of a block mesh
    const auto& blockVerts = GetBlockVertices();
    const auto& blockFaces = GetBlockFaceCenters();

    // +Y = up, +X = right, +Z = forward
    for(int z = 0; z < CHUNK_SIZE_Z; ++z)
    {
        for(int y = 0; y < CHUNK_SIZE_Y; ++y)
        {
            for(int x = 0; x < CHUNK_SIZE_X; ++x)
            {
                if(m_blocks(x, y, z) == BLOCK_AIR) { continue; }
                const BlockType& block = GetBlockType( m_blocks(x, y, z) );
                
                for(int side = 0; side < 6; ++side)
                {
                    const BlockType& neighbor = GetBlockType( GetBlock(
                        x + neighbors[side].x,
                        y + neighbors[side].y,
                        z + neighbors[side].z,
                        world
                    ) );
                    if(neighbor.isSolid) { continue; }

                    float u = GetBlockFaceU(block, side);
                    float v = GetBlockFaceV(block, side);

                    // Add the block face, to mabye later be sorted
                    unsigned int faceIndex = m_vb.GetData().size() >> 2;
                    m_solidFaces.push_back( {faceIndex, blockFaces[side] } );

                    // Add the 4 vertices of the quad
                    for(int i = 0; i < 4; ++i)
                    {
                        m_vb.GetData().push_back( {
                            blockVerts[side][i].x + x, blockVerts[side][i].y + y, blockVerts[side][i].z + z,
                            blockVerts[side][i].u + u, blockVerts[side][i].v + v,
                            blockVerts[side][i].light
                        } );
                    } // end loop i

                } // end loop side
            }  // end loop x
        }  // end loop y
    } // end loop z

    // Update the OpenGL vertex buffer data
    m_vb.DynamicBufferData(m_vb.GetData(), false);
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

    m_solidIB.DynamicBufferData(m_solidIB.GetData(), false);
}

unsigned char Chunk::GetBlock(int x, int y, int z, const World& world) const
{
    if(x >= 0 && y >= 0 && z >= 0 && x < CHUNK_SIZE_X && y < CHUNK_SIZE_Y && z < CHUNK_SIZE_Z)
    {
        return m_blocks(x,y,z);
    }

    return world.GetBlock(x + m_pos.x * CHUNK_SIZE_X, y, z + m_pos.y * CHUNK_SIZE_Z);
}

void Chunk::SetBlock(int x, int y, int z, unsigned char block, World& world)
{
    if(x >= 0 && y >= 0 && z >= 0 && x < CHUNK_SIZE_X && y < CHUNK_SIZE_Y && z < CHUNK_SIZE_Z)
    {
        m_blocks(x,y,z) = block;
        return;
    }

    world.SetBlock(x + m_pos.x * CHUNK_SIZE_X, y, z + m_pos.y * CHUNK_SIZE_Z, block);
}