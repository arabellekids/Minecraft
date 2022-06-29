#include <GLES3/gl32.h>
#include <glm/ext/matrix_transform.hpp>

#include <iostream>

#include "../app/app.h"
#include "../object/player.h"
#include "../ray/ray.h"
#include "../settings/settings.h"
#include "world.h"

#include "../input/input.h"

#define abs(x) ((x) > 0) ? (x) : (-x)

ChunkLoader::ChunkLoader(long xPos, long zPos, int xChunk, int yChunk) : pos(xPos, zPos), chunkIndex(xChunk, yChunk) {}

World::World() : m_chunks(0, 0), m_blockShader("assets/shaders/test/vert.glsl", "assets/shaders/test/frag.glsl"), m_blockAtlasTex("assets/textures/Minecraft-atlas.png", GL_NEAREST)
{
    m_offset.x = 0;
    m_offset.y = 0;
    
    m_blockShader.SetUniform1i("u_tex", 0);

    m_chunks.Resize(Settings::viewDist * 2 + 1, Settings::viewDist * 2 + 1);
    m_loadingChunks.clear();

    // First load the chunks
    for(auto y = 0; y < m_chunks.GetYSize(); ++y)
    {
        for(auto x = 0; x < m_chunks.GetXSize(); ++x)
        {
            m_chunks(x,y) = std::make_unique<Chunk>( glm::ivec2(x,y) );
            m_chunks(x,y)->Load( { x,y } );
        }
    }

    // Then generate the buffers
    for(auto y = 0; y < m_chunks.GetYSize(); ++y)
    {
        for(auto x = 0; x < m_chunks.GetXSize(); ++x)
        {
            GenChunkBuffers(x,y, { 0.0f, 0.0f, 0.0f } );
        }
    }
}

World::~World() {}

void World::Update(Player& player)
{
    // if(Input::Instance().GetKey(SDL_SCANCODE_UP))
    // {
    //     ShiftGrid(BLOCK_SIDE_FRONT, player);
    // }
    // if(Input::Instance().GetKey(SDL_SCANCODE_DOWN))
    // {
    //     ShiftGrid(BLOCK_SIDE_BACK, player);
    // }
    // if(Input::Instance().GetKey(SDL_SCANCODE_LEFT))
    // {
    //     ShiftGrid(BLOCK_SIDE_LEFT, player);
    // }
    // if(Input::Instance().GetKey(SDL_SCANCODE_RIGHT))
    // {
    //     ShiftGrid(BLOCK_SIDE_RIGHT, player);
    // }
    
    // return;

    // Player crossed forward
    while(player.GetPos().z > (CHUNK_SIZE_Z * BS * 0.5f))
    {
        ShiftGrid(BLOCK_SIDE_FRONT, player);
    }

    // Player crossed backward
    while(player.GetPos().z < -(CHUNK_SIZE_Z * BS * 0.5f))
    {
        ShiftGrid(BLOCK_SIDE_BACK, player);
    }

    // Player crossed left
    while(player.GetPos().x < -(CHUNK_SIZE_X * BS * 0.5f))
    {
        ShiftGrid(BLOCK_SIDE_LEFT, player);
    }

    // Player crossed right
    while(player.GetPos().x > (CHUNK_SIZE_X * BS * 0.5f))
    {
        ShiftGrid(BLOCK_SIDE_RIGHT, player);
    }

    LoadChunks(player);
}

void World::RenderSolid(const glm::mat4& vp)
{
    m_blockShader.Bind();
    m_blockAtlasTex.Bind();
    
    if(!Settings::wireframe)
    {
        for(int z = 0; z < m_chunks.GetYSize(); ++z)
        {
            for(int x = 0; x < m_chunks.GetXSize(); ++x)
            {
                if(m_chunks(x,z)->GetLoading() == true) { continue; }
                
                glm::mat4 model = glm::translate(glm::identity<glm::mat4>(), { (x - m_chunks.GetXSize()*0.5f) *CHUNK_SIZE_X*BS, 0, (z - m_chunks.GetYSize()*0.5f) *CHUNK_SIZE_Z*BS });
                m_blockShader.SetUniformMat4("u_mvp", vp * model);

                m_chunks(x,z)->GetVa().Bind();
                m_chunks(x,z)->GetSolidIb().Bind();

                glDrawElements(GL_TRIANGLES, m_chunks(x,z)->GetSolidIb().GetData().size(), GL_UNSIGNED_SHORT, nullptr);
            }
        }
    }
    else
    {
        for(int z = 0; z < m_chunks.GetYSize(); ++z)
        {
            for(int x = 0; x < m_chunks.GetXSize(); ++x)
            {
                if(m_chunks(x,z)->GetLoading() == true) { continue; }
                
                glm::mat4 model = glm::translate(glm::identity<glm::mat4>(), { (x - m_chunks.GetXSize()*0.5f) *CHUNK_SIZE_X*BS, 0, (z - m_chunks.GetYSize()*0.5f) *CHUNK_SIZE_Z*BS });
                m_blockShader.SetUniformMat4("u_mvp", vp * model);

                m_chunks(x,z)->GetVa().Bind();
                m_chunks(x,z)->GetSolidIb().Bind();

                glDrawElements(GL_LINES, m_chunks(x,z)->GetSolidIb().GetData().size(), GL_UNSIGNED_SHORT, nullptr);

            }
        }
    }
}

