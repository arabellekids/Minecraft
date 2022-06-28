#include "ray.h"

Ray::Ray(const glm::vec3& origin, const glm::vec3& dir, float length) : m_origin(origin), m_direction(dir), m_length(length)
{
}