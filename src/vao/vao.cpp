#include <iostream>

#include "bufferLayout.h"
#include "vao.h"
#include "../vbo/vbo.h"

Vao::Vao() : m_id(0)
{
    GenGLArray();
}

Vao::~Vao()
{
    glDeleteVertexArrays(1, &m_id);
}

void Vao::GenGLArray()
{
    if(m_id == 0)
    {
        glGenVertexArrays(1, &m_id);
    }
    else
    {
        std::cout << "Trying to re-create a vertex array with id = " << m_id << "\n";
    }
}

void Vao::AddBuffer(const Vbo& vb, const BufferLayout& layout)
{
    Bind();
    vb.Bind();

    auto& elements = layout.GetElements();
    unsigned int stride = layout.GetStride();

    unsigned int offset = 0;
    for(unsigned int i = 0; i < elements.size(); i++)
    {
        auto& element = elements[i];
        
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, element.count, element.type, element.normalized, stride, (void*)offset);
        offset += element.size;
    }
}

void Vao::Bind() const
{
    glBindVertexArray(m_id);
}

void Vao::UnBind() const
{
    glBindVertexArray(0);
}
