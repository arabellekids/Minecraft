#pragma once

#include <vector>

struct Vertex
{
    float x;
    float y;
    float z;

    float u;
    float v;

    float light = 0.0f;
};

class Vbo
{
private:
    std::vector<Vertex> m_data;
    unsigned int m_id;
    
    void GenGLBuffer();

public:
    Vbo(const std::vector<Vertex>& data, unsigned int usage);
    ~Vbo();

    void Bind() const;
    void UnBind() const;

    void SetData(const std::vector<Vertex>& data, bool setData = true);
    inline std::vector<Vertex>& GetData() { return m_data; }
};