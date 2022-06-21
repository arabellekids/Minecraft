#include <GLES3/gl32.h>

#include "../object/player.h"
#include "../settings/settings.h"
#include "world.h"

World* World::s_pInstance = nullptr;

// World::World() : m_chunks(0, 0), m_blockShader("assets/shaders/test/vert.glsl", "assets/shaders/test/frag.glsl"), m_blockAtlasTex("assets/textures/Minecraft-atlas.png", GL_NEAREST)
// {
//     World::s_pInstance = this;

//     m_chunks.Resize(Settings::viewDist * 2 + 1, Settings::viewDist * 2 + 1);

//     for(auto y = 0; y < m_chunks.GetYSize(); ++y)
//     {
//         for(auto x = 0; x < m_chunks.GetXSize(); ++x)
//         {
//             m_chunks(x,y).GenerateVertices();
//             m_chunks(x,y).GenerateIndices( { 0.0f, 0.0f, 0.0f } );
//         }
//     }
// }

World::World()
{
    World::s_pInstance = this;
}

World::~World() {}

void World::Update(Player& player)
{
    // Player crossed left
    while(player.GetPos().x <= 0.0f)
    {
        ShiftGrid(BLOCK_SIDE_LEFT, player);
    }

    // Player crossed right
    while(player.GetPos().x >= (CHUNK_SIZE_X * BS))
    {
        ShiftGrid(BLOCK_SIDE_RIGHT, player);
    }

    // Player crossed forward
    while(player.GetPos().z >= (CHUNK_SIZE_Z * BS))
    {
        ShiftGrid(BLOCK_SIDE_FRONT, player);
    }

    // Player crossed backward
    while(player.GetPos().z <= 0.0f)
    {
        ShiftGrid(BLOCK_SIDE_BACK, player);
    }
    
}

void World::ShiftGrid(BlockSide dir, Player& player)
{
    switch (dir)
    {
        case BLOCK_SIDE_FRONT:
            player.GetPos().z -= (CHUNK_SIZE_Z * 0.5f * BS);

            

            break;
        case BLOCK_SIDE_BACK:
            player.GetPos().z += (CHUNK_SIZE_Z * 0.5f * BS);
            break;
        case BLOCK_SIDE_LEFT:
            player.GetPos().x += (CHUNK_SIZE_X * 0.5f * BS);
            break;
        case BLOCK_SIDE_RIGHT:
            player.GetPos().x -= (CHUNK_SIZE_X * 0.5f * BS);
            break;
        
    }
}