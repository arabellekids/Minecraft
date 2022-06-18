#pragma once

#include "bufferLayout.h"

class Vbo;

class Vao
{
private:
    unsigned int m_id;

    void GenGLArray();
public:
    Vao();
    ~Vao();

    void AddBuffer(const Vbo& vb, const BufferLayout& layout);

    void Bind() const;
    void UnBind() const;
};