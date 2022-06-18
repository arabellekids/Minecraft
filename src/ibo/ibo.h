#pragma once

#include <vector>

class Ibo
{
private:
    unsigned int m_id;
    
    void GenGLBuffer();

public:
    Ibo(const std::vector<unsigned short>& data, unsigned int usage);
    ~Ibo();

    void Bind() const;
    void UnBind() const;
};