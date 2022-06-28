#pragma once

#include <vector>

class Ibo
{
private:
    std::vector<unsigned short> m_data;
    unsigned int m_id;
    
    void GenGLBuffer();

public:
    Ibo();
    ~Ibo();

    void Bind() const;
    void UnBind() const;

    void DynamicBufferData(const std::vector<unsigned short>& data, bool setData = true);
    void StaticBufferData(const std::vector<unsigned short>& data, bool setData = true);

    inline std::vector<unsigned short>& GetData() { return m_data; }
};