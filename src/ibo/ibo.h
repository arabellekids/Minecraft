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

    void SetData(const std::vector<unsigned short>& data, bool setData = true);
    inline std::vector<unsigned short>& GetData() { return m_data; }
};