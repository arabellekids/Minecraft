#include <GLES3/gl32.h>
#include <iostream>

#include "vbo.h"

Vbo::Vbo() : m_id(0)
{
    GenGLBuffer();
}

Vbo::~Vbo()
{
    glDeleteBuffers(1, &m_id);
}

void Vbo::GenGLBuffer()
{
    if(m_id == 0)
    {
        glGenBuffers(1, &m_id);
    }
    else
    {
        std::cout << "Trying to re-create a vertex buffer with id = " << m_id << "\n";
    }
}    

void Vbo::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void Vbo::UnBind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Vbo::DynamicBufferData(const std::vector<Vertex>& data, bool setData)
{
    Bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * data.size(), data.data(), GL_DYNAMIC_DRAW);

    if(setData)
    {
        m_data = data;
    }
}

void Vbo::StaticBufferData(const std::vector<Vertex>& data, bool setData)
{
    Bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * data.size(), data.data(), GL_STATIC_DRAW);

    if(setData)
    {
        m_data = data;
    }
}