void World::LoadChunks(Player& player)
{
    int i = m_loadingChunks.size() - 1;
    for(int j = 0; j < m_chunkLoadLimit; ++j, --i)
    {
        if(i < 0) { return; }
        
        int chunkX = m_loadingChunks[i]->chunkIndex.x;
        int chunkY = m_loadingChunks[i]->chunkIndex.y;
        
        m_chunks(chunkX, chunkY)->SetLoading(false);

        m_chunks(chunkX, chunkY)->Load(m_loadingChunks[i]->pos);
        GenChunkBuffers(chunkX, chunkY, player.GetPos());

        // Update the chunks 4 neighbors
        
        if(IsValidChunk(chunkX - 1, chunkY)) { GenChunkBuffers(chunkX - 1, chunkY, player.GetPos()); }
        if(IsValidChunk(chunkX + 1, chunkY)) { GenChunkBuffers(chunkX + 1, chunkY, player.GetPos()); }
        if(IsValidChunk(chunkX, chunkY - 1)) { GenChunkBuffers(chunkX, chunkY - 1, player.GetPos()); }
        if(IsValidChunk(chunkX, chunkY + 1)) { GenChunkBuffers(chunkX, chunkY + 1, player.GetPos()); }
        
        m_loadingChunks.pop_back();
    }
}

void World::QueueChunk(long xPos, long zPos, int xChunk, int yChunk)
{
    m_chunks(xChunk, yChunk)->SetLoading(true);
    
    ChunkLoader * chunkToLoad = new ChunkLoader( xPos, zPos, xChunk, yChunk );
    m_loadingChunks.push_back( chunkToLoad );
}

void World::GenChunkBuffers(int x, int y, const glm::vec3& pPos)
{
    m_chunks(x,y)->GenerateVertices(*this);
    m_chunks(x,y)->GenerateIndices( pPos );
}

void World::GenNeighborChunkBuffers(int x, int y, const glm::vec3& pPos)
{
    if(IsValidChunk(x - 1, y)) { GenChunkBuffers(x - 1, y, pPos); }
    if(IsValidChunk(x + 1, y)) { GenChunkBuffers(x + 1, y, pPos); }
    if(IsValidChunk(x, y - 1)) { GenChunkBuffers(x, y - 1, pPos); }
    if(IsValidChunk(x, y + 1)) { GenChunkBuffers(x, y + 1, pPos); }
}

