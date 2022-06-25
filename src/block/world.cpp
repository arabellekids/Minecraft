#include <GLES3/gl32.h>
#include <glm/ext/matrix_transform.hpp>

#include <iostream>

#include "../object/player.h"
#include "../settings/settings.h"
#include "world.h"

#include "../input/input.h"

ChunkLoader::ChunkLoader(long xPos, long zPos, int xChunk, int yChunk) : pos(xPos, zPos), chunkIndex(xChunk, yChunk) {}

World::World() : m_chunks(0, 0), m_blockShader("assets/shaders/test/vert.glsl", "assets/shaders/test/frag.glsl"), m_blockAtlasTex("assets/textures/Minecraft-atlas.png", GL_NEAREST)
{
    m_blockShader.SetUniform1i("u_tex", 0);

    m_chunks.Resize(Settings::viewDist * 2 + 1, Settings::viewDist * 2 + 1);
    m_loadingChunks.clear();

    // First load the chunks
    for(auto y = 0; y < m_chunks.GetYSize(); ++y)
    {
        for(auto x = 0; x < m_chunks.GetXSize(); ++x)
        {
            m_chunks(x,y) = std::make_unique<Chunk>();
            m_chunks(x,y)->Load( { x, y } );
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
        
        m_chunks(chunkX, chunkY)->Load(m_loadingChunks[i]->pos);
        GenChunkBuffers(chunkX, chunkY, player.GetPos());

        // Update the chunks 4 neighbors
        
        if(chunkX > 0 && !m_chunks(chunkX - 1, chunkY)->GetLoading()) { GenChunkBuffers(chunkX - 1, chunkY, player.GetPos()); }
        if((chunkX + 1) < m_chunks.GetXSize() && !m_chunks(chunkX + 1, chunkY)->GetLoading()) { GenChunkBuffers(chunkX + 1, chunkY, player.GetPos()); }
        if(chunkY > 0 && !m_chunks(chunkX, chunkY - 1)->GetLoading()) { GenChunkBuffers(chunkX, chunkY - 1, player.GetPos()); }
        if((chunkY + 1) < m_chunks.GetYSize() && !m_chunks(chunkX, chunkY + 1)->GetLoading()) { GenChunkBuffers(chunkX, chunkY + 1, player.GetPos()); }

        m_chunks(m_loadingChunks[i]->chunkIndex.x, m_loadingChunks[i]->chunkIndex.y)->SetLoading(false);
        
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
    //if(x >= m_chunks.GetXSize() || y >= m_chunks.GetYSize() || x < 0 || y < 0) { return; }
    
    Chunk* n = ((y + 1) < m_chunks.GetYSize() && m_chunks(x, y + 1)->GetLoading() == false) ? m_chunks(x, y + 1).get() : nullptr;
    
    Chunk* s = ((y - 1) >= 0 && m_chunks(x, y - 1)->GetLoading() == false) ? m_chunks(x, y - 1).get() : nullptr;
    
    Chunk* e = ((x + 1) < m_chunks.GetXSize() && m_chunks(x + 1, y)->GetLoading() == false) ? m_chunks(x + 1, y).get() : nullptr;
    
    Chunk* w =  ((x - 1) >= 0 && m_chunks(x - 1, y)->GetLoading() == false) ? m_chunks(x - 1, y).get() : nullptr;
    
    m_chunks(x,y)->GenerateVertices(n,s,e,w);
    //m_chunks(x,y)->GenerateVertices(nullptr, nullptr, nullptr, nullptr);
    m_chunks(x,y)->GenerateIndices( pPos );
}

void World::ShiftGrid(BlockSide dir, Player& player)
{
    switch (dir)
    {
        case BLOCK_SIDE_FRONT:
            player.GetPos().z -= (CHUNK_SIZE_Z * BS);

            for(int z = 0; z < m_chunks.GetYSize() - 1; ++z)
            {
                for(int x = 0; x < m_chunks.GetXSize(); ++x)
                {
                    std::swap(m_chunks(x, z), m_chunks(x, z + 1));
                }
            }

            for(int x = 0; x < m_chunks.GetXSize(); ++x)
            {
                QueueChunk(m_chunks(x, m_chunks.GetYSize() - 1)->GetPos().x,
                m_chunks(x, m_chunks.GetYSize() - 1)->GetPos().y + m_chunks.GetYSize(),
                x, m_chunks.GetYSize() - 1);
            }

            break;
        
        case BLOCK_SIDE_BACK:
            player.GetPos().z += (CHUNK_SIZE_Z * BS);

            for(int z = m_chunks.GetYSize() - 1; z > 0; --z)
            {
                for(int x = 0; x < m_chunks.GetXSize(); ++x)
                {
                    std::swap(m_chunks(x, z), m_chunks(x, z - 1));
                }
            }
            
            for(int x = 0; x < m_chunks.GetXSize(); ++x)
            {
                QueueChunk(m_chunks(x, 0)->GetPos().x,
                m_chunks(x, 0)->GetPos().y + m_chunks.GetYSize(),
                x, 0);
            }

            break;
        
        case BLOCK_SIDE_LEFT:
            player.GetPos().x += (CHUNK_SIZE_X * BS);

            for(int x = m_chunks.GetXSize() - 1; x > 0; --x)
            {
                for(int z = 0; z < m_chunks.GetYSize(); ++z)
                {
                    std::swap(m_chunks(x, z), m_chunks(x - 1, z));
                }
            }

            for(int z = 0; z < m_chunks.GetYSize(); ++z)
            {
                QueueChunk(m_chunks(0, z)->GetPos().x - m_chunks.GetXSize(),
                m_chunks(0, z)->GetPos().y,
                0, z);
            }

            break;
        
        case BLOCK_SIDE_RIGHT:
            player.GetPos().x -= (CHUNK_SIZE_X * BS);

            for(int x = 0; x < m_chunks.GetXSize() - 1; ++x)
            {
                for(int z = 0; z < m_chunks.GetYSize(); ++z)
                {
                    std::swap(m_chunks(x, z), m_chunks(x + 1, z));
                }
            }

            for(int z = 0; z < m_chunks.GetYSize(); ++z)
            {
                QueueChunk(m_chunks(m_chunks.GetXSize() - 1, z)->GetPos().x + m_chunks.GetXSize(),
                m_chunks(m_chunks.GetXSize() - 1, z)->GetPos().y,
                m_chunks.GetXSize() - 1, z);
            }

            break;
    }
}