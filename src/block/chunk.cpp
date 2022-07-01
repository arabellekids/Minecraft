#include <algorithm>
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
    //std::cout << "pos = { x = " << pos.x << ", y = " << pos.y << " }\n";
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
                    if(y < 32)
                    {
                        m_blocks(x,y,z) = BLOCK_WATER;
                    }
                    else
                    {
                        m_blocks(x,y,z) = BLOCK_AIR;
                    }
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
                const auto& data = GetBlockData(m_blocks(x,y,z));
                if(data.type == BlockType::Solid)
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
    m_transparentFaces.clear();

    std::array< glm::ivec3, 6> directions = {
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
                const auto& block = GetBlockData( m_blocks(x, y, z) );
                
                std::array<const BlockData*, 6> neighbors;

                auto& faces = (block.type != BlockType::Solid) ? m_transparentFaces : m_solidFaces;

                for(int side = 0; side < 6; ++side)
                {
                    neighbors[side] = &GetBlockData( GetBlock(
                        x + directions[side].x,
                        y + directions[side].y,
                        z + directions[side].z,
                        world
                    ) );
                }

                float height = (neighbors[BLOCK_SIDE_TOP]->type != block.type) ? block.height : 1.0f;

                for(int side = 0; side < 6; ++side)
                {
                    const auto& neighbor = *neighbors[side];

                    if(height == 1.0f || side != BLOCK_SIDE_TOP)
                    {
                        if(neighbor.type == block.type || (block.type != BlockType::Solid && neighbor.type == BlockType::Solid)) { continue; }
                    }
                    
                    float u = GetBlockFaceU(block, side);
                    float v = GetBlockFaceV(block, side);

                    // Add the block face, to mabye later be sorted
                    unsigned int faceIndex = m_vb.GetData().size() >> 2;
                    faces.push_back( BlockFace(faceIndex, glm::vec3(x+blockFaces[side].x, y+blockFaces[side].y*height, z+blockFaces[side].z)) );

                    // Add the 4 vertices of the quad
                    for(int i = 0; i < 4; ++i)
                    {
                        m_vb.GetData().push_back( {
                            blockVerts[side][i].x + x, blockVerts[side][i].y*height + y, blockVerts[side][i].z + z,
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

    RegenerateTransparency(pPos);
}

void Chunk::SortFaces(const glm::vec3& pPos)
{
    for(auto i = 0; i < m_transparentFaces.size(); ++i)
    {
        auto& face = m_transparentFaces[i];
        glm::vec3 toFace(face.center.x - pPos.x, face.center.y - pPos.y, face.center.z - pPos.z);
        m_transparentFaces[i].dist = glm::dot(toFace, toFace);
    }

    std::sort(m_transparentFaces.begin(), m_transparentFaces.end());
}

void Chunk::RegenerateTransparency(const glm::vec3& pPos)
{
    m_transparentIB.GetData().clear();
    
    // ToDo: sort the faces
    SortFaces(pPos);
    
    for(auto i = 0; i < m_transparentFaces.size(); ++i)
    {
        auto index = m_transparentFaces[i].index << 2;
        m_transparentIB.GetData().push_back(index);
        m_transparentIB.GetData().push_back(index+1);
        m_transparentIB.GetData().push_back(index+2);

        m_transparentIB.GetData().push_back(index);
        m_transparentIB.GetData().push_back(index+2);
        m_transparentIB.GetData().push_back(index+3);
    }

    m_transparentIB.DynamicBufferData(m_transparentIB.GetData(), false);
}

unsigned char Chunk::GetBlock(int x, int y, int z, const World& world) const
{
    if(x >= 0 && y >= 0 && z >= 0 && x < CHUNK_SIZE_X && y < CHUNK_SIZE_Y && z < CHUNK_SIZE_Z)
    {
        return m_blocks(x,y,z);
    }

    return world.GetBlockFromIndex(x + m_pos.x * CHUNK_SIZE_X, y, z + m_pos.y * CHUNK_SIZE_Z);
}

void Chunk::SetBlock(int x, int y, int z, unsigned char block, World& world)
{
    if(x >= 0 && y >= 0 && z >= 0 && x < CHUNK_SIZE_X && y < CHUNK_SIZE_Y && z < CHUNK_SIZE_Z)
    {
        m_blocks(x,y,z) = block;
        return;
    }

    world.SetBlockFromIndex(x + m_pos.x * CHUNK_SIZE_X, y, z + m_pos.y * CHUNK_SIZE_Z, block);
}