void World::ShiftGrid(BlockSide dir, Player& player)
{
    switch (dir)
    {
        case BLOCK_SIDE_FRONT:
            player.GetPos().z -= (CHUNK_SIZE_Z * BS);
            m_offset.y += 1;

            for(int z = 0; z < m_chunks.GetYSize() - 1; ++z)
            {
                for(int x = 0; x < m_chunks.GetXSize(); ++x)
                {
                    std::swap(m_chunks(x, z), m_chunks(x, z + 1));
                    m_chunks(x,z)->GetPos().y -= 1; // Make sure the chunk's pos stays the same
                }
            }

            for(int x = 0; x < m_chunks.GetXSize(); ++x)
            {
                m_chunks(x, m_chunks.GetYSize() - 1)->GetPos().y += m_chunks.GetYSize() - 1; // Make sure the chunk's pos stays the same
                QueueChunk(m_offset.x, m_offset.y, x, m_chunks.GetYSize() - 1);
            }

            break;
        
        case BLOCK_SIDE_BACK:
            player.GetPos().z += (CHUNK_SIZE_Z * BS);
            m_offset.y -= 1;

            for(int z = m_chunks.GetYSize() - 1; z > 0; --z)
            {
                for(int x = 0; x < m_chunks.GetXSize(); ++x)
                {
                    std::swap(m_chunks(x, z), m_chunks(x, z - 1));
                    m_chunks(x,z)->GetPos().y += 1; // Make sure the chunk's pos stays the same
                }
            }
            
            for(int x = 0; x < m_chunks.GetXSize(); ++x)
            {
                m_chunks(x, 0)->GetPos().y -= m_chunks.GetYSize() - 1; // Make sure the chunk's pos stays the same
                QueueChunk(m_offset.x, m_offset.y, x, 0);
            }

            break;
        
        case BLOCK_SIDE_LEFT:
            player.GetPos().x += (CHUNK_SIZE_X * BS);
            m_offset.x -= 1;

            for(int x = m_chunks.GetXSize() - 1; x > 0; --x)
            {
                for(int z = 0; z < m_chunks.GetYSize(); ++z)
                {
                    std::swap(m_chunks(x, z), m_chunks(x - 1, z));
                    m_chunks(x,z)->GetPos().x += 1; // Make sure the chunk's pos stays the same
                }
            }

            for(int z = 0; z < m_chunks.GetYSize(); ++z)
            {
                m_chunks(0, z)->GetPos().x -= m_chunks.GetXSize() - 1; // Make sure the chunk's pos stays the same
                QueueChunk(m_offset.x, m_offset.y, 0, z);
            }

            break;
        
        case BLOCK_SIDE_RIGHT:
            player.GetPos().x -= (CHUNK_SIZE_X * BS);
            m_offset.x += 1;

            for(int x = 0; x < m_chunks.GetXSize() - 1; ++x)
            {
                for(int z = 0; z < m_chunks.GetYSize(); ++z)
                {
                    std::swap(m_chunks(x, z), m_chunks(x + 1, z));
                    m_chunks(x,z)->GetPos().x -= 1; // Make sure the chunk's pos stays the same
                }
            }

            for(int z = 0; z < m_chunks.GetYSize(); ++z)
            {
                m_chunks(m_chunks.GetXSize() - 1, z)->GetPos().x += m_chunks.GetXSize() - 1; // Make sure the chunk's pos stays the same
                QueueChunk(m_offset.x, m_offset.y, m_chunks.GetXSize() - 1, z);
            }

            break;
    }
}

unsigned char World::GetBlockFromIndex(int x, int y, int z) const
{
    if(!IsValidBlock(x, y, z))
    {
        return BLOCK_AIR;
    }

    int xChunk = x / CHUNK_SIZE_X;
    int yChunk = z / CHUNK_SIZE_Z;

    return m_chunks(xChunk, yChunk)->GetBlock(x % CHUNK_SIZE_X, y, z % CHUNK_SIZE_Z, *this);
}

unsigned char World::GetBlockFromPos(float x, float y, float z) const
{
    auto index = PosToBlock(x, y, z);

    if(!IsValidBlock(index.x, index.y, index.z))
    {
        return BLOCK_AIR;
    }

    int xChunk = index.x / CHUNK_SIZE_X;
    int yChunk = index.z / CHUNK_SIZE_Z;

    return m_chunks(xChunk, yChunk)->GetBlock(index.x % CHUNK_SIZE_X, index.y, index.z % CHUNK_SIZE_Z, *this);
}

void World::SetBlockFromIndex(int x, int y, int z, unsigned char block)
{
    if(!IsValidBlock(x, y, z))
    {
        return;
    }

    int xChunk = x / CHUNK_SIZE_X;
    int yChunk = z / CHUNK_SIZE_Z;

    m_chunks(xChunk, yChunk)->SetBlock(x % CHUNK_SIZE_X, y, z % CHUNK_SIZE_Z, block, *this);

    GenChunkBuffers(xChunk, yChunk, App::Instance().GetPlayer().GetPos());

    // Regenerate neighbors
    GenNeighborChunkBuffers(xChunk, yChunk, App::Instance().GetPlayer().GetPos());
}

void World::SetBlockFromPos(float x, float y, float z, unsigned char block)
{
    auto index = PosToBlock(x, y, z);
    
    if(!IsValidBlock(index.x, index.y, index.z))
    {
        return;
    }

    int xChunk = index.x / CHUNK_SIZE_X;
    int yChunk = index.z / CHUNK_SIZE_Z;

    m_chunks(xChunk, yChunk)->SetBlock(index.x % CHUNK_SIZE_X, index.y, index.z % CHUNK_SIZE_Z, block, *this);

    GenChunkBuffers(xChunk, yChunk, App::Instance().GetPlayer().GetPos());

    // Regenerate neighbors
    GenNeighborChunkBuffers(xChunk, yChunk, App::Instance().GetPlayer().GetPos());
}

glm::ivec3 World::PosToBlock(float x, float y, float z) const
{
    int blockX = x + (m_chunks.GetXSize() * 0.5f) * CHUNK_SIZE_X;
    int blockY = y;
    int blockZ = z + (m_chunks.GetYSize() * 0.5f) * CHUNK_SIZE_Z;

    return glm::ivec3(blockX, blockY, blockZ);
}

glm::vec3 World::BlockToPos(int x, int y, int z) const
{
    float posX = x - (m_chunks.GetXSize() * 0.5f) * CHUNK_SIZE_X;
    float posY = y;
    float posZ = z - (m_chunks.GetYSize() * 0.5f) * CHUNK_SIZE_Z;

    return glm::vec3(posX, posY, posZ);
}

bool World::IsValidChunk(int x, int y) const
{
    return (x >= 0 && y >= 0 && x < m_chunks.GetXSize() && y < m_chunks.GetYSize() && !m_chunks(x,y)->GetLoading());
}

bool World::IsValidBlock(int x, int y, int z) const
{
    return (x >= 0 && y >= 0 && z >= 0 &&
    x < m_chunks.GetXSize() * CHUNK_SIZE_X && y < CHUNK_SIZE_Y && z < m_chunks.GetYSize() * CHUNK_SIZE_Z &&
    !m_chunks(x / CHUNK_SIZE_X, z / CHUNK_SIZE_Z)->GetLoading());
}

bool World::Raycast(const Ray& ray, BlockHitInfo& info) const
{
    glm::vec3 origin = ray.GetOrigin();
    glm::vec3 dir = ray.GetDirection();
    float length = ray.GetLength();
    
    glm::vec3 localPos(origin.x - std::floor(origin.x),
    origin.y - std::floor(origin.y),
    origin.z - std::floor(origin.z));

    glm::ivec3 startPos(std::floor(origin.x), std::floor(origin.y), std::floor(origin.z));

    std::array<AxisPlane, 3> planes = {{
        { {1, 0, 0}, 0.0f, 0.0f },
        { {0, 1, 0}, 0.0f, 0.0f },
        { {0, 0, 1}, 0.0f, 0.0f }
    }};

    glm::ivec3 cellPos(std::floor(origin.x), std::floor(origin.y), std::floor(origin.z));
    std::array<glm::ivec3, 2> cells = { cellPos, cellPos };

    int signX = (dir.x < 0) ? -1 : 1;
    int signY = (dir.y < 0) ? -1 : 1;
    int signZ = (dir.z < 0) ? -1 : 1;

    planes[0].dist = ( ((signX < 0) ? 0 : 1) - localPos.x ) / dir.x;
    planes[1].dist = ( ((signY < 0) ? 0 : 1) - localPos.y ) / dir.y;
    planes[2].dist = ( ((signZ < 0) ? 0 : 1) - localPos.z ) / dir.z;

    planes[0].inc = signX / dir.x;
    planes[1].inc = signY / dir.y;
    planes[2].inc = signZ / dir.z;

    planes[0].normal.x *= signX;
    planes[1].normal.y *= signY;
    planes[2].normal.z *= signZ;
    
    std::sort(planes.begin(), planes.end());

    while(planes[0].dist < length)
    {
        cells[1] = cells[0];
        cells[0] += planes[0].normal;
        
        auto blockIndex = PosToBlock(cells[0].x, cells[0].y, cells[0].z);

        if(!IsValidBlock(blockIndex.x, blockIndex.y, blockIndex.z)) { return false; }

        auto& block = GetBlockType( GetBlockFromIndex(blockIndex.x, blockIndex.y, blockIndex.z) );
        if(block.isSolid)
        {
            info.pos = cells[0];
            info.neighbor = cells[1];
            info.blockData = &block;
            return true;
        }

        //cellIndex = !cellIndex;
        planes[0].dist += planes[0].inc;

        if(planes[0].dist > planes[1].dist)
        {
            std::sort(planes.begin(), planes.end());
        }
    }

    return false;
}