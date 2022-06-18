#pragma once

#include <vector>

struct Vertex
{
    float x;
    float y;
    float z;
};

class Vbo
{
private:
    unsigned int m_id;
    
    void GenGLBuffer();

public:
    Vbo(const std::vector<Vertex>& data, unsigned int usage);
    ~Vbo();

    void Bind() const;
    void UnBind() const;
};