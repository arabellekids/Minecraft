#include <GLES3/gl32.h>
#include <iostream>

#include "vbo.h"

Vbo::Vbo(const std::vector<Vertex>& data, unsigned int usage) : m_id(0)
{
    m_data = data;

    GenGLBuffer();
    Bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * data.size(), data.data(), usage);
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

void Vbo::SetData(const std::vector<Vertex>& data, bool setData)
{
    Bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * data.size(), data.data(), GL_DYNAMIC_DRAW);

    if(setData)
    {
        m_data = data;
    }
}