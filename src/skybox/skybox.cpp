#include <glm/ext/matrix_transform.hpp>

#include "skybox.h"

Skybox::Skybox() :
m_shader("assets/shaders/skybox/vert.glsl", "assets/shaders/skybox/frag.glsl"), m_texture("assets/textures/Skybox1.png", GL_LINEAR),
m_ib({}, GL_STATIC_DRAW), m_vb({}, GL_STATIC_DRAW)
{
    MakeMesh();
    m_shader.SetUniform1f("u_tex", 0);
}

Skybox::Skybox(const std::string& tex) :
m_shader("assets/shaders/skybox/vert.glsl", "assets/shaders/skybox/frag.glsl"), m_texture(tex, GL_LINEAR),
m_ib({}, GL_STATIC_DRAW), m_vb({}, GL_STATIC_DRAW)
{
    MakeMesh();
    m_shader.SetUniform1f("u_tex", 0);
}

Skybox::~Skybox() {}

void Skybox::Draw(const glm::mat4& vp, const glm::vec3& camPos, float time)
{
    glm::mat4 model = glm::translate(glm::identity<glm::mat4>(), camPos);
    
    m_shader.Bind();
    m_texture.Bind();
    
    m_ib.Bind();
    m_va.Bind();
    
    m_shader.SetUniformMat4("u_mvp", vp * model);
    m_shader.SetUniform1f("u_time", time);
    
    glDisable(GL_DEPTH_TEST);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);

    glEnable(GL_DEPTH_TEST);
}

void Skybox::MakeMesh()
{
    std::vector<Vertex> verts = {
        // North
        {-0.5,  0.5, 0.5,  0.0 + 0.01,  0.25 + 0.01},
        {-0.5, -0.5, 0.5,  0.0 + 0.01,  0.5 - 0.01 },
        { 0.5, -0.5, 0.5,  0.25 - 0.01, 0.5 - 0.01 },
        { 0.5,  0.5, 0.5,  0.25 - 0.01, 0.25 + 0.01},

        // South
        { 0.5,  0.5, -0.5,  0.5 + 0.01,  0.25 + 0.01},
        { 0.5, -0.5, -0.5,  0.5 + 0.01,  0.5 - 0.01 },
        {-0.5, -0.5, -0.5,  0.75 - 0.01, 0.5 - 0.01 },
        {-0.5,  0.5, -0.5,  0.75 - 0.01, 0.25 + 0.01},

        // East
        { 0.5,  0.5,  0.5,  0.5 - 0.01,  0.25 + 0.01},
        { 0.5, -0.5,  0.5,  0.5 - 0.01,  0.5 - 0.01 },
        { 0.5, -0.5, -0.5,  0.25 + 0.01, 0.5 - 0.01 },
        { 0.5,  0.5, -0.5,  0.25 + 0.01, 0.25 + 0.01},
        
        // West
        {-0.5,  0.5, -0.5,  0.5 + 0.01,  0.25 + 0.01},
        {-0.5, -0.5, -0.5,  0.5 + 0.01,  0.5 - 0.01 },
        {-0.5, -0.5,  0.5,  0.75 - 0.01, 0.5 - 0.01 },
        {-0.5,  0.5,  0.5,  0.75 - 0.01, 0.25 + 0.01},
        
        // Up
        { 0.5,  0.5,  0.5,  0.5 - 0.01,  0.0 + 0.01 },
        { 0.5,  0.5, -0.5,  0.5 - 0.01,  0.25 - 0.01},
        {-0.5,  0.5, -0.5,  0.25 + 0.01, 0.25 - 0.01},
        {-0.5,  0.5,  0.5,  0.25 + 0.01, 0.0 + 0.01 },
        
        // Down
        {-0.5, -0.5,  0.5,  0.25 + 0.01, 0.5 + 0.01 },
        {-0.5, -0.5, -0.5,  0.25 + 0.01, 0.75 - 0.01},
        { 0.5, -0.5, -0.5,  0.5 - 0.01,  0.75 - 0.01},
        { 0.5, -0.5,  0.5,  0.5 - 0.01,  0.5 + 0.01 }
    };

    std::vector<unsigned short> indices = {
        0,1,2,
        0,2,3,
        
        4,5,6,
        4,6,7,
        
        8,9,10,
        8,10,11,
        
        12,13,14,
        12,14,15,
        
        16,17,18,
        16,18,19,
        
        20,21,22,
        20,22,23
    };

    m_vb.SetData(verts);
    m_ib.SetData(indices);
    
    BufferLayout layout;
    layout.Push<float>(3); // x,y,z
    layout.Push<float>(2); // u,v
    layout.Push<float>(1); // light
    
    m_va.AddBuffer(m_vb, layout);
}