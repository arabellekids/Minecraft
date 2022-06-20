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

    inline const glm::vec3& GetPos() const { return m_pos; }
    inline const glm::vec3& GetRot() const { return m_rot; }
    inline const glm::vec3& GetScale() const { return m_scale; }

    inline const glm::mat4& GetModel() const { return m_model; }
};