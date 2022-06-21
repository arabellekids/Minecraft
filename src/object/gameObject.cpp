#include <glm/ext/matrix_transform.hpp>

#include "gameObject.h"

GameObject::GameObject() : m_pos(0.0f, 0.0f, 0.0f), m_rot(0.0f, 0.0f, 0.0f), m_scale(1.0f, 1.0f, 1.0f)
{
    m_model = glm::identity<glm::mat4>();
}
GameObject::~GameObject() {}

void GameObject::CalcModel()
{
    m_model = glm::scale(
        glm::rotate(
            glm::rotate(
                glm::translate(glm::identity<glm::mat4>(), m_pos), m_rot.y  * 0.01745329251994329576923690768489f, glm::vec3(0.0f, 1.0f, 0.0f)
            ), m_rot.x  * 0.01745329251994329576923690768489f, glm::vec3(1.0f, 0.0f, 0.0f)
        ), m_scale
    );
}