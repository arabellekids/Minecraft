#include <algorithm>

#include <GLES3/gl32.h>
#include <iostream>

#define STB_DEFINE
#include <stb/stb.h>

#include "chunk.h"
#include "../settings/settings.h"
#include "world.h"

#define max(x,y) (x > y) ? (x) : (y)
#define min(x,y) (x < y) ? (x) : (y)
#define abs(x) ((x) > 0) ? (x) : (-x)

Chunk::Chunk() : m_pos(0, 0), m_toDoBlocks({{ {}, {}, {}, {} }}), m_blocks(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z)
{
    m_isLoading = false;
    m_modified = true;
    
    BufferLayout layout;
    layout.Push<float>(3); // x,y,z
    layout.Push<float>(2); // u,v
    layout.Push<float>(1); // light
    
    m_va.AddBuffer(m_vb, layout);
}

Chunk::Chunk(const glm::ivec2& pos) : m_pos(pos), m_toDoBlocks({{ {}, {}, {}, {} }}), m_blocks(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z)
{
    m_isLoading = false;
    m_modified = true;
    
    BufferLayout layout;
    layout.Push<float>(3); // x,y,z
    layout.Push<float>(2); // u,v
    layout.Push<float>(1); // light
    
    m_va.AddBuffer(m_vb, layout);
}

Chunk::~Chunk() {}

void Chunk::Load(const glm::i64vec2& pos)
{
    LoadBaseTerrain(pos);
    LoadLayers(pos);
    LoadDetails(pos);
}

void Chunk::LoadBaseTerrain(const glm::i64vec2& pos)
{
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
}

void Chunk::LoadLayers(const glm::i64vec2& pos)
{
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

void Chunk::LoadDetails(const glm::i64vec2& pos)
{
    stb_srand((m_pos.x + pos.x) + (m_pos.y + pos.y));

    for(int z = 0; z < CHUNK_SIZE_Z; ++z)
    {
        for(int x = 0; x < CHUNK_SIZE_X; ++x)
        {
            bool hasTree = (stb_frand() < 0.01);
            if(!hasTree) { continue; }

            for(int y = CHUNK_SIZE_Y - 1; y > 0; --y)
            {
                if(m_blocks(x,y,z) == BLOCK_GRASS)
                {
                    m_blocks(x,y,z) = BLOCK_DIRT;

                    GenTree(x, y + 1, z);
                    break;
                }
            }
        }
    }

}

void Chunk::GenTree(int x, int y, int z)
{
    // LogHeight is from 4-6 blocks
    int logHeight = stb_frand() * 2 + 4;
    
    // Generate the log
    for(int i = 0; i < logHeight; ++i)
    {
        LoadBlock(x, y + i, z, BLOCK_OAKLOG);
    }

    // Generate the leaves
    float leafRadius = stb_frand() * 2 + 2;
    glm::ivec3 logTipPos(x, y + logHeight - 1, z);

    for(int lx = -leafRadius; lx <= leafRadius; ++lx)
    {
        for(int lz = -leafRadius; lz <= leafRadius; ++lz)
        {
            for(int ly = 0; ly <= leafRadius; ++ly)
            {
                glm::ivec3 leafPos(x + lx, logTipPos.y + ly, z + lz );
                glm::vec3 toTip = logTipPos - leafPos;
                float dist = glm::dot(toTip, toTip);

                if(dist > 0 && dist < leafRadius*leafRadius)
                {
                    LoadBlock(leafPos.x, leafPos.y, leafPos.z, BLOCK_OAKLEAVES);
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

void Chunk::UpdateToDoList(World& world)
{
    //int listLength = m_toDoBlocks.size();
    for(int side = 0; side < 4; ++side)
    {
        int xOffset = 0;
        int yOffset = 0;

        if(side == BLOCK_SIDE_FRONT) { xOffset = 0; yOffset = 1; }
        else if(side == BLOCK_SIDE_BACK) { xOffset = 0; yOffset = -1; }
        else if(side == BLOCK_SIDE_LEFT) { xOffset = -1; yOffset = 0; }
        else if(side == BLOCK_SIDE_RIGHT) { xOffset = 1; yOffset = 0; }

        glm::ivec2 blockChunk(m_pos.x + xOffset, m_pos.y + yOffset);

        // Chunk is valid to set blocks
        if(world.IsValidChunk(blockChunk.x, blockChunk.y) && !m_toDoBlocks[side].empty())
        {
            for(int i = 0; i < m_toDoBlocks[side].size(); ++i)
            {
                glm::ivec3 setPos(m_toDoBlocks[side][i].pos.x + m_pos.x * CHUNK_SIZE_X,
                m_toDoBlocks[side][i].pos.y,
                m_toDoBlocks[side][i].pos.z + m_pos.y * CHUNK_SIZE_Z);

                world.SetBlockFromIndex(setPos.x, setPos.y, setPos.z, m_toDoBlocks[side][i].block);
            }
            m_toDoBlocks[side].clear();
        }
    }
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
        m_modified = true;
        return;
    }

    world.SetBlockFromIndex(x + m_pos.x * CHUNK_SIZE_X, y, z + m_pos.y * CHUNK_SIZE_Z, block);
}

void Chunk::LoadBlock(int x, int y, int z, unsigned char block)
{
    if(y < 0 || y >= CHUNK_SIZE_Y) { return; }
    
    if(x >= 0 && z >= 0 && x < CHUNK_SIZE_X && z < CHUNK_SIZE_Z)
    {
        m_blocks(x,y,z) = block;
    }

    // Todo: Add out of bounds blocks to a list to load later, like tree leaves
    if(x < 0) { m_toDoBlocks[BLOCK_SIDE_LEFT].push_back( {block, {x, y, z}} ); }
    else if(x >= CHUNK_SIZE_X) { m_toDoBlocks[BLOCK_SIDE_RIGHT].push_back( {block, {x, y, z}} ); }
    else if(z < 0) { m_toDoBlocks[BLOCK_SIDE_BACK].push_back( {block, {x, y, z}} ); }
    else if(z >= CHUNK_SIZE_Z) { m_toDoBlocks[BLOCK_SIDE_FRONT].push_back( {block, {x, y, z}} ); }
}