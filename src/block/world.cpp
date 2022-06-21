#include <GLES3/gl32.h>

#include <glm/ext/matrix_transform.hpp>

#include "../object/player.h"
#include "../settings/settings.h"
#include "world.h"

World* World::s_pInstance = nullptr;

World::World() : m_chunks(0, 0), m_blockShader("assets/shaders/test/vert.glsl", "assets/shaders/test/frag.glsl"), m_blockAtlasTex("assets/textures/Minecraft-atlas.png", GL_NEAREST)
{
    World::s_pInstance = this;

    m_blockShader.SetUniform1i("u_tex", 0);

    m_chunks.Resize(Settings::viewDist * 2 + 1, Settings::viewDist * 2 + 1);

    for(auto y = 0; y < m_chunks.GetYSize(); ++y)
    {
        for(auto x = 0; x < m_chunks.GetXSize(); ++x)
        {
            m_chunks(x,y).Load( { x, y } );

            m_chunks(x,y).GenerateVertices();
            m_chunks(x,y).GenerateIndices( { 0.0f, 0.0f, 0.0f } );
        }
    }
}

World::~World() {}

void World::Update(Player& player)
{
    // Player crossed left
    while(player.GetPos().x < 0.0f)
    {
        ShiftGrid(BLOCK_SIDE_LEFT, player);
    }

    // Player crossed right
    while(player.GetPos().x > (CHUNK_SIZE_X * BS))
    {
        ShiftGrid(BLOCK_SIDE_RIGHT, player);
    }

    // Player crossed forward
    while(player.GetPos().z > (CHUNK_SIZE_Z * BS))
    {
        ShiftGrid(BLOCK_SIDE_FRONT, player);
    }

    // Player crossed backward
    while(player.GetPos().z < 0.0f)
    {
        ShiftGrid(BLOCK_SIDE_BACK, player);
    }
    
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
                    m_chunks(x, z) = m_chunks(x, z + 1);
                }
            }

            for(int x = 0; x < m_chunks.GetXSize(); ++x)
            {
                glm::vec2 pos = { m_chunks(x, m_chunks.GetYSize() - 1).GetPos().x, m_chunks(x, m_chunks.GetYSize() - 1).GetPos().y + 1 };
                m_chunks(x, m_chunks.GetYSize() - 1).Load(pos);
            }

            break;
        
        case BLOCK_SIDE_BACK:
            player.GetPos().z += (CHUNK_SIZE_Z * BS);

            for(int z = m_chunks.GetYSize() - 1; z > 0; --z)
            {
                for(int x = 0; x < m_chunks.GetXSize(); ++x)
                {
                    m_chunks(x, z) = m_chunks(x, z - 1);
                }
            }

            for(int x = 0; x < m_chunks.GetXSize(); ++x)
            {
                glm::vec2 pos = { m_chunks(x, 0).GetPos().x, m_chunks(x, 0).GetPos().y - 1 };
                m_chunks(x, 0).Load(pos);
            }

            break;
        
        case BLOCK_SIDE_LEFT:
            player.GetPos().x += (CHUNK_SIZE_X * BS);

            for(int x = m_chunks.GetXSize() - 1; x > 0; --x)
            {
                for(int z = 0; z < m_chunks.GetYSize(); ++z)
                {
                    m_chunks(x, z) = m_chunks(x - 1, z);
                }
            }

            for(int z = 0; z < m_chunks.GetYSize(); ++z)
            {
                glm::vec2 pos = { m_chunks(0, z).GetPos().x - 1, m_chunks(0, z).GetPos().y };
                m_chunks(0, z).Load(pos);
            }

            break;
        
        case BLOCK_SIDE_RIGHT:
            player.GetPos().x -= (CHUNK_SIZE_X * BS);

            for(int x = 0; x < m_chunks.GetXSize() - 1; ++x)
            {
                for(int z = 0; z < m_chunks.GetYSize(); ++z)
                {
                    m_chunks(x, z) = m_chunks(x + 1, z);
                }
            }

            for(int z = 0; z < m_chunks.GetYSize(); ++z)
            {
                glm::vec2 pos = { m_chunks(m_chunks.GetXSize() - 1, z).GetPos().x + 1, m_chunks(m_chunks.GetXSize() - 1, z).GetPos().y };
                m_chunks(m_chunks.GetXSize() - 1, z).Load(pos);
            }

            break;
    }
}

void World::RenderSolid(const glm::mat4& vp)
{
    for(int z = 0; z < m_chunks.GetYSize(); ++z)
    {
        for(int x = 0; x < m_chunks.GetXSize(); ++x)
        {
            m_blockShader.Bind();
            m_blockAtlasTex.Bind();

            glm::mat4 model = glm::translate(glm::identity<glm::mat4>(), { (x - m_chunks.GetXSize()*0.5f) *CHUNK_SIZE_X*BS, 0, (z - m_chunks.GetYSize()*0.5f) *CHUNK_SIZE_Z*BS });
            m_blockShader.SetUniformMat4("u_mvp", vp * model);

            m_chunks(x,z).GetVa().Bind();
            m_chunks(x,z).GetSolidIb().Bind();

            glDrawElements(GL_TRIANGLES, m_chunks(x,z).GetSolidIb().GetData().size(), GL_UNSIGNED_SHORT, nullptr);

        }
    }
}