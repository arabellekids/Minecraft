#include <glm/ext/matrix_transform.hpp>

#include "skybox.h"

Skybox::Skybox() :
m_shader("assets/shaders/skybox/vert.glsl", "assets/shaders/skybox/frag.glsl"), m_texture("assets/textures/Skybox2.png", GL_LINEAR)
{
    m_skyTime = 1.0f;
    m_skyInc = -0.001f;

    MakeMesh();
    m_shader.SetUniform1f("u_tex", 0);
}

Skybox::Skybox(const std::string& tex) :
m_shader("assets/shaders/skybox/vert.glsl", "assets/shaders/skybox/frag.glsl"), m_texture(tex, GL_LINEAR)
{
    m_skyTime = 1.0f;
    m_skyInc = -0.001f;

    MakeMesh();
    m_shader.SetUniform1f("u_tex", 0);
}

Skybox::~Skybox() {}

void Skybox::Update()
{
    m_skyTime += m_skyInc;
    if(m_skyTime > 1.0f || m_skyTime < 0.0f)
    {
        m_skyInc = -m_skyInc;
        if(m_skyInc > 0) { m_skyTime = 0.0f; }
        else if(m_skyInc < 0) { m_skyTime = 1.0f; }
    }
}

void Skybox::Draw(const glm::mat4& vp, const glm::vec3& camPos)
{
    glm::mat4 model = glm::translate(glm::identity<glm::mat4>(), camPos);
    
    m_shader.Bind();
    m_texture.Bind();
    
    m_va.Bind();
    m_ib.Bind();
    
    m_shader.SetUniformMat4("u_mvp", vp * model);
    m_shader.SetUniform1f("u_time", m_skyTime);
    
    glDisable(GL_DEPTH_TEST);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);

    glEnable(GL_DEPTH_TEST);
}

void Skybox::MakeMesh()
{
    std::vector<glm::vec3> v = {
        { 1.0,  1.0, -1.0},
        { 1.0, -1.0, -1.0},
        { 1.0,  1.0,  1.0},
        { 1.0, -1.0,  1.0},
        {-1.0,  1.0, -1.0},
        {-1.0, -1.0, -1.0},
        {-1.0,  1.0,  1.0},
        {-1.0, -1.0,  1.0}
    };

    std::vector<glm::vec2> t = {
        {0.5,  0.25},
        {0.5,  0.0 },
        {0.25, 0.0 },
        {0.25, 0.25},
        {0.25, 0.5 },
        {0.0,  0.25},
        {0.0,  0.5 },
        {1.0,  0.5 },
        {1.0,  0.25},
        {0.75, 0.25},
        {0.75, 0.5 },
        {0.5,  0.75},
        {0.5,  0.5 },
        {0.25, 0.75}
    };

    std::vector<unsigned short> indices = {
        0, 1, 2,  0, 2, 3,
        4, 5, 6,  4, 6, 7,
        8, 9, 10,  8, 10, 11,
        12, 13, 14,  12, 14, 15,
        16, 17, 18,  16, 18, 19,
        20, 21, 22,  20, 22, 23
    };

    std::vector<Vertex> verts = {
        {v[0].x, v[0].y, v[0].z,  t[0].x - 0.001f, t[0].y - 0.001f, 15.0},
        {v[4].x, v[4].y, v[4].z,  t[1].x - 0.001f, t[1].y + 0.001f, 15.0},
        {v[6].x, v[6].y, v[6].z,  t[2].x + 0.001f, t[2].y + 0.001f, 15.0},
        {v[2].x, v[2].y, v[2].z,  t[3].x + 0.001f, t[3].y - 0.001f, 15.0},
        
        {v[3].x, v[3].y, v[3].z,  t[4].x - 0.001f, t[4].y - 0.001f, 15.0},
        {v[2].x, v[2].y, v[2].z,  t[3].x - 0.001f, t[3].y + 0.001f, 15.0},
        {v[6].x, v[6].y, v[6].z,  t[5].x + 0.001f, t[5].y + 0.001f, 15.0},
        {v[7].x, v[7].y, v[7].z,  t[6].x + 0.001f, t[6].y - 0.001f, 15.0},
        
        {v[7].x, v[7].y, v[7].z,  t[7].x - 0.001f, t[7].y - 0.001f, 15.0},
        {v[6].x, v[6].y, v[6].z,  t[8].x - 0.001f, t[8].y + 0.001f, 15.0},
        {v[4].x, v[4].y, v[4].z,  t[9].x + 0.001f, t[9].y + 0.001f, 15.0},
        {v[5].x, v[5].y, v[5].z,  t[10].x + 0.001f, t[10].y - 0.001f, 15.0},
        
        {v[5].x, v[5].y, v[5].z,  t[11].x - 0.001f, t[11].y - 0.001f, 15.0},
        {v[1].x, v[1].y, v[1].z,  t[12].x - 0.001f, t[12].y + 0.001f, 15.0},
        {v[3].x, v[3].y, v[3].z,  t[4].x + 0.001f, t[4].y + 0.001f, 15.0},
        {v[7].x, v[7].y, v[7].z,  t[13].x + 0.001f, t[13].y - 0.001f, 15.0},
        
        {v[1].x, v[1].y, v[1].z,  t[12].x - 0.001f, t[12].y - 0.001f, 15.0},
        {v[0].x, v[0].y, v[0].z,  t[0].x - 0.001f, t[0].y + 0.001f, 15.0},
        {v[2].x, v[2].y, v[2].z,  t[3].x + 0.001f, t[3].y + 0.001f, 15.0},
        {v[3].x, v[3].y, v[3].z,  t[4].x + 0.001f, t[4].y - 0.001f, 15.0},
        
        {v[5].x, v[5].y, v[5].z,  t[10].x - 0.001f, t[10].y - 0.001f, 15.0},
        {v[4].x, v[4].y, v[4].z,  t[9].x - 0.001f, t[9].y + 0.001f, 15.0},
        {v[0].x, v[0].y, v[0].z,  t[0].x + 0.001f, t[0].y + 0.001f, 15.0},
        {v[1].x, v[1].y, v[1].z,  t[12].x + 0.001f, t[12].y - 0.001f, 15.0}
    };

    m_vb.StaticBufferData(verts, true);
    m_ib.StaticBufferData(indices, true);
    
    BufferLayout layout;
    layout.Push<float>(3); // x,y,z
    layout.Push<float>(2); // u,v
    layout.Push<float>(1); // light
    
    m_va.AddBuffer(m_vb, layout);
}