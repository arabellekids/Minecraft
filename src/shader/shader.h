#pragma once

#include <glm/mat4x4.hpp>
#include <string>
#include <unordered_map>

class Shader
{
private:
    int GetUniformLocation(const std::string& name) const;

    unsigned int LoadShader(unsigned int type, const std::string& path);
    bool CompileShader();

    unsigned int m_id;
    mutable std::unordered_map<std::string, int> m_uniformLocations;

public:
    Shader(const std::string& vert, const std::string& frag);
    ~Shader();

    void Bind() const;
    void UnBind() const;

    void SetUniform1i(const std::string& name, int value) const;
    void SetUniform1f(const std::string& name, float value) const;
    void SetUniformMat4(const std::string& name, const glm::mat4& value) const;
};