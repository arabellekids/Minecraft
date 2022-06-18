#pragma once

#include <string>

class Shader
{
private:
    unsigned int LoadShader(unsigned int type, const std::string& path);
    bool CompileShader();

    unsigned int m_id;
public:
    Shader(const std::string& vert, const std::string& frag);
    ~Shader();

    void Bind() const;
    void UnBind() const;
};