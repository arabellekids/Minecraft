#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class GameObject
{
protected:
    glm::vec3 m_pos;
    glm::vec3 m_rot;
    glm::vec3 m_scale;

    glm::mat4 m_model;

public:
    GameObject();
    virtual ~GameObject();

    void CalcModel();

    const glm::vec3& GetPos() const { return m_pos; }
    const glm::vec3& GetRot() const { return m_rot; }
    const glm::vec3& GetScale() const { return m_scale; }
    
    const glm::mat4& GetModel() const { return m_model; }

    glm::vec3& GetPos() { return m_pos; }
    glm::vec3& GetRot() { return m_rot; }
    glm::vec3& GetScale() { return m_scale; }
    
    glm::mat4& GetModel() { return m_model; }
};