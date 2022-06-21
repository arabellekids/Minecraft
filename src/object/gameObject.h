#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "baseObject.h"

class GameObject : public BaseObject
{
protected:
    glm::vec3 m_pos;
    glm::vec3 m_rot;
    glm::vec3 m_scale;

    glm::mat4 m_model;

    void CalcModel();
public:
    GameObject();
    virtual ~GameObject();

    const glm::vec3& GetPos() const { return m_pos; }
    const glm::vec3& GetRot() const { return m_rot; }
    const glm::vec3& GetScale() const { return m_scale; }
    
    const glm::mat4& GetModel() const { return m_model; }

    glm::vec3& GetPos() { return m_pos; }
    glm::vec3& GetRot() { return m_rot; }
    glm::vec3& GetScale() { return m_scale; }
    
    glm::mat4& GetModel() { return m_model; }
};