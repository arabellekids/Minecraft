#include <GLES3/gl32.h>

#include "world.h"

World::World() : m_chunks(16,16) ,m_blockShader("assets/shaders/test/vert.glsl", "assets/shaders/test/frag.glsl"), m_blockAtlasTex("assets/textures/Minecraft-atlas.png", GL_NEAREST)
{
    for(auto y = 0; y < m_chunks.GetYSize(); ++y)
    {
        for(auto x = 0; x < m_chunks.GetXSize(); ++x)
        {
            m_chunks(x,y).GenerateVertices();
            m_chunks(x,y).GenerateIndices( { 0.0f, 0.0f, 0.0f } );
        }
    }
}