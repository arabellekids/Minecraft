#pragma once

#include <string>

class Texture
{
public:
    Texture(const std::string& path, unsigned int filterMode);
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void UnBind() const;

private:
    unsigned int m_rendererID;
};