#pragma once

#include <vector>

class Ibo
{
private:
    std::vector<unsigned short> m_data;
    unsigned int m_id;
    
    void GenGLBuffer();

public:
    Ibo(const std::vector<unsigned short>& data, unsigned int usage);
    ~Ibo();

    void Bind() const;
    void UnBind() const;

    inline const std::vector<unsigned short>& GetData() const { return m_data; }
};