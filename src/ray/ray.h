#pragma once

#include <glm/vec3.hpp>

class Ray
{
private:
    glm::vec3 m_origin;
    glm::vec3 m_direction;
    float m_length;
public:
    Ray(const glm::vec3& origin, const glm::vec3& dir, float length);

    const glm::vec3& GetOrigin() const { return m_origin; }
    glm::vec3& GetOrigin() { return m_origin; }

    const glm::vec3& GetDirection() const { return m_direction; }
    glm::vec3& GetDirection() { return m_direction; }
    
    const float& GetLength() const { return m_length; }
    float& GetLength() { return m_length; }
};

struct AxisPlane
{
    glm::ivec3 normal;
    float dist;
    float inc;

    bool operator< (const AxisPlane& b) { return dist < b.dist; }
};