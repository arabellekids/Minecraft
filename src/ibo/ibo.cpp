#include <GLES3/gl32.h>
#include <iostream>

#include "ibo.h"

Ibo::Ibo(const std::vector<unsigned short>& data, unsigned int usage) : m_id(0)
{
    m_data = data;

    GenGLBuffer();
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * data.size(), data.data(), usage);
}
Ibo::~Ibo()
{
    glDeleteBuffers(1, &m_id);
}

void Ibo::GenGLBuffer()
{
    if(m_id == 0)
    {
        glGenBuffers(1, &m_id);
    }
    else
    {
        std::cout << "Trying to re-create an index buffer with id = " << m_id << "\n";
    }
}

void Ibo::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void Ibo::UnBind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Ibo::SetData(const std::vector<unsigned short>& data, bool setData)
{
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * data.size(), data.data(), GL_DYNAMIC_DRAW);

    if(setData)
    {
        m_data = data;
    }